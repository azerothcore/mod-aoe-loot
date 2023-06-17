# mod-aoe-loot

# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

## Important notes

The module is currently not working. We are still working to finish it. We have made some progress, but it still needs to be functional. Do not use it, because it will give you failures. If you want to collaborate with it, you can open a pull request or an issue by completing the requested data.

For the module to work, 4 things are needed.
- [x] Create the CanSendErrorArleadyLooted hook. (created)
- [x] Create the hook CanSendCreatureLoot (created)
- [x] Create the hook OnBeforeCreatureLootMoney (created)
- [ ] The module works, if the player takes an object from the loot. If the player selects gold, it does not run. So we want to check that and also correct to be able to add up the total amount of gold obtained.

## Description

The objective of this module is to allow players to obtain all items from various npc's within a certain distance range. It is currently under development and is not finished yet. If you want to collaborate with it, you can do it by creating a pull request.
