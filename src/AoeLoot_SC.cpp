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

#include "Log.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptedGossip.h"

class AoeLoot_Player : public PlayerScript
{
public:
    AoeLoot_Player() : PlayerScript("AoeLoot_Player") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("AOELoot.Enable", false))
        {
            ChatHandler(player->GetSession()).PSendSysMessage("This server is running the |cff4CFF00Loot aoe |r module.");
        }
    }

    bool CanSendErrorArleadyLooted(Player* /*player*/) override
    {
        return true;
    }

    bool CanSendCreatureLoot(Creature* creature, Player* player) override
    {
        bool _Enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

        if (player->GetGroup() || !creature || !_Enable)
            return true;

        float range = 30.0f;
        uint32 gold = 0;

        std::list<Creature*> creaturedie;
        player->GetDeadCreatureListInGrid(creaturedie, range);

        for (auto const& _creature : creaturedie)
        {
            auto loot = &_creature->loot;
            gold += loot->gold;
            loot->gold = 0;

            uint8 lootSlot = 0;
            uint8 maxSlot = loot->GetMaxSlotInLootFor(player);

            for (uint32 i = 0; i < maxSlot; ++i)
            {
                if (LootItem* item = loot->LootItemInSlot(i, player))
                {
                    if (player->AddItem(item->itemid, item->count))
                    {
                        player->SendNotifyLootItemRemoved(lootSlot);
                        player->SendLootRelease(player->GetLootGUID());
                    }
                    else
                    {
                        player->SendItemRetrievalMail(item->itemid, item->count);
                        ChatHandler(player->GetSession()).SendSysMessage("Your items has been mailed to you.");
                    }
                }
            }

            loot->clear();

            if (loot->isLooted() && loot->empty())
            {
                _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }

            loot->gold = gold;
            player->ModifyMoney(loot->gold);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, loot->gold);
        }
        return true;
    }

    void OnBeforeCreatureLootMoney(Player* player) override
    {
        bool _Enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

        Creature* creature = nullptr;

        if (player->GetGroup() || !creature || !_Enable)
            return;

        float range = 30.0f;
        uint32 gold = 0;
        Loot* loot = nullptr;
        std::list<Creature*> creaturedie;
        player->GetDeadCreatureListInGrid(creaturedie, range);

        for (auto const& _creature : creaturedie)
        {
            loot = &_creature->loot;
            gold += loot->gold;
            loot->gold = 0;
        }

        loot->gold = gold;
    }
};

void AddSC_AoeLoot()
{
    new AoeLoot_Player();
}
