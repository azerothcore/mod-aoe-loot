# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

## Notas importantes

El módulo no funciona actualmente. Seguimos trabajando para terminarlo. Hemos hecho algunos progresos, pero todavía tiene que ser funcional. No lo utilices, porque te dará fallos. Si quieres colaborar con él, puedes abrir un pull request o una issue completando los datos solicitados.

Para que el módulo funcione, se necesitan 4 cosas.
- [x] Crear el hook `CanSendErrorArleadyLooted`. (creado)
- [x] Crear el hook `CanSendCreatureLoot` (creado)
- [x] Crear el hook `OnBeforeCreatureLootMoney` (creado)
- [ ] El módulo funciona, si el jugador toma un objeto del botín. Si el jugador selecciona oro, no funciona. Así que queremos comprobar que funcione y también poder sumar la cantidad total de oro obtenida.

## Descripción

El objetivo de este módulo es permitir a los jugadores obtener todos los objetos de varios npc's dentro de un cierto rango de distancia. Actualmente está en desarrollo y aún no está terminado. Si quieres colaborar con él, puedes hacerlo creando un pull request.
