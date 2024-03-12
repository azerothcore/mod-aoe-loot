# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# mod-aoe-loot

[English](README.md) | [Español](README_ES.md)

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-aoe-loot/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-aoe-loot)

# Important note

The module is currently under development. Although, we believe that it works in almost all areas, since we have carried out tests and also received reports, from people who played in a group, people alone, people inside a dungeon (whether they entered by walking or used the module `mod-solo-lfg`), we do not rule out that an error may arise, so it is important that you help us test the content, and creating a report within the repository issues, in case you find a possible error .

To work, the module requires updated versions of the emulator, since some hooks that have existed for a long time are used, but some hooks also had to be created for it. So check your version of the emulator, with the `.server info` command and make sure you use the most updated version of it. Inside the `data/sql` folder is a simple script that adds translations of the welcome message. You can add the remaining languages, and the script should run automatically if you have the default options enabled within the `worldserver.conf`, otherwise you may have to run them manually.

# How is it used?

Clone the repository into the modules folder of the emulator, or download the zip and unzip it. Keep in mind that if you use the .zip, it will probably add a `-` to the end of the unzipped folder and then the name of the branch you have downloaded. We recommend cloning the repository, to be able, if it exists, to easily download updates without the need to download all the content again.

Thanks to all the people, who in one way or another participated and actively participate in this module, so that it can continue to be updated and working. If you are a collaborator or are interested in being one, do not hesitate to open a pull request, with the content that you would like the module to have or to fix any possible bug.
