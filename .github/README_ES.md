# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot/actions)

## Descripción

Este módulo habilita la funcionalidad de saqueo en área (AOE) para AzerothCore, permitiendo a los jugadores saquear múltiples cadáveres cercanos interactuando con solo uno de ellos. Todos los objetos y oro de los cadáveres dentro del rango configurado se recopilan automáticamente en una sola ventana de botín.

## Características

- **Saqueo AOE**: Recolecta automáticamente el botín de múltiples cadáveres cercanos con una sola interacción
- **Comandos de Activación Individual**: Los jugadores pueden activar/desactivar el saqueo AOE usando los comandos `.aoeloot on/off`
- **Soporte Multi-idioma**: Internacionalización completa con traducciones en inglés y español (fácilmente extensible a otros idiomas)
- **Rango Configurable**: Los administradores del servidor pueden establecer la distancia máxima para la recolección de botín AOE
- **Soporte de Grupo**: Saqueo en grupo opcional con configuración personalizable
- **Optimizado para Rendimiento**: Limita el número de cadáveres procesados para mantener la estabilidad del servidor
- **Gestión Inteligente de Objetos**:
  - Acumulación automática de oro con protección contra desbordamiento
  - Objetos de misión enviados directamente al inventario
  - Máximo de 15 objetos por ventana de botín para evitar problemas de interfaz
- **Gestión de Cadáveres**: Limpia automáticamente los cadáveres saqueados para reducir el desorden visual

## Actualizaciones Recientes

### v2.0 - Control del Jugador e Internacionalización
- ✅ Agregados comandos `.aoeloot on/off` para control individual del jugador
- ✅ Implementado soporte multi-idioma mediante el sistema `acore_string`
- ✅ Corregidos problemas de alineación de IDs en enums
- ✅ Traducciones completas en inglés y español
- ✅ Mejorada la documentación y estructura del código

### v1.x - Funcionalidad Principal
- Implementación inicial del saqueo AOE
- Sistema de configuración
- Ajustes de rango y grupo

## Requisitos

- AzerothCore v3.0.0+ (se recomienda la última rama master)
- MySQL 8.0+
- Compilador con soporte para C++17

## Instalación

### 1. Clonar el Módulo

Navega al directorio de módulos de AzerothCore:

```bash
cd <DirectorioACore>/modules
git clone https://github.com/azerothcore/mod-aoe-loot.git
```

### 2. Compilar

Recompila AzerothCore:

```bash
cd <DirectorioACore>/build
cmake ../ -DCMAKE_INSTALL_PREFIX=/ruta/al/servidor -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++
make -j $(nproc)
make install
```

### 3. Configurar

Edita tu archivo `worldserver.conf` (o crea `AOELoot.conf` en la carpeta de configuraciones):

```conf
###################################################################################################
#    CONFIGURACIÓN DEL MÓDULO AOE LOOT
###################################################################################################

#
#    AOELoot.Enable
#        Descripción: Habilita o deshabilita el módulo AOE Loot globalmente
#        Por defecto:  1 (habilitado)
#                      0 (deshabilitado)

AOELoot.Enable = 1

#
#    AOELoot.Range
#        Descripción: Distancia máxima (en yardas) para recolectar botín de cadáveres cercanos
#        Por defecto:  55.0
#        Rango:        5.0 - 100.0

AOELoot.Range = 55.0

#
#    AOELoot.Group
#        Descripción: Permitir saqueo AOE cuando el jugador está en un grupo
#        Por defecto:  1 (permitido)
#                      0 (no permitido)

AOELoot.Group = 1

#
#    AOELoot.Message
#        Descripción: Mostrar mensaje informativo al iniciar sesión
#        Por defecto:  1 (mostrar mensaje)
#                      0 (sin mensaje)

AOELoot.Message = 1

###################################################################################################
```

### 4. Configuración de Degradación de Cadáveres (IMPORTANTE)

