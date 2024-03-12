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

enum AoeLootString
{
    AOE_ACORE_STRING_MESSAGE = 50000,
    AOE_ITEM_IN_THE_MAIL
};

void OnCreatureLootAOE(Player* player);

class AoeLootPlayer : public PlayerScript
{
public:
    AoeLootPlayer() : PlayerScript("AoeLootPlayer") { }

    void OnLogin(Player* player) override;
    bool CanSendErrorAlreadyLooted(Player* /*player*/) override;
    void OnAfterCreatureLoot(Player* player) override;
    void OnBeforeLootMoney(Player* player, Loot* /*loot*/) override;
    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override;
};

void AddSC_AoeLoot()
{
    new AoeLootPlayer();
}

#endif //MODULE_AOELOOT_H
