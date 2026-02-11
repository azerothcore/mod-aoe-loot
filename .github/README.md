# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot/actions)

## Description

This module enables Area of Effect (AOE) looting functionality for AzerothCore, allowing players to loot multiple nearby corpses by interacting with just one of them. All items and gold from corpses within the configured range are automatically collected into a single loot window.

## Features

- **AOE Looting**: Automatically collect loot from multiple nearby corpses with a single interaction
- **Player Toggle Commands**: Individual players can enable/disable AOE loot using `.aoeloot on/off` commands
- **Multi-language Support**: Full internationalization support with English and Spanish translations (easily extensible to other languages)
- **Configurable Range**: Server administrators can set the maximum distance for AOE loot collection
- **Group Support**: Optional group looting with configurable settings
- **Performance Optimized**: Limits number of corpses processed to maintain server stability
- **Smart Item Management**: 
  - Automatic gold accumulation with overflow protection
  - Quest items sent directly to inventory
  - Maximum 15 items per loot window to prevent UI issues
- **Corpse Management**: Automatically cleans up looted corpses to reduce clutter

## Recent Updates

### v2.0 - Player Control & Internationalization
- ✅ Added `.aoeloot on/off` player commands for individual control
- ✅ Implemented multi-language support via `acore_string` system
- ✅ Fixed enum ID alignment issues
- ✅ Complete English and Spanish translations
- ✅ Improved code documentation and structure

### v1.x - Core Functionality
- Initial AOE loot implementation
- Configuration system
- Range and group settings

## Requirements

- AzerothCore v3.0.0+ (latest master branch recommended)
- MySQL 8.0+
- Compiler with C++17 support

## Installation

### 1. Clone the Module

Navigate to your AzerothCore modules directory:

```bash
cd <ACoreDir>/modules
git clone https://github.com/azerothcore/mod-aoe-loot.git
```

### 2. Compile

Re-compile AzerothCore:

```bash
cd <ACoreDir>/build
cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/server -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
make -j $(nproc)
make install
```

### 3. Configure

Edit your `worldserver.conf` file (or create `AOELoot.conf` in configs folder):

```conf
###################################################################################################
#    AOE LOOT MODULE CONFIGURATION
###################################################################################################

#
#    AOELoot.Enable
#        Description: Enable or disable the AOE Loot module globally
#        Default:     1 (enabled)
#                     0 (disabled)

AOELoot.Enable = 1

#
#    AOELoot.Range
#        Description: Maximum distance (in yards) to collect loot from nearby corpses
#        Default:     30.0
#        Range:       5.0 - 100.0

AOELoot.Range = 30.0

#
#    AOELoot.Group
#        Description: Allow AOE looting when player is in a group
#        Default:     1 (allowed)
#                     0 (not allowed)

AOELoot.Group = 1

#
#    AOELoot.Message
#        Description: Show informational message on player login
#        Default:     1 (show message)
#                     0 (no message)

AOELoot.Message = 1

###################################################################################################
```

### 4. Corpse Decay Configuration (IMPORTANT)

For optimal experience, modify the corpse decay settings in `worldserver.conf`:

```conf
#
#    Rate.Corpse.Decay.Looted
#        Description: Multiplier for Corpse.Decay.* to configure how long creature corpses stay
#                     after they have been looted.
#        Default:     0.5
#        Recommended: 0.01 (for AOE Loot module)

Rate.Corpse.Decay.Looted = 0.01
```

**Why this is important:** The default decay rate (0.5) can cause corpses to linger after being looted via AOE, creating visual clutter. Setting this to 0.01 ensures corpses disappear quickly after looting.

### 5. Restart Server

Restart your worldserver to load the module:

```bash
./worldserver
```

## Usage

### For Players

#### Commands
- `.aoeloot on` - Enable AOE looting for your character
- `.aoeloot off` - Disable AOE looting for your character

#### How to Use
1. Kill multiple enemies in close proximity
2. Right-click on any corpse to loot
3. All items from nearby corpses will appear in a single loot window
4. Quest items are automatically added to your inventory

**Note:** Player preferences reset on logout. AOE loot is enabled by default if the module is active.

### For Administrators

The module can be controlled through configuration file settings (see Configuration section above).

## Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `AOELoot.Enable` | Boolean | 1 | Enable/disable module globally |
| `AOELoot.Range` | Float | 30.0 | Maximum loot collection radius (5.0 - 100.0) |
| `AOELoot.Group` | Boolean | 1 | Allow AOE loot in groups |
| `AOELoot.Message` | Boolean | 1 | Show login message |

