# Uae4all2 for Raspberry Pi by Chips

A fast and optimized OCS and AGA Amiga Emulator

How to compile on Raspbian:

   In order to do this enter the following in command line:

      sudo apt-get install libsdl1.2-dev
      sudo apt-get install libsdl-image1.2-dev
      sudo apt-get install libsdl-gfx1.2-dev
      sudo apt-get install libguichan-dev
      sudo apt-get install libsdl-ttf2.0-dev

   Then to compile for rpi1:

      make TARGET="pi1"

   or for rpi2 (for Neon and Cortex optimization):

      make TARGET="pi2"

General readme:

Features: AGA/OCS/ECS, 68020 and 68000 emulation, harddisk-support, WHDLoad-support, Chip/Slow/Fast-mem settings, savestates, vsync, most games run fullspeed

Authors:
Chui, john4p, TomB, notaz, Bernd Schneider, Toni Wilen, Pickle, smoku, AnotherGuest, Anonymous engineer, finkel, Lubomyr, pelya
android port by Lubomyr, android libSDL by pelya 

Use F12 in order to enter/exit menu.

Put kickstarts files in kickstarts directory. Files must be named as kick13.rom kick20.rom kick31.rom