Para una experiencia óptima, modifica la configuración de degradación de cadáveres en `worldserver.conf`:

```conf
#
#    Rate.Corpse.Decay.Looted
#        Descripción: Multiplicador para Corpse.Decay.* que configura cuánto tiempo permanecen
#                     los cadáveres de las criaturas después de ser saqueados.
#        Por defecto:  0.5
#        Recomendado:  0.01 (para el módulo AOE Loot)

Rate.Corpse.Decay.Looted = 0.01
```

**Por qué es importante:** La tasa de degradación predeterminada (0.5) puede hacer que los cadáveres permanezcan después de ser saqueados mediante AOE, creando desorden visual. Establecer esto en 0.01 asegura que los cadáveres desaparezcan rápidamente después del saqueo.

### 5. Reiniciar el Servidor

Reinicia tu worldserver para cargar el módulo:

```bash
./worldserver
```

## Uso

### Para Jugadores

#### Comandos
- `.aoeloot on` - Activar el saqueo AOE para tu personaje
- `.aoeloot off` - Desactivar el saqueo AOE para tu personaje

#### Cómo Usar
1. Mata múltiples enemigos en proximidad cercana
2. Haz clic derecho en cualquier cadáver para saquear
3. Todos los objetos de los cadáveres cercanos aparecerán en una sola ventana de botín
4. Los objetos de misión se agregan automáticamente a tu inventario

**Nota:** Las preferencias del jugador se restablecen al cerrar sesión. El saqueo AOE está habilitado por defecto si el módulo está activo.

### Para Administradores

El módulo puede controlarse a través de la configuración del archivo (ver sección de Configuración arriba).

## Opciones de Configuración

| Opción | Tipo | Por Defecto | Descripción |
|--------|------|-------------|-------------|
| `AOELoot.Enable` | Booleano | 1 | Habilitar/deshabilitar módulo globalmente |
| `AOELoot.Range` | Decimal | 55.0 | Radio máximo de recolección de botín (5.0 - 100.0) |
| `AOELoot.Group` | Booleano | 1 | Permitir saqueo AOE en grupos |
| `AOELoot.Message` | Booleano | 1 | Mostrar mensaje de inicio de sesión |

## Soporte Multi-idioma

El módulo incluye soporte completo multi-idioma a través del sistema `acore_string` de AzerothCore.

### Idiomas Actualmente Soportados
- 🇬🇧 Inglés (en_US)
- 🇪🇸 Español (es_ES / es_MX)

### Agregar Más Idiomas

Para agregar soporte de idiomas adicionales, actualiza el archivo SQL:

```sql
UPDATE `acore_string` SET
    `locale_frFR` = 'Votre traduction ici',
    `locale_deDE` = 'Ihre Übersetzung hier',
    `locale_ruRU` = 'Ваш перевод здесь'
WHERE `entry` BETWEEN 50000 AND 50007;
```

Columnas de localización soportadas:
- `locale_koKR` (Coreano)
- `locale_frFR` (Francés)
- `locale_deDE` (Alemán)
- `locale_zhCN` (Chino Simplificado)
- `locale_zhTW` (Chino Tradicional)
- `locale_ruRU` (Ruso)

## Detalles Técnicos

### Entradas de Base de Datos

El módulo utiliza las entradas `acore_string` 50000-50007:

| Entrada | Constante | Propósito |
|---------|-----------|-----------|
| 50000 | AOE_ACORE_STRING_MESSAGE | Mensaje de inicio de sesión |
| 50001 | AOE_ITEM_IN_THE_MAIL | Notificación de correo (reservado) |
| 50002-50003 | - | Reservado para uso futuro |
| 50004 | AOE_LOOT_ALREADY_ENABLED | Mensaje "Ya activado" |
| 50005 | AOE_LOOT_ENABLED | Confirmación "Activado" |
| 50006 | AOE_LOOT_ALREADY_DISABLED | Mensaje "Ya desactivado" |
| 50007 | AOE_LOOT_DISABLED | Confirmación "Desactivado" |

