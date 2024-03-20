# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

# Important note

- The mod allows you to obtain all the items from nearby bodies, by looting only one of them.
- It is important to make a modification in the emulator, regarding the delay of the bodies.

```
#
#    Rate.Corpse.Decay.Looted
#        Description: Multiplier for Corpse.Decay.* to configure how long creature corpses stay
#                     after they have been looted.
#         Default:    0.5

Rate.Corpse.Decay.Looted = 0.5
```

Consider changing that value, to `0.01` or lower. Perform tests to avoid problems.

## Credits

- acidmanifesto [Author of the idea and part of the initial code.](https://github.com/azerothcore/mod-aoe-loot/pull/2)
- Hooks and updates from the AzerothCore community
- AzerothCore: [repository](https://github.com/azerothcore) - [website](https://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
