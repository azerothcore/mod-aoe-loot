# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Último estado de compilación con azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

# Nota importante

El modulo, se encuentra actualmente en desarrollo. Si bien, creemos que funciona en casi todos los ámbitos, dado que hemos realizado pruebas y también recibido reportes, de personas que jugaban en grupo, personas en solitario, personas dentro de una mazmorra (ya sea que ingresaron caminando o utilizaron el modulo de `mod-solo-lfg`), no descartamos que pueda llegar a surgir algún error, por lo que es importante, que nos ayudes a probar el contenido, y creando un reporte dentro de los issue del repositorio, en caso de que encuentres algún posible fallo.

Para funcionar, el modulo requiere de versiones actualizadas del emulador, dado que se utilizan algunos hooks que existen desde hace tiempo, pero también se tuvo que crear unos hooks para el mismo. Por lo que corrobore su versión del emulador, con el comando `.server info` y asegúrese de utilizar la versión mas actualizada del mismo. Dentro de la carpeta `data/sql` se encuentra un simple script que añade traducciones del mensaje de bienvenida. Puede añadir los idiomas restantes, y el script, debería de ejecutarse automáticamente, si tiene habilitadas las opciones por defecto dentro del `worldserver.conf`, caso contrario, puede que tenga que ejecutarlas de forma manual.

# ¿Como se utiliza?

Clona el repositorio dentro de la carpeta modules del emulador, o descarga el zip y descomprimelo. Ten en cuenta, que si utilizas el .zip, es probable, que te añada al final de la carpeta descomprimida, un `-` y a continuación, el nombre de la rama que hayas descargado. Te recomendamos, clonar el repositorio, para poder, en caso de existir, descargar actualizaciones fácilmente sin la necesidad de tener que descargar todo el contenido nuevamente.

Gracias a todas las personas, que de alguna forma u otra participaron y participan activamente de este modulo, para que el mismo, pueda continuar actualizado y funcionando. Si eres colaborador o te interesa serlo, no dudes en abrir un pull request, con el contenido que te gustaría que tuviera el modulo o para arreglar algún posible fallo.
