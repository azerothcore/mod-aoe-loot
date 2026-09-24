/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "aoe_loot.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "ObjectMgr.h"
#include "World.h"
#include <algorithm>
#include <limits>

std::map<uint64, bool> AoeLootCommandScript::playerAoeLootEnabled;

namespace
{
    constexpr uint32 AOE_LOOT_STACK_LIMIT = 200;

    bool MailEnabled = false;

    // Loot window opened by the module, kept on the player.
    struct AoeLootWindow : public DataMap::Base
    {
        ObjectGuid CreatureGuid;
    };

    std::string const AOE_LOOT_WINDOW_KEY = "mod-aoe-loot.window";

    void TrackAoeLootWindow(Player* player, ObjectGuid creatureGuid)
    {
        player->CustomData.GetDefault<AoeLootWindow>(
            AOE_LOOT_WINDOW_KEY)->CreatureGuid = creatureGuid;
    }

    void ForgetAoeLootWindow(Player* player)
    {
        player->CustomData.Erase(AOE_LOOT_WINDOW_KEY);
    }

    bool IsAoeLootWindow(Player const* player, ObjectGuid lootGuid)
    {
        if (!lootGuid)
            return false;

        AoeLootWindow const* window =
            player->CustomData.Get<AoeLootWindow>(AOE_LOOT_WINDOW_KEY);
        return window && window->CreatureGuid == lootGuid;
    }

    // Same "no maximum" test as Player::CanTakeMoreSimilarItems.
    bool HasCarryLimit(ItemTemplate const* proto)
    {
        return (proto->MaxCount > 0 || proto->ItemLimitCategory) &&
            proto->MaxCount != std::numeric_limits<int32>::max();
    }

    // Item::CreateItem clamps the count to one stack, so split the row.
    bool CreateMailItems(
        Player* player,
        ItemTemplate const* proto,
        LootItem const& lootItem,
        std::vector<Item*>& mailItems)
    {
        uint32 maxStack = std::max<uint32>(proto->GetMaxStackSize(), 1);
        uint32 remaining = lootItem.count;

        while (remaining > 0)
        {
            uint32 pieceCount = std::min(remaining, maxStack);
            Item* item = Item::CreateItem(
                lootItem.itemid,
                pieceCount,
                player,
                false,
                lootItem.randomPropertyId);
            if (!item)
            {
                for (Item* created : mailItems)
                    delete created;

                mailItems.clear();
                return false;
            }

            // Random properties queue the item for the owner's inventory
            // save, which deletes an item it can't find in the bags.
            item->RemoveFromUpdateQueueOf(player);

            mailItems.push_back(item);
            remaining -= pieceCount;
        }

        return !mailItems.empty();
    }

    // Mirrors SendRollWonItemViaMail in Group.cpp.
    void SendMailItems(
        Player* player,
        LootItem const& lootItem,
        std::vector<Item*> const& mailItems)
    {
        AllowedLooterSet looters = lootItem.GetAllowedLooters();

        for (Item* item : mailItems)
        {
            ItemTemplate const* proto = item->GetTemplate();
            // Preserve the 2-hour group trade window the item would have
            // had if stored directly.
            if (looters.size() > 1 && proto->GetMaxStackSize() == 1 &&
                (proto->Bonding == BIND_WHEN_PICKED_UP ||
                    proto->Bonding == BIND_QUEST_ITEM) &&
                sWorld->getBoolConfig(CONFIG_SET_BOP_ITEM_TRADEABLE))
            {
                item->SetBinding(true);
                item->SetSoulboundTradeable(looters);
                item->SetUInt32Value(
                    ITEM_FIELD_CREATE_PLAYED_TIME,
                    player->GetTotalPlayedTime());

                std::string lootersStr;
                for (ObjectGuid const& guid : looters)
                {
                    if (!lootersStr.empty())
                        lootersStr += ' ';
                    lootersStr += std::to_string(guid.GetCounter());
                }

                CharacterDatabasePreparedStatement* stmt =
                    CharacterDatabase.GetPreparedStatement(
                        CHAR_INS_ITEM_BOP_TRADE);
                stmt->SetData(0, item->GetGUID().GetCounter());
                stmt->SetData(1, lootersStr);
                CharacterDatabase.Execute(stmt);
            }

            player->SendItemRetrievalMail(item);
        }
    }