### Consideraciones de Rendimiento

- Máximo 10 cadáveres procesados por operación de saqueo (fijo para estabilidad)
- Máximo 15 objetos por ventana de botín
- Protección contra desbordamiento de oro (previene exceder el valor máximo uint32)
- Filtrado y limpieza eficiente de cadáveres

## Solución de Problemas

### Problema: El saqueo AOE no funciona

**Soluciones:**
- Verifica que el módulo esté habilitado: `AOELoot.Enable = 1`
- Comprueba si lo desactivaste personalmente: usa `.aoeloot on`
- Asegúrate de estar dentro del rango (55 yardas por defecto)
- Si estás en grupo, verifica la configuración `AOELoot.Group`

### Problema: Los cadáveres no desaparecen
**Solución:**
- Establece `Rate.Corpse.Decay.Looted = 0.01` en worldserver.conf

### Problema: Mensajes en idioma incorrecto

**Solución:**
- Verifica que el SQL se importó correctamente
- Comprueba la configuración de localización del cliente
- Confirma que la tabla `acore_string` tiene traducciones para tu localización

### Problema: Mensajes "Ya activado/desactivado" aparecen incorrectamente

**Solución:**
- Este es el comportamiento esperado - las preferencias se restablecen al cerrar sesión
- En el primer inicio de sesión, el saqueo AOE está habilitado por defecto

## Limitaciones Conocidas

- Las preferencias del jugador no persisten entre sesiones de inicio/cierre de sesión
- Máximo 10 cadáveres procesados a la vez (límite de rendimiento)
- Los objetos de misión enviados al inventario pueden llenar las bolsas rápidamente
- Rango limitado a un máximo de 100 yardas

## Mejoras Futuras

Características potenciales para versiones futuras:
- [ ] Persistencia en base de datos para preferencias del jugador
- [ ] Límite configurable de cadáveres máximos
- [ ] Opción de envío de objetos de misión por correo (en lugar de inventario directo)
- [ ] Indicador visual de rango
- [ ] Interfaz de configuración por personaje
- [ ] Seguimiento de estadísticas (objetos/oro total saqueado)

## Créditos

- **acidmanifesto** - [Autor original y concepto](https://github.com/azerothcore/mod-aoe-loot/pull/2)
- **Comunidad AzerothCore** - Hooks, actualizaciones y mejoras
- **Colaboradores** - Comandos de jugador, soporte multi-idioma y correcciones de errores

## Enlaces

- **AzerothCore:** [Repositorio](https://github.com/azerothcore) | [Sitio Web](https://azerothcore.org/) | [Discord](https://discord.gg/PaqQRkd)
- **Repositorio del Módulo:** [GitHub](https://github.com/azerothcore/mod-aoe-loot)
- **Problemas y Sugerencias:** [Rastreador de Problemas](https://github.com/azerothcore/mod-aoe-loot/issues)

## Licencia

Este módulo se publica bajo la [Licencia GNU AGPL v3](https://github.com/azerothcore/mod-aoe-loot/blob/master/LICENSE).

---

### Soporte

Si encuentras algún problema o tienes sugerencias:
1. Consulta la sección [Solución de Problemas](#solución-de-problemas)
2. Busca en [problemas existentes](https://github.com/azerothcore/mod-aoe-loot/issues)
3. Únete al [Discord de AzerothCore](https://discord.gg/PaqQRkd)
4. Crea un [nuevo problema](https://github.com/azerothcore/mod-aoe-loot/issues/new) con información detallada

**Por favor incluye:**
- Hash del commit de AzerothCore
- Sistema operativo y versión
- Mensajes de error completos (si los hay)
- Configuraciones utilizadas
- Pasos para reproducir el problema
