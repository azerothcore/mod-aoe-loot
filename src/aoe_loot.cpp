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
#include <algorithm>
#include <limits>

std::map<uint64, bool> AoeLootCommandScript::playerAoeLootEnabled;

void AOELootPlayer::OnPlayerLogin(Player* player)
{
    if (!player)
        return;

    if (sConfigMgr->GetOption<bool>("AOELoot.Enable", true))
    {
        if (sConfigMgr->GetOption<bool>("AOELoot.Message", true))
        {
            if (WorldSession* session = player->GetSession())
                ChatHandler(session).PSendSysMessage(AOE_ACORE_STRING_MESSAGE);
        }
    }
}

bool AOELootServer::CanPacketReceive(WorldSession* session, WorldPacket& packet)
{
    // Only handle loot packets
    if (packet.GetOpcode() != CMSG_LOOT)
        return true;

    // Basic validation checks
    if (!session)
        return true;

    Player* player = session->GetPlayer();
    if (!player)
        return true;

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
    float range = sConfigMgr->GetOption<float>("AOELoot.Range", 30.0f);

    // Limit range to reasonable values
    if (range < 5.0f)
        range = 5.0f;

    if (range > 100.0f)
        range = 100.0f;

    // Read target GUID from packet
    ObjectGuid targetGuid;
    packet >> targetGuid;

    if (!targetGuid)
        return true;

    // Get target creature
    Creature* mainCreature = player->GetMap()->GetCreature(targetGuid);
    if (!mainCreature)
        return true;

    // Check if main creature has loot
    if (!mainCreature->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE))
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
                !player->isAllowedToLoot(c);
        });

    // If no other corpses, process normally
    if (nearbyCorpses.empty())
    {
        player->SendLoot(targetGuid, LOOT_CORPSE);
        return false;
    }

    // Get main loot
    Loot* mainLoot = &mainCreature->loot;

    // Limit number of corpses to process
    size_t const maxCorpses = 10; // set to 10 to improve stability
    size_t processedCorpses = 0;

    // Track total gold to merge
    uint32 totalGold = mainLoot->gold;

    // Collect all items to merge (don't modify main loot directly)
    std::vector<LootItem> itemsToAdd;
    std::vector<LootItem> questItemsToAdd;

    for (Creature* creature : nearbyCorpses)
    {
        if (processedCorpses >= maxCorpses)
            break;

        if (!creature)
            continue;

        Loot* loot = &creature->loot;

        // Skip already looted corpses
        if (loot->isLooted())
            continue;

        // Collect gold
        if (loot->gold > 0)
        {
            // Prevent overflow
            if (totalGold < (std::numeric_limits<uint32>::max() - loot->gold))
                totalGold += loot->gold;
        }

        // Collect regular items
        for (size_t i = 0; i < loot->items.size(); ++i)
        {
            // Check if there's still space
            if ((mainLoot->items.size() + itemsToAdd.size() + mainLoot->quest_items.size() + questItemsToAdd.size()) >= 15)
                break;

            itemsToAdd.push_back(loot->items[i]);
        }

        // Collect quest items
        for (size_t i = 0; i < loot->quest_items.size(); ++i)
        {
            // Check if there's still space
            if ((mainLoot->items.size() + itemsToAdd.size() + mainLoot->quest_items.size() + questItemsToAdd.size()) >= 15)
                break;

            questItemsToAdd.push_back(loot->quest_items[i]);
        }

        // Clear source loot (but don't modify vector directly)
        loot->clear();
        creature->AllLootRemovedFromCorpse();
        creature->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);

        processedCorpses++;
    }

    // Now safely add collected items to main loot
    // Update gold
    mainLoot->gold = totalGold;

    // Add regular items
    for (const auto& item : itemsToAdd)
    {
        if (mainLoot->items.size() < 15)
            mainLoot->items.push_back(item);
    }

    // Add quest items to player inventory
    for (const auto& item : questItemsToAdd)
    {
        player->AddItem(item.itemid, item.count);
    }

    // Send merged loot window
    player->SendLoot(targetGuid, LOOT_CORPSE);

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
    //AoeLootCommandScript::DebugMessage(nullptr, fmt::format("Set AOE loot enabled for GUID {}: {}", guid, mode));
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
        handler->PSendSysMessage("AOE Loot is already enabled for your character.");
        return true;
    }

    AoeLootCommandScript::setPlayerAoeLootEnabled(playerGuid, true);
    handler->PSendSysMessage("AOE Loot enabled for your character. Type: '.aoeloot off' to turn AoE Looting off.");
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
        handler->PSendSysMessage("AOE Loot is already disabled for your character.");
        return true;
    }

    AoeLootCommandScript::setPlayerAoeLootEnabled(playerGuid, false);
    handler->PSendSysMessage("AOE Loot disabled for your character. Type: '.aoeloot on' to turn AoE Looting on.");
    return true;
}