    bool CanStackRegularLoot(LootItem const& existingItem, LootItem const& incomingItem)
    {
        return !existingItem.is_looted &&
            !incomingItem.is_looted &&
            !existingItem.freeforall &&
            !incomingItem.freeforall &&
            !existingItem.needs_quest &&
            !incomingItem.needs_quest &&
            !existingItem.is_blocked &&
            !incomingItem.is_blocked &&
            !existingItem.follow_loot_rules &&
            !incomingItem.follow_loot_rules &&
            existingItem.conditions.empty() &&
            incomingItem.conditions.empty() &&
            !existingItem.rollWinnerGUID &&
            !incomingItem.rollWinnerGUID &&
            existingItem.itemid == incomingItem.itemid &&
            existingItem.randomSuffix == incomingItem.randomSuffix &&
            existingItem.randomPropertyId == incomingItem.randomPropertyId &&
            existingItem.is_underthreshold == incomingItem.is_underthreshold &&
            existingItem.allowedGUIDs == incomingItem.allowedGUIDs;
    }

     uint32 AddOrStackRegularLoot(
        Loot* mainLoot,
        LootItem const& incomingItem,
        size_t reservedQuestRows)
    {
        ItemTemplate const* itemTemplate =
            sObjectMgr->GetItemTemplate(incomingItem.itemid);

        uint32 stackLimit = itemTemplate
            ? std::min<uint32>(
                itemTemplate->GetMaxStackSize(),
                AOE_LOOT_STACK_LIMIT)
            : 1;

        bool canStack = stackLimit > 1 &&
            !incomingItem.freeforall &&
            !incomingItem.needs_quest &&
            !incomingItem.is_blocked &&
            !incomingItem.follow_loot_rules &&
            incomingItem.conditions.empty() &&
            !incomingItem.rollWinnerGUID;

        uint32 remaining = incomingItem.count;
        uint32 transferred = 0;

        if (canStack)
        {
            for (LootItem& existingItem : mainLoot->items)
            {
                if (!CanStackRegularLoot(existingItem, incomingItem) ||
                    existingItem.count >= stackLimit)
                {
                    continue;
                }

                uint32 amountToAdd = std::min<uint32>(
                    remaining,
                    stackLimit - existingItem.count);

                existingItem.count =
                    static_cast<uint8>(
                        existingItem.count + amountToAdd);

                remaining -= amountToAdd;
                transferred += amountToAdd;

                if (remaining == 0)
                    return transferred;
            }
        }

        while (remaining > 0)
        {
            if (mainLoot->items.size() + reservedQuestRows >=
                MAX_LOOT_ITEMS)
            {
                return transferred;
            }

            LootItem newItem = incomingItem;

            uint32 newStackCount = canStack
                ? std::min<uint32>(remaining, stackLimit)
                : remaining;

            newItem.count = static_cast<uint8>(newStackCount);
            newItem.itemIndex =
                static_cast<uint32>(mainLoot->items.size());
            newItem.is_looted = false;
            newItem.is_counted = false;

            mainLoot->items.push_back(newItem);
            transferred += newStackCount;

            if (!newItem.freeforall &&
                newItem.conditions.empty() &&
                !newItem.needs_quest)
            {
                ++mainLoot->unlootedCount;
            }

            remaining -= newStackCount;

            if (!canStack)
                break;
        }

        return transferred;
    }
    
