# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## # mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

## Notas importantes

Puede que todavia, haya que hacerle alguna mejora al modulo, pero se podria decir que esta bastante bien. Sin embargo, aun se va a seguir trabajando y tratando de revisar lo que haga falta, en la medida, en la que se realicen pruebas y se pueda determinar, que los reportes son validos.

Para que el módulo funcione, se necesitan 4 cosas.
- [x] Crear el hook `CanSendErrorAlreadyLooted`. (creado)
- [x] Crear el hook `CanSendCreatureLoot` (creado)
- [x] Crear el hook `OnBeforeCreatureLootMoney` (creado)
- [x] El módulo funciona, si el jugador toma un objeto del botín. Si el jugador selecciona oro, no funciona. Así que queremos comprobar que funcione y también poder sumar la cantidad total de oro obtenida.
- [x] Si la criatura, puede ser desollada, el cuerpo, permanece por un tiempo, y cuando el mismo es desollado, automáticamente desaparece, para evitar ser desollado nuevamente.

Este módulo requiere el siguiente pull request

https://github.com/azerothcore/azerothcore-wotlk/pull/16589

## Descripción

El objetivo de este módulo es permitir a los jugadores obtener todos los objetos de varios npc's dentro de un cierto rango de distancia.
