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

    void OnCreatureLootAOE(Player* player)
    {
        bool _enable = sConfigMgr->GetOption<bool>("AOELoot.Enable", true);

        if (!_enable)
            return;

        float range = 30.0f;
        uint32 gold = 0;

        std::list<Creature*> creaturedie;
        player->GetDeadCreatureListInGrid(creaturedie, range);

        for (auto const& _creature : creaturedie)
        {
            if (player->GetGroup())
            {
                if (player->GetGroup()->GetMembersCount() > 1)
                {
                    if (_creature->IsDungeonBoss() || _creature->isWorldBoss())
                        continue;
                }
                else if (player->GetGroup()->GetMembersCount() == 1)
                {
                    player->GetGroup()->SetLootMethod(FREE_FOR_ALL);
                }
            }

            if (player == _creature->GetLootRecipient())
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
                        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(item->itemid);

                        if (itemTemplate->MaxCount != 1)
                        {
                            player->SendNotifyLootItemRemoved(lootSlot);
                            player->SendLootRelease(player->GetLootGUID());

                            if (player->AddItem(item->itemid, item->count))
                            {
                                player->SendNotifyLootItemRemoved(lootSlot);
                                player->SendLootRelease(player->GetLootGUID());
                            }
                            else if (sConfigMgr->GetOption<bool>("AOELoot.MailEnable", true))
                            {
                                player->SendItemRetrievalMail(item->itemid, item->count);
                                ChatHandler(player->GetSession()).SendSysMessage(AOE_ITEM_IN_THE_MAIL);
                            }
                        }
                        else
                        {
                            if (!player->HasItemCount(item->itemid, 1))
                            {
                                player->AddItem(item->itemid, item->count);
                            }
                            player->SendNotifyLootItemRemoved(lootSlot);
                            player->SendLootRelease(player->GetLootGUID());
                        }
                    }
                }

                if (!loot->empty())
                {
                    if (!_creature->IsAlive())
                    {
                        _creature->AllLootRemovedFromCorpse();
                        _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        loot->clear();

                        if (_creature->HasUnitFlag(UNIT_FLAG_SKINNABLE))
                        {
                            _creature->RemoveUnitFlag(UNIT_FLAG_SKINNABLE);
                        }
                    }
                }
                else
                {
                    _creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                    _creature->AllLootRemovedFromCorpse();
                }
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

    void OnAfterCreatureLoot(Player* player) override
    {
        OnCreatureLootAOE(player);
    }

    void OnAfterCreatureLootMoney(Player* player) override
    {
        OnCreatureLootAOE(player);
    }
};

void AddSC_AoeLoot()
{
    new AoeLoot_Player();
}
