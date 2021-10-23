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

#include "Item.h"
#include "Log.h"
#include "Chat.h"
#include "Config.h"
#include "Corpse.h"
#include "Creature.h"
#include "Chat.h"
#include "GameObject.h"
#include "Group.h"
#include "LootItemStorage.h"
#include "LootMgr.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"

bool AOELootEnable = 1;
bool AOELootAnnounceModule = 1;

class AoeLoot_World : public WorldScript
{
public:
    AoeLoot_World() : WorldScript("AoeLoot_World") { }

    void OnConfigLoad(bool reload)
    {
        if (!reload)
        {
            // Load Configuration Settings
            SetInitialWorldSettings();
        }
    }

    // Load Configuration Settings
    void SetInitialWorldSettings()
    {
        AOELootEnable = sConfigMgr->GetOption<bool>("AOELoot.Enable", 1);
        AOELootAnnounceModule = sConfigMgr->GetOption<bool>("AOELoot.Announce.Enable", 1);
    };
};

class AoeLootAnnounce : public PlayerScript
{
public:
    AoeLootAnnounce() : PlayerScript("AoeLootAnnounce") {}
    void OnLogin(Player* Player) override
    {
        // Announce Module
        if (AOELootEnable)
        {
            if (AOELootAnnounceModule)
            {
                ChatHandler(Player->GetSession()).SendSysMessage("This server is running the |cff4CFF00AOE Loot Azerothcore Custom |rmodule.");
            }
        }
    }
};

class AoeLoot_Player : public PlayerScript
{
public:
    AoeLoot_Player() : PlayerScript("AoeLoot_Player") { }

    void Process(Loot* loot, LootStore const& store, Creature* creature, Player* player, uint8 lootSlot) const
    {
        QuestItem* qitem = nullptr;
        QuestItem* ffaitem = nullptr;
        QuestItem* conditem = nullptr;
        LootItem* item = loot->LootItemInSlot(lootSlot, player, &qitem, &ffaitem, &conditem);

        if (!item || item->is_looted)
        {
            if (!player->GetGroup() && creature && AOELootEnable)
            {   // prevents error already loot from spamming
                // SendEquipError(EQUIP_ERR_ALREADY_LOOTED, nullptr, nullptr);
                return;
            }

            else
            {
                player->SendEquipError(EQUIP_ERR_ALREADY_LOOTED, nullptr, nullptr);
                return;
            }
        }
    }

    void Process(Creature* creature, Player* player) const
    {
        if (!player->GetGroup() && creature && AOELootEnable)
        int i = 0;
        float range = 30.0f;
        Creature* c = nullptr;
        uint32 gold = 0;
        Loot* loot = nullptr;
        std::list<Creature*> creaturedie;
        player->GetDeadCreatureListInGrid(creaturedie, range);
        for (std::list<Creature*>::iterator itr = creaturedie.begin(); itr != creaturedie.end(); ++itr)
        {
            int i = 0;
            c = *itr;
            loot = &c->loot;
            gold += loot->gold;
            loot->gold = 0;
            uint8 lootSlot = 0;
            uint8 maxSlot = loot->GetMaxSlotInLootFor(player);
            for (i = 0; i < maxSlot; ++i)
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
                        player->GetSession()->SendAreaTriggerMessage("Your items has been mailed to you.");
                    }
                }
            }

            // This if covers a issue with skinning being infinite by Aokromes
            if (!creature->IsAlive())
            {
                creature->AllLootRemovedFromCorpse();
            }

            loot->clear();

            if (loot->isLooted() && loot->empty())
            {
                c->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                c->AllLootRemovedFromCorpse();
            }
            loot->gold = gold;
            player->ModifyMoney(loot->gold);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, loot->gold);
        }
    }
};
// Group all custom scripts
void AddSC_AoeLoot()
{
    new AoeLoot_World();
    new AoeLootAnnounce();
    new AoeLoot_Player();
}
