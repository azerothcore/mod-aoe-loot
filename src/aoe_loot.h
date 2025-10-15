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

#ifndef MODULE_AOELOOT_H
#define MODULE_AOELOOT_H

#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "Group.h"
#include "LootMgr.h"
#include "Creature.h"
#include "Log.h"
#include <vector>
#include <list>
#include <algorithm>

 // Maximum loot items count
constexpr size_t MAX_LOOT_ITEMS = 16;

enum AoeLootString
{
    AOE_ACORE_STRING_MESSAGE = 50000,
    AOE_ITEM_IN_THE_MAIL
};

class AOELootPlayer : public PlayerScript
{
public:
    AOELootPlayer() : PlayerScript("AOELootPlayer") {}

    void OnPlayerLogin(Player* player) override;
};

class AOELootServer : public ServerScript
{
public:
    AOELootServer() : ServerScript("AOELootServer") {}

    bool CanPacketReceive(WorldSession* session, WorldPacket& packet) override;

private:
    // Helper function - Check if loot is valid
    bool IsLootValid(Loot* loot) const;

    // Check if loot can be merged
    bool CanMergeLoot(Player* player, Creature* creature) const;

    // Safely get item count
    size_t GetSafeItemCount(Loot* loot) const;

    // Safely merge loot items
    bool SafeMergeLootItems(Loot* mainLoot, Loot* sourceLoot, size_t& remainingSlots);
};

// Configuration options structure (optional, for better config management)
struct AOELootConfig
{
    bool enabled = true;
    bool messageOnLogin = true;
    bool allowInGroup = true;
    float range = 55.0f;
    uint32 maxCorpses = 20;

    static AOELootConfig* instance()
    {
        static AOELootConfig instance;
        return &instance;
    }

    void Load()
    {
        enabled = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);
        messageOnLogin = sConfigMgr->GetOption<bool>("AOELoot.Message", true);
        allowInGroup = sConfigMgr->GetOption<bool>("AOELoot.Group", true);
        range = sConfigMgr->GetOption<float>("AOELoot.Range", 55.0f);
        maxCorpses = sConfigMgr->GetOption<uint32>("AOELoot.MaxCorpses", 20);

        // Validate configuration values
        if (range < 5.0f) range = 5.0f;
        if (range > 100.0f) range = 100.0f;
        if (maxCorpses < 1) maxCorpses = 1;
        if (maxCorpses > 50) maxCorpses = 50;
    }
};

void AddSC_AoeLoot()
{
    new AOELootPlayer();
    new AOELootServer();
}

#endif //MODULE_AOELOOT_H