    uint8 GetLootSortPriority(LootItem const& item)
    {
        ItemTemplate const* itemTemplate =
            sObjectMgr->GetItemTemplate(item.itemid);

        // Unknown templates behave like ordinary normal items.
        if (!itemTemplate)
            return 5;

        // Quality is checked first, regardless of item type.
        switch (itemTemplate->Quality)
        {
            case ITEM_QUALITY_HEIRLOOM:
            case ITEM_QUALITY_ARTIFACT:
            case ITEM_QUALITY_LEGENDARY:
                return 0;

            case ITEM_QUALITY_EPIC:
                return 1;

            case ITEM_QUALITY_RARE:
                return 2;

            case ITEM_QUALITY_UNCOMMON:
                return 3;

            case ITEM_QUALITY_POOR:
                return 6;

            case ITEM_QUALITY_NORMAL:
            default:
                break;
        }

        // Normal crafting materials.
        if (itemTemplate->Class == ITEM_CLASS_TRADE_GOODS ||
            itemTemplate->Class == ITEM_CLASS_REAGENT ||
            itemTemplate->Class == ITEM_CLASS_GEM)
        {
            return 4;
        }

        // Normal food and drinks go below grey items.
        if (itemTemplate->Class == ITEM_CLASS_CONSUMABLE &&
            itemTemplate->SubClass == ITEM_SUBCLASS_FOOD)
        {
            return 7;
        }

        // Only normal-quality recipes reach this point.
        if (itemTemplate->Class == ITEM_CLASS_RECIPE)
            return 8;

        return 5;
    }

    bool HasSimpleRegularLootRules(LootItem const& item)
    {
        return !item.freeforall &&
            !item.needs_quest &&
            !item.is_blocked &&
            !item.follow_loot_rules &&
            item.conditions.empty() &&
            !item.rollWinnerGUID;
    }

    bool CanSafelySortLootItem(LootItem const& item)
    {
        return !item.is_looted &&
            HasSimpleRegularLootRules(item);
    }

    // Rolls and master loot point at rows by index.
    bool HasPendingRoll(Loot const& loot)
    {
        auto isPending = [](LootItem const& item)
        {
            return !item.is_looted &&
                (item.is_blocked || item.rollWinnerGUID);
        };

        return std::any_of(
                loot.items.begin(),
                loot.items.end(),
                isPending) ||
            std::any_of(
                loot.quest_items.begin(),
                loot.quest_items.end(),
                isPending);
    }

    bool IsReservedByGroupLootRules(
        Player const* player,
        Loot const& loot,
        LootItem const& item)
    {
        Group const* group = player->GetGroup();

        if (!group || group->GetLootMethod() == FREE_FOR_ALL)
            return false;

        // Rolls only start when the corpse is first opened.
        if (group->GetLootMethod() != ROUND_ROBIN &&
            !item.is_underthreshold)
        {
            return true;
        }

        if (!loot.roundRobinPlayer ||
            loot.roundRobinPlayer == player->GetGUID())
        {
            return false;
        }

        return group->GetLootMethod() == ROUND_ROBIN ||
            item.is_underthreshold;
    }

    void CompactTransferredRegularLoot(Loot* loot)
    {
        if (!loot ||
            !std::all_of(
                loot->items.begin(),
                loot->items.end(),
                HasSimpleRegularLootRules))
        {
            return;
        }

        loot->items.erase(
            std::remove_if(
                loot->items.begin(),
                loot->items.end(),
                [](LootItem const& item)
                {
                    return item.is_looted;
                }),
            loot->items.end());

        for (size_t i = 0; i < loot->items.size(); ++i)
            loot->items[i].itemIndex = static_cast<uint32>(i);
    }

    void SortRegularLoot(Loot* loot)
    {
        if (!loot ||
            !std::all_of(
                loot->items.begin(),
                loot->items.end(),
                CanSafelySortLootItem))
        {
            return;
        }

        std::stable_sort(
            loot->items.begin(),
            loot->items.end(),
            [](LootItem const& left, LootItem const& right)
            {
                return GetLootSortPriority(left) <
                    GetLootSortPriority(right);
            });

        // Sorting moved the items, so their stored indices must match
        // their new positions.
        for (size_t i = 0; i < loot->items.size(); ++i)
            loot->items[i].itemIndex = static_cast<uint32>(i);
    }
}

void AOELootPlayer::OnPlayerLogin(Player* player)
{
    if (!player)
        return;

    if (sConfigMgr->GetOption<bool>("AOELoot.Enable", true) && sConfigMgr->GetOption<bool>("AOELoot.Message", true))
        if (WorldSession* session = player->GetSession())
            ChatHandler(session).PSendModuleSysMessage(MODULE_STRING, AOE_LOGIN_MESSAGE);
}

void AOELootWorld::OnAfterConfigLoad(bool /*reload*/)
{
    MailEnabled = sConfigMgr->GetOption<bool>("AOELoot.MailEnable", false);
}

