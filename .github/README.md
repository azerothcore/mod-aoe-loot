# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

![bloggif_65c3814c1ec76](https://github.com/azerothcore/mod-aoe-loot/assets/2810187/acd4bfc4-5cfb-4508-85ab-d8787f51c44e)

## Important notes

The module is currently under development, but is functional. It should be compatible at least with the module: mod-solo-lfg. Loot can be done in an area, both in a group and alone, and it works both when obtaining an item and when obtaining gold, so the client's self-loot option could be enabled. In the configuration file, there are some variables that can be modified, such as sending the item by mail if the bags are full, or increasing the search range for bodies. When you chop an ore, you may also obtain all the items from the bodies, with chests I have done the test and it does not work, but with ores, I understand that it does, it remains to be seen if it also supports the herbalist profession in case you have some material near the bodies.

The time that a body remains in the world, after having no items, is determined in the worldserver.conf configuration file, so when loot in area, it is perhaps necessary to increase that time a little, to give it time for people who have the skinning skill, enough time to be able to obtain the materials for said skill, without the body being erased too quickly.

```
#
#    Corpse.Decay.NORMAL
#    Corpse.Decay.RARE
#    Corpse.Decay.ELITE
#    Corpse.Decay.RAREELITE
#    Corpse.Decay.WORLDBOSS
#        Description: Time (in seconds) until creature corpse will decay if not looted or skinned.
#        Default:     60   - (1 Minute, Corpse.Decay.NORMAL)
#                     300  - (5 Minutes, Corpse.Decay.RARE)
#                     300  - (5 Minutes, Corpse.Decay.ELITE)
#                     300  - (5 Minutes, Corpse.Decay.RAREELITE)
#                     3600 - (1 Hour, Corpse.Decay.WORLDBOSS)

Corpse.Decay.NORMAL    = 60
Corpse.Decay.RARE      = 300
Corpse.Decay.ELITE     = 300
Corpse.Decay.RAREELITE = 300
Corpse.Decay.WORLDBOSS = 3600

#
#    Rate.Corpse.Decay.Looted
#        Description: Multiplier for Corpse.Decay.* to configure how long creature corpses stay
#                     after they have been looted.
#         Default:    0.5

Rate.Corpse.Decay.Looted = 0.5
```

This module requires the following pull request

https://github.com/azerothcore/azerothcore-wotlk/pull/16589

## Description

The objective of this module is to allow players to obtain all items from various npc's within a certain distance range.
