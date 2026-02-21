SET @MODULE_STRING := 'mod-aoe-loot';

-- module string
DELETE FROM `module_string` WHERE `module` = @MODULE_STRING;
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
(@MODULE_STRING, 1, 'This server is running the |cff4CFF00Loot aoe|r module.'),
(@MODULE_STRING, 2, '|cff4CFF00[Loot aoe]|r Your items has been mailed to you.'),
(@MODULE_STRING, 3, 'AOE Loot module is active. Use .aoeloot on/off to toggle it.'),
(@MODULE_STRING, 4, 'AOE Loot: Quest item sent to your mailbox.'),
(@MODULE_STRING, 5, 'AOE Loot is already enabled for your character.'),
(@MODULE_STRING, 6, 'AOE Loot enabled for your character. Type .aoeloot off to disable it.'),
(@MODULE_STRING, 7, 'AOE Loot is already disabled for your character.'),
(@MODULE_STRING, 8, 'AOE Loot disabled for your character. Type .aoeloot on to enable it.');

-- localizations
DELETE FROM `module_string_locale` WHERE `module` = @MODULE_STRING;
INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
(@MODULE_STRING, 1, 'esES', 'Este servidor está ejecutando el módulo |cff4CFF00Loot aoe|r.'),
(@MODULE_STRING, 2, 'esES', '|cff4CFF00[Loot aoe]|r  Sus artículos le han sido enviados por correo.'),
(@MODULE_STRING, 3, 'esES', 'El módulo de Botín AOE está activo. Usa .aoeloot on/off para activarlo o desactivarlo.'),
(@MODULE_STRING, 4, 'esES', 'Botín AOE: Objeto de misión enviado a tu buzón.'),
(@MODULE_STRING, 5, 'esES', 'El Botín AOE ya está activado para tu personaje.'),
(@MODULE_STRING, 6, 'esES', 'Botín AOE activado para tu personaje. Escribe .aoeloot off para desactivarlo.'),
(@MODULE_STRING, 7, 'esES', 'El Botín AOE ya está desactivado para tu personaje.'),
(@MODULE_STRING, 8, 'esES', 'Botín AOE desactivado para tu personaje. Escribe .aoeloot on para activarlo.'),
(@MODULE_STRING, 1, 'esMX', 'Este servidor está ejecutando el módulo |cff4CFF00Loot aoe|r.'),
(@MODULE_STRING, 2, 'esMX', '|cff4CFF00[Loot aoe]|r  Sus artículos le han sido enviados por correo.'),
(@MODULE_STRING, 3, 'esMX', 'El módulo de Botín AOE está activo. Usa .aoeloot on/off para activarlo o desactivarlo.'),
(@MODULE_STRING, 4, 'esMX', 'Botín AOE: Objeto de misión enviado a tu buzón.'),
(@MODULE_STRING, 5, 'esMX', 'El Botín AOE ya está activado para tu personaje.'),
(@MODULE_STRING, 6, 'esMX', 'Botín AOE activado para tu personaje. Escribe .aoeloot off para desactivarlo.'),
(@MODULE_STRING, 7, 'esMX', 'El Botín AOE ya está desactivado para tu personaje.'),
(@MODULE_STRING, 8, 'esMX', 'Botín AOE desactivado para tu personaje. Escribe .aoeloot on para activarlo.');
