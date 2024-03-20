# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg)](https://github.com/azerothcore/mod-aoe-loot)

# Nota importante

- El mod te permite obtener todos los objetos de los cuerpos cercanos, saqueando solo uno de ellos.
- Es importante realizar una modificación en el emulador, en cuanto al delay de los cuerpos.

```
#
#    Rate.Corpse.Decay.Looted
#        Description: Multiplier for Corpse.Decay.* to configure how long creature corpses stay
#                     after they have been looted.
#         Default:    0.5

Rate.Corpse.Decay.Looted = 0.5
```

Considere cambiar ese valor a `0.01` o menos. Realizar pruebas para evitar problemas.

## Créditos

- acidmanifesto [Autor de la idea y parte del código inicial.](https://github.com/azerothcore/mod-aoe-loot/pull/2)
- Hooks y actualizaciones por parte de la comunidad de AzerothCore
- AzerothCore: [repositorio](https://github.com/azerothcore) - [web](https://azerothcore.org/) - [discord](https://discord.gg/PaqQRkd)
