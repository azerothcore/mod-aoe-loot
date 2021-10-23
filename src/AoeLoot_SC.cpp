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

#include "AoeLoot.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptedGossip.h"

class AoeLoot_World : public WorldScript
{
public:
    AoeLoot_World() : WorldScript("AoeLoot_World") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        // Add conigs options configiration
    }
};

class AoeLoot_World : public WorldScript
{
public:
    AoeLoot_World() : WorldScript("AoeLoot_World") { }

    void OnAfterConfigLoad(bool reload) override
    {
        // Load settings
        sAoeLoot->Init(reload);
    }

    void OnStartup() override
    {
        // Load settings
        sAoeLoot->Init(false);
    }
};

class AoeLoot_Player : public PlayerScript
{
public:
    AoeLoot_Player() : PlayerScript("AoeLoot_Player") { }

    /*void Process(Loot* loot, LootStore const& store, Creature* creature, Player* player, uint8 lootSlot) const
    {
        
    }

    void Process(Creature* creature, Player* player) const
    {

    }*/
};

// Group all custom scripts
void AddSC_AoeLoot()
{
    new AoeLoot_World();
    new AoeLoot_Player();
}
