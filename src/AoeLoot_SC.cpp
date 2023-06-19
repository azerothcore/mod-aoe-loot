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

enum AoeLootString
{
    AOE_ACORE_STRING_MESSAGE = 50000,
    AOE_ITEM_IN_THE_MAIL
};

class AoeLoot_Player : public PlayerScript
{
public:
    AoeLoot_Player() : PlayerScript("AoeLoot_Player") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("AOELoot.Enable", true))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(AOE_ACORE_STRING_MESSAGE);
        }
    }

    bool CanSendErrorAlreadyLooted(Player* /*player*/) override
    {
        return true;
    }

    void OnCreatureLootAoe(Player* player)
    {
        float range = 30.0f;
        uint32 gold = 0;

        std::list<Creature*> creaturedie;
        player->GetDeadCreatureListInGrid(creaturedie, range);

        for (auto const& _creature : creaturedie)
        {
            Loot* loot = &_creature->loot;
            gold += loot->gold;
            loot->gold = 0;
            uint8 lootSlot = 0;
            uint32 maxSlot = loot->GetMaxSlotInLootFor(player);

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
                        ChatHandler(player->GetSession()).SendSysMessage(AOE_ITEM_IN_THE_MAIL);
                    }
                }
            }

            if (!loot->empty())
            {
                if (!_creature->IsAlive())
                {
                    if (_creature->HasUnitFlag(UNIT_FLAG_SKINNABLE))
                    {
                        _creature->AllLootRemovedFromCorpse();
                        _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        _creature->RemoveUnitFlag(UNIT_FLAG_SKINNABLE);
                        loot->clear();
                    }
                    else
                    {
                        _creature->AllLootRemovedFromCorpse();
                        _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        loot->clear();
                    }
                }
            }
            else
            {
                _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                _creature->AllLootRemovedFromCorpse();
            }
        }

        player->ModifyMoney(gold);
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, gold);
        WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
        data << uint32(gold);
        data << uint8(1);
        player->GetSession()->SendPacket(&data);
    }

    bool CanSendCreatureLoot(Player* player) override
    {
        bool _Enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

        if (player->GetGroup() || !_Enable)
            return true;

        OnCreatureLootAoe(player);
        return true;
    }

    void OnBeforeCreatureLootMoney(Player* player) override
    {
        bool _Enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

        if (player->GetGroup() || !_Enable)
            return;

        OnCreatureLootAoe(player);
    }
};

void AddSC_AoeLoot()
{
    new AoeLoot_Player();
}
