# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

![bloggif_65c3814c1ec76](https://github.com/azerothcore/mod-aoe-loot/assets/2810187/acd4bfc4-5cfb-4508-85ab-d8787f51c44e)

## Notas importantes

El modulo, actualmente se encuentra en desarrollo, pero es funcional. Deberia de ser compatible al menos, con el modulo: mod-solo-lfg. Se puede realizar el loot en area, tanto en grupo, como en solitario, y funciona tanto al obtener un item, como al obtener el oro, por lo que la opcion de autodestoje del cliente, podria estar habilitada. En el fichero de configuracion, existen algunas variables, que pueden ser modificadas, como el hecho de enviar el item por correo en caso de tener las bolsas llenas, o aumentar el rango de busqueda de cuerpos. Al picar una mena, puede que tambien obtengas todos los items de los cuerpos, con cofres he realizado la prueba y no funciona, pero con menas, tengo entendido que si, faltaria ver si tambien admite la profesion de herboristeria en caso de tener algun material cerca de los cuerpos.

El tiempo, que un cuerpo permanece en el mundo, luego de no tener items, se determina en el fichero de configuracion del worldserver.conf, por lo que al hacer loot en area, es necesario quizas, aumentar un poco ese tiempo, para darle tiempo a las personas que tenga la habilidad de desuello, el tiempo suficiente, para poder obtener los materiales de dicha habilidad, sin que el cuerpo se borre demasiado rapido.

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

Este módulo requiere el siguiente pull request

https://github.com/azerothcore/azerothcore-wotlk/pull/16589

## Descripción

El objetivo de este módulo es permitir a los jugadores obtener todos los objetos de varios npc's dentro de un cierto rango de distancia.