bool AOELootServer::CanPacketReceive(WorldSession* session, WorldPacket const& packet)
{
    // Only handle loot open, loot release and item pickup packets
    uint16 opcode = packet.GetOpcode();
    if (opcode != CMSG_LOOT &&
        opcode != CMSG_LOOT_RELEASE &&
        opcode != CMSG_AUTOSTORE_LOOT_ITEM)
        return true;

    // Basic validation checks
    if (!session)
        return true;

    Player* player = session->GetPlayer();
    if (!player)
        return true;

    if (opcode == CMSG_LOOT_RELEASE)
    {
        ForgetAoeLootWindow(player);
        return true;
    }

    if (opcode == CMSG_AUTOSTORE_LOOT_ITEM)
        return HandleAutostoreLootItem(player, packet);

    // A new loot window replaces the tracked one
    ForgetAoeLootWindow(player);

    // Check if module is enabled
    if (!sConfigMgr->GetOption<bool>("AOELoot.Enable", true))
        return true;

    // Check if player has AOE loot disabled via command
    uint64 playerGuid = player->GetGUID().GetRawValue();
    if (AoeLootCommandScript::hasPlayerAoeLootEnabled(playerGuid) &&
        !AoeLootCommandScript::getPlayerAoeLootEnabled(playerGuid))
        return true;

    // Check group settings
    if (player->GetGroup() && !sConfigMgr->GetOption<bool>("AOELoot.Group", true))
        return true;

    // Get configured loot range
    float range = sConfigMgr->GetOption<float>("AOELoot.Range", 55.0f);

    // Limit range to reasonable values
    if (range < 5.0f)
        range = 5.0f;

    if (range > 100.0f)
        range = 100.0f;

    // Read target GUID from packet
    WorldPacket packetCopy(packet);
    ObjectGuid targetGuid;
    packetCopy >> targetGuid;

    if (!targetGuid)
        return true;

    // Get target creature
    Creature* mainCreature = player->GetMap()->GetCreature(targetGuid);
    if (!mainCreature)
        return true;

    // Check if main creature has loot
    if (!mainCreature->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE))
        return true;

    if (HasPendingRoll(mainCreature->loot))
        return true;

    // Get nearby corpses
    std::list<Creature*> nearbyCorpses;
    player->GetDeadCreatureListInGrid(nearbyCorpses, range);

    // Remove invalid corpses and main target
    nearbyCorpses.remove_if([&](Creature* c)
        {
            return !c ||
                c->GetGUID() == targetGuid ||
                !c->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE) ||
                c->loot.loot_type == LOOT_SKINNING ||
                HasPendingRoll(c->loot) ||
                !player->isAllowedToLoot(c);
        });

    // If no other corpses, process normally
    if (nearbyCorpses.empty())
    {
        player->SendLoot(targetGuid, LOOT_CORPSE);
        TrackAoeLootWindow(player, targetGuid);
        return false;
    }

    // Get main loot
    Loot* mainLoot = &mainCreature->loot;

    // Track total gold to merge
    uint32 totalGold = mainLoot->gold;

    struct RegularLootCandidate
    {
        Creature* sourceCreature;
        size_t sourceIndex;
        LootItem item;
    };

    struct QuestLootCandidate
    {
        Creature* sourceCreature;
        size_t sourceIndex;
        LootItem item;
    };

    std::vector<Creature*> processedCreatures;
    std::vector<RegularLootCandidate> regularCandidates;
    std::vector<QuestLootCandidate> questCandidates;

    for (Creature* creature : nearbyCorpses)
    {

        if (!creature)
            continue;

        Loot* loot = &creature->loot;

        // Skip already looted corpses.
        if (loot->isLooted())
            continue;

        processedCreatures.push_back(creature);

        // Gold does not consume a loot-window row.
        if (loot->gold > 0)
        {
            if (loot->gold <=
                std::numeric_limits<uint32>::max() - totalGold)
            {
                totalGold += loot->gold;
                loot->gold = 0;
            }
        }

        // Collect safe regular items as candidates. Do not remove
        // anything from the source corpse yet.
        for (size_t i = 0; i < loot->items.size(); ++i)
        {
            LootItem const& item = loot->items[i];

            if (!CanSafelySortLootItem(item))
                continue;

            // The row ends up on the clicked corpse.
            if (!item.AllowedForPlayer(
                    player, mainLoot->sourceWorldObjectGUID))
            {
                continue;
            }

            if (IsReservedByGroupLootRules(player, *loot, item) ||
                IsReservedByGroupLootRules(player, *mainLoot, item))
            {
                continue;
            }

            regularCandidates.push_back(
                { creature, i, item });
        }

        // Collect safe quest items needed by the player.
        for (size_t i = 0; i < loot->quest_items.size(); ++i)
        {
            LootItem const& questItem = loot->quest_items[i];

            if (!player->HasQuestForItem(questItem.itemid))
                continue;

            // Special group/free-for-all quest items remain on their
            // source corpse so we don't damage ownership information.
            if (questItem.freeforall ||
                questItem.is_blocked ||
                questItem.follow_loot_rules ||
                !questItem.conditions.empty() ||
                questItem.rollWinnerGUID)
            {
                continue;
            }

            // Quest rows are only filled for looters present at the kill.
            if (!questItem.AllowedForPlayer(
                    player, loot->sourceWorldObjectGUID) ||
                !questItem.GetAllowedLooters().count(player->GetGUID()))
            {
                continue;
            }

            uint32 maxNeeded = 0;

            for (uint8 slot = 0;
                 slot < MAX_QUEST_LOG_SIZE;
                 ++slot)
            {
                uint32 questId =
                    player->GetQuestSlotQuestId(slot);

                if (!questId)
                    continue;

                Quest const* quest =
                    sObjectMgr->GetQuestTemplate(questId);

                if (!quest)
                    continue;

                for (uint8 j = 0;
                     j < QUEST_ITEM_OBJECTIVES_COUNT;
                     ++j)
                {
                    if (quest->RequiredItemId[j] ==
                            questItem.itemid &&
                        quest->RequiredItemCount[j] >
                            maxNeeded)
                    {
                        maxNeeded =
                            quest->RequiredItemCount[j];
                    }
                }
            }

            if (maxNeeded == 0)
                continue;

            uint32 ownedCount =
                player->GetItemCount(questItem.itemid, true);

            for (QuestLootCandidate const& pending :
                 questCandidates)
            {
                if (pending.item.itemid == questItem.itemid)
                    ownedCount += pending.item.count;
            }

            for (LootItem const& mainQuestItem :
                 mainLoot->quest_items)
            {
                if (mainQuestItem.itemid == questItem.itemid)
                    ownedCount += mainQuestItem.count;
            }

            if (ownedCount >= maxNeeded)
                continue;

            uint32 stillNeeded = maxNeeded - ownedCount;

            LootItem cappedItem = questItem;
            cappedItem.count = std::min(
                static_cast<uint32>(questItem.count),
                stillNeeded);

            questCandidates.push_back(
                { creature, i, cappedItem });
        }
    }

    // Prioritize every regular candidate before consuming rows.
    std::stable_sort(
        regularCandidates.begin(),
        regularCandidates.end(),
        [](RegularLootCandidate const& left,
           RegularLootCandidate const& right)
        {
            return GetLootSortPriority(left.item) <
                GetLootSortPriority(right.item);
        });

    // Quest rows already belonging to the selected corpse still
    // occupy client loot-window capacity.
    size_t reservedQuestRows =
        mainLoot->quest_items.size();

    // Transfer regular candidates in priority order.
    for (RegularLootCandidate const& candidate :
         regularCandidates)
    {
        Loot* sourceLoot =
            &candidate.sourceCreature->loot;

        if (candidate.sourceIndex >=
            sourceLoot->items.size())
        {
            continue;
        }

        LootItem& sourceItem =
            sourceLoot->items[candidate.sourceIndex];

        if (sourceItem.is_looted)
            continue;

        uint32 sourceCount = sourceItem.count;

        uint32 transferred = AddOrStackRegularLoot(
            mainLoot,
            sourceItem,
            reservedQuestRows);

        // Nothing fit. Leave the complete item on its corpse.
        if (transferred == 0)
            continue;

        if (transferred >= sourceCount)
        {
            // The complete source row was transferred.
            sourceItem.is_looted = true;

            if (sourceLoot->unlootedCount > 0)
                --sourceLoot->unlootedCount;
        }
        else
        {
            // Only part of the stack fitted. Preserve the remainder.
            sourceItem.count = static_cast<uint8>(
                sourceCount - transferred);
        }
    }

    // Add safe quest items directly to inventory. Their source rows
    // are changed only after AddItem reports success.
    for (QuestLootCandidate const& candidate :
         questCandidates)
    {
        if (!player->HasQuestForItem(
                candidate.item.itemid))
        {
            continue;
        }

        Loot* sourceLoot =
            &candidate.sourceCreature->loot;

        if (candidate.sourceIndex >=
            sourceLoot->quest_items.size())
        {
            continue;
        }

        LootItem& sourceItem =
            sourceLoot->quest_items[
                candidate.sourceIndex];

        if (sourceItem.is_looted)
            continue;

        uint32 amountToAdd = std::min<uint32>(
            candidate.item.count,
            sourceItem.count);

        if (amountToAdd == 0 ||
            !player->AddItem(
                sourceItem.itemid,
                amountToAdd))
        {
            continue;
        }

        if (amountToAdd >= sourceItem.count)
        {
            sourceItem.is_looted = true;

            if (sourceLoot->unlootedCount > 0)
                --sourceLoot->unlootedCount;
        }
        else
        {
            sourceItem.count = static_cast<uint8>(
                sourceItem.count - amountToAdd);
        }
    }

    // Apply all successfully collected gold to the selected corpse.
    mainLoot->gold = totalGold;

    // Remove only rows that were successfully transferred.
    // Rejected overflow rows remain on their source corpses.
    for (Creature* creature : processedCreatures)
    {
        Loot* loot = &creature->loot;

        CompactTransferredRegularLoot(loot);

        if (!loot->isLooted())
            continue;

        creature->AllLootRemovedFromCorpse();
        creature->RemoveDynamicFlag(
            UNIT_DYNFLAG_LOOTABLE);
        loot->clear();
    }
    
    // Organize regular loot before sending the window.
    SortRegularLoot(mainLoot);

    // Send merged loot window
    player->SendLoot(targetGuid, LOOT_CORPSE);
    TrackAoeLootWindow(player, targetGuid);

    return false;
}