## Multi-language Support

The module includes full multi-language support through AzerothCore's `acore_string` system.

### Currently Supported Languages
- 🇬🇧 English (en_US)
- 🇪🇸 Spanish (es_ES / es_MX)

### Adding More Languages

To add additional language support, update the SQL file:

```sql
UPDATE `acore_string` SET 
    `locale_frFR` = 'Votre traduction ici',
    `locale_deDE` = 'Ihre Übersetzung hier',
    `locale_ruRU` = 'Ваш перевод здесь'
WHERE `entry` BETWEEN 50000 AND 50007;
```

Supported locale columns:
- `locale_koKR` (Korean)
- `locale_frFR` (French)
- `locale_deDE` (German)
- `locale_zhCN` (Chinese Simplified)
- `locale_zhTW` (Chinese Traditional)
- `locale_ruRU` (Russian)

## Technical Details

### Database Entries

The module uses `acore_string` entries 50000-50007:

| Entry | Constant | Purpose |
|-------|----------|---------|
| 50000 | AOE_ACORE_STRING_MESSAGE | Login message |
| 50001 | AOE_ITEM_IN_THE_MAIL | Mail notification (reserved) |
| 50002-50003 | - | Reserved for future use |
| 50004 | AOE_LOOT_ALREADY_ENABLED | "Already enabled" message |
| 50005 | AOE_LOOT_ENABLED | "Enabled" confirmation |
| 50006 | AOE_LOOT_ALREADY_DISABLED | "Already disabled" message |
| 50007 | AOE_LOOT_DISABLED | "Disabled" confirmation |

### Performance Considerations

- Maximum 10 corpses processed per loot operation (hardcoded for stability)
- Maximum 15 items per loot window
- Gold overflow protection (prevents exceeding uint32 max value)
- Efficient corpse filtering and cleanup

## Troubleshooting

### Issue: AOE loot not working

**Solutions:**
- Verify module is enabled: `AOELoot.Enable = 1`
- Check if you disabled it personally: use `.aoeloot on`
- Ensure you're within range (default 30 yards)
- If in group, check `AOELoot.Group` setting

### Issue: Corpses not disappearing

**Solution:**
- Set `Rate.Corpse.Decay.Looted = 0.01` in worldserver.conf

### Issue: Messages in wrong language

**Solution:**
- Verify SQL was imported correctly
- Check client locale settings
- Confirm `acore_string` table has translations for your locale

### Issue: "Already enabled/disabled" messages appearing incorrectly

**Solution:**
- This is expected behavior - preferences reset on logout
- On first login, AOE loot is enabled by default

## Known Limitations

- Player preferences do not persist across logout/login sessions
- Maximum 10 corpses processed at once (performance limit)
- Quest items sent to inventory may fill bags quickly
- Range limited to 100 yards maximum

## Future Enhancements

Potential features for future versions:
- [ ] Database persistence for player preferences
- [ ] Configurable max corpses limit
- [ ] Quest item mail option (instead of direct inventory)
- [ ] Visual range indicator
- [ ] Per-character settings UI
- [ ] Statistics tracking (total items/gold looted)

## Credits

- **acidmanifesto** - [Original author and concept](https://github.com/azerothcore/mod-aoe-loot/pull/2)
- **AzerothCore Community** - Hooks, updates, and improvements
- **Contributors** - Player commands, multi-language support, and bug fixes

## Links

- **AzerothCore:** [Repository](https://github.com/azerothcore) | [Website](https://azerothcore.org/) | [Discord](https://discord.gg/PaqQRkd)
- **Module Repository:** [GitHub](https://github.com/azerothcore/mod-aoe-loot)
- **Issues & Suggestions:** [Issue Tracker](https://github.com/azerothcore/mod-aoe-loot/issues)

## License

This module is released under the [GNU AGPL v3 License](https://github.com/azerothcore/mod-aoe-loot/blob/master/LICENSE).

---

### Support

If you encounter any issues or have suggestions:
1. Check the [Troubleshooting](#troubleshooting) section
2. Search [existing issues](https://github.com/azerothcore/mod-aoe-loot/issues)
3. Join the [AzerothCore Discord](https://discord.gg/PaqQRkd)
4. Create a [new issue](https://github.com/azerothcore/mod-aoe-loot/issues/new) with detailed information

**Please include:**
- AzerothCore commit hash
- Operating system and version
- Complete error messages (if any)
- Configuration settings
- Steps to reproduce the issue
