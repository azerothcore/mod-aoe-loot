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

void OnCreatureLootAOE(Player* player, ObjectGuid lootguid)
{
    bool _enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

    if (!_enable)
        return;

    if (!lootguid.IsCreature())
        return;

    float range = sConfigMgr->GetOption<float>("AOELoot.Range", 30.0);

    std::list<Creature*> deadCreatures;
    uint32 gold = 0;
    player->GetDeadCreatureListInGrid(deadCreatures, range);

    for (auto& _creature : deadCreatures)
    {
        Loot* loot = &_creature->loot;
        gold += loot->gold;
        loot->gold = 0;
        uint8 maxSlot = loot->GetMaxSlotInLootFor(player);

        for (uint32 lootSlot = 0; lootSlot < maxSlot; ++lootSlot)
        {
            InventoryResult msg;
            LootItem* lootItem = player->StoreLootItem(lootSlot, loot, msg);

            if (msg != EQUIP_ERR_OK && lootguid.IsItem() && loot->loot_type != LOOT_CORPSE)
            {
                lootItem->is_looted = true;
                loot->NotifyItemRemoved(lootItem->itemIndex);
                loot->unlootedCount--;

                player->SendItemRetrievalMail(lootItem->itemid, lootItem->count);
            }
            else
            {
                player->SendLootRelease(player->GetLootGUID());
            }
        }

        if (loot->isLooted())
        {
            player->GetSession()->DoLootRelease(lootguid);

            // skip pickpocketing loot for speed, skinning timer reduction is no-op in fact
            if (!_creature->IsAlive())
                _creature->AllLootRemovedFromCorpse();

            _creature->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
            loot->clear();
        }
    }

    if (player->GetGroup())
    {
        Group* group = player->GetGroup();

        std::vector<Player*> playersNear;
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member)
                continue;

            if (player->IsAtLootRewardDistance(member))
                playersNear.push_back(member);
        }

        uint32 goldPerPlayer = uint32((gold) / (playersNear.size()));

        for (std::vector<Player*>::const_iterator i = playersNear.begin(); i != playersNear.end(); ++i)
        {
            (*i)->ModifyMoney(goldPerPlayer);
            (*i)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, goldPerPlayer);

            WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
            data << uint32(goldPerPlayer);
            data << uint8(playersNear.size() > 1 ? 0 : 1);
            (*i)->GetSession()->SendPacket(&data);
        }
    }
    else
    {
        player->ModifyMoney(gold);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, gold);

        WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
        data << uint32(gold);
        data << uint8(1);
        player->GetSession()->SendPacket(&data);
    }
}

void AoeLootPlayer::OnLogin(Player* player)
{
    if (sConfigMgr->GetOption<bool>("AOELoot.Enable", true))
    {
        if (sConfigMgr->GetOption<bool>("AOELoot.Message", true))
            ChatHandler(player->GetSession()).PSendSysMessage(AOE_ACORE_STRING_MESSAGE);
    }
}

bool AoeLootPlayer::CanSendErrorAlreadyLooted(Player* /*player*/)
{
    return true;
}

void AoeLootPlayer::OnLootItem(Player* player, Item* /*item*/, uint32 /*count*/, ObjectGuid lootguid)
{
    OnCreatureLootAOE(player, lootguid);
}

void AoeLootPlayer::OnBeforeLootMoney(Player* player, Loot* /*loot*/)
{
    OnCreatureLootAOE(player, player->GetLootGUID());
}

/*
* This function is responsible for deleting the player's leftover items.
* But it only deletes those that are from a quest, and that cannot be obtained if the quest were not being carried out.
* Since there are some items that can be obtained even if you are not doing a quest.
*/
void AoeLootPlayer::OnPlayerCompleteQuest(Player* player, Quest const* quest)
{
    for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
    {
        if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(quest->RequiredItemId[i]))
        {
            if ((itemTemplate->Bonding == BIND_QUEST_ITEM) && (!quest->IsRepeatable()))
            {
                player->DestroyItemCount(quest->RequiredItemId[i], 9999, true);
            }
        }
    }
}