bool AOELootServer::HandleAutostoreLootItem(
    Player* player,
    WorldPacket const& packet)
{
    if (!MailEnabled)
        return true;

    if (packet.size() < 1)
        return true;

    WorldPacket packetCopy(packet);
    uint8 lootSlot = 0;
    packetCopy >> lootSlot;

    // Only windows the module opened itself
    ObjectGuid lootGuid = player->GetLootGUID();
    if (!IsAoeLootWindow(player, lootGuid))
        return true;

    // Every check below is at least as strict as the core pickup path,
    // so any failure leaves the packet to the core handler.
    if (player->HasPlayerFlag(PLAYER_FLAGS_NO_PLAY_TIME))
        return true;

    Creature* creature = player->GetMap()->GetCreature(lootGuid);
    if (!creature ||
        creature->IsAlive() ||
        !creature->IsWithinDistInMap(player, INTERACTION_DISTANCE) ||
        creature->loot.loot_type != LOOT_CORPSE)
        return true;

    Loot* loot = &creature->loot;
    QuestItem* qitem = nullptr;
    QuestItem* ffaitem = nullptr;
    QuestItem* conditem = nullptr;

    LootItem* item = loot->LootItemInSlot(
        lootSlot, player, &qitem, &ffaitem, &conditem);
    if (!item || item->is_looted)
        return true;

    Group const* group = player->GetGroup();
    if (!item->is_underthreshold &&
        loot->roundRobinPlayer &&
        group &&
        group->GetLootMethod() == MASTER_LOOT &&
        player->GetGUID() != group->GetMasterLooterGuid() &&
        !qitem &&
        !ffaitem &&
        !conditem)
        return true;

    if (!item->AllowedForPlayer(player, loot->sourceWorldObjectGUID))
        return true;

    // Roll pending
    if (!qitem && item->is_blocked)
        return true;

    if (item->rollWinnerGUID && item->rollWinnerGUID != player->GetGUID())
        return true;

    // Mail does not count towards carry limits, so limited items keep the
    // core error.
    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid);
    if (!proto || HasCarryLimit(proto))
        return true;

    // Only a lack of bag space is handled here.
    ItemPosCountVec dest;
    if (player->CanStoreNewItem(
            NULL_BAG, NULL_SLOT, dest, item->itemid, item->count) !=
        EQUIP_ERR_INVENTORY_FULL)
        return true;

    std::vector<Item*> mailItems;
    if (!CreateMailItems(player, proto, *item, mailItems))
        return true;

    sScriptMgr->OnPlayerAfterCreatureLoot(player);

    // Remove the row as Player::StoreLootItem does on success
    if (qitem)
    {
        qitem->is_looted = true;
        if (item->freeforall || loot->GetPlayerQuestItems().size() == 1)
            player->SendNotifyLootItemRemoved(lootSlot);
        else
            loot->NotifyQuestItemRemoved(qitem->index);
    }
    else if (ffaitem)
    {
        ffaitem->is_looted = true;
        player->SendNotifyLootItemRemoved(lootSlot);
    }
    else
    {
        if (conditem)
            conditem->is_looted = true;

        loot->NotifyItemRemoved(lootSlot);
    }

    if (!item->freeforall)
        item->is_looted = true;

    --loot->unlootedCount;

    SendMailItems(player, *item, mailItems);

    ChatHandler(player->GetSession()).PSendModuleSysMessage(
        MODULE_STRING, AOE_ITEM_IN_THE_MAIL);

    return false;
}

