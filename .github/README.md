# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

## Important notes

There may still be some improvements to be made to the module, but you could say that it is pretty good. However, we are still going to continue working and trying to revise what is needed, as long as tests are performed and it can be determined that the reports are valid.

For the module to work, 4 things are needed.
- [x] Create the `CanSendErrorAlreadyLooted` hook. (created)
- [x] Create the hook `CanSendCreatureLoot` (created)
- [x] Create the hook `OnBeforeCreatureLootMoney` (created)
- [x] The module works, if the player takes an object from the loot. If the player selects gold, it does not run. So we want to check that and also correct to be able to add up the total amount of gold obtained.
- [x] If the creature can be skinned, the body remains for a while, and when it is skinned, it automatically disappears to avoid being skinned again.

This module requires the following pull request

https://github.com/azerothcore/azerothcore-wotlk/pull/16589

## Description

The objective of this module is to allow players to obtain all items from various npc's within a certain distance range.