ChatCommandTable AoeLootCommandScript::GetCommands() const
{
    static ChatCommandTable aoeLootSubCommandTable =
    {
        { "on", HandleAoeLootOnCommand, SEC_PLAYER, Console::No },
        { "off", HandleAoeLootOffCommand, SEC_PLAYER, Console::No }
    };

    static ChatCommandTable aoeLootCommandTable =
    {
        { "aoeloot", aoeLootSubCommandTable }
    };

    return aoeLootCommandTable;
}

bool AoeLootCommandScript::hasPlayerAoeLootEnabled(uint64 guid)
{
    return playerAoeLootEnabled.count(guid) > 0;
}

bool AoeLootCommandScript::getPlayerAoeLootEnabled(uint64 guid)
{
    auto it = playerAoeLootEnabled.find(guid);
    if (it != playerAoeLootEnabled.end())
        return it->second;
    return false;
}

void AoeLootCommandScript::setPlayerAoeLootEnabled(uint64 guid, bool mode)
{
    playerAoeLootEnabled[guid] = mode;
}

bool AoeLootCommandScript::HandleAoeLootOnCommand(ChatHandler* handler, Optional<std::string> /*args*/)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return true;

    uint64 playerGuid = player->GetGUID().GetRawValue();

    if (AoeLootCommandScript::hasPlayerAoeLootEnabled(playerGuid) &&
        AoeLootCommandScript::getPlayerAoeLootEnabled(playerGuid))
    {
        handler->PSendModuleSysMessage(MODULE_STRING, AOE_LOOT_ALREADY_ENABLED);
        return true;
    }

    AoeLootCommandScript::setPlayerAoeLootEnabled(playerGuid, true);
    handler->PSendModuleSysMessage(MODULE_STRING, AOE_LOOT_ENABLED);
    return true;
}

bool AoeLootCommandScript::HandleAoeLootOffCommand(ChatHandler* handler, Optional<std::string> /*args*/)
{
    Player* player = handler->GetSession()->GetPlayer();
    if (!player)
        return true;

    uint64 playerGuid = player->GetGUID().GetRawValue();

    if (AoeLootCommandScript::hasPlayerAoeLootEnabled(playerGuid) &&
        !AoeLootCommandScript::getPlayerAoeLootEnabled(playerGuid))
    {
        handler->PSendModuleSysMessage(MODULE_STRING, AOE_LOOT_ALREADY_DISABLED);
        return true;
    }

    AoeLootCommandScript::setPlayerAoeLootEnabled(playerGuid, false);
    handler->PSendModuleSysMessage(MODULE_STRING, AOE_LOOT_DISABLED);
    return true;
}
