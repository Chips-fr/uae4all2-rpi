Uae4all2 for Raspberry Pi by Chips
A fast and optimized Amiga Emulator

Raspberry Pi version history:


V0.5:
   Enable CPU frequency emulation above 28 Mhz.
   Resolution set by command line option is working.
   Add Sony six axis workaround.
   Add joystick hat support.

V0.4:
   enable screen resolution change in guichan menu.
   enable vkeybd: now Left shift is Fire button of joystick

V0.3:
   Now two binary: one for Raspberry Pi 1 and another one for Raspberry Pi 2 (Neon enabled)
   Switch to gcc 4.8 compiler instead of 4.6.
   Solve the emulator stuck at exit.

V0.2:
   Merge of latest AGA improvement from TomB.
   Add command line parameter management. Just write any wrong parameter to get the list of them :p

V0.1:
   Original version

How to compile on Raspbian:
   You need to install gcc-4.8 (or edit Makefile) and install some dev package.
   In order to do this enter the following in command line:
   sudo apt-get install g++-4.8
   sudo apt-get install libsdl1.2-dev
   sudo apt-get install libsdl-image1.2-dev
   sudo apt-get install libsdl-gfx1.2-dev
   sudo apt-get install libguichan-dev
   sudo apt-get install libsdl-ttf2.0-dev

   Then to compile for rpi1:
      export TARGET="pi1"
      make

   or for rpi2 (for Neon and Cortex optimization):
      export TARGET="pi2"
      make


General readme:

Features: AGA/OCS/ECS, 68020 and 68000 emulation, harddisk-support, WHDLoad-support, Chip/Slow/Fast-mem settings, savestates, vsync, most games run fullspeed

Authors:
Chui, john4p, TomB, notaz, Bernd Schneider, Toni Wilen, Pickle, smoku, AnotherGuest, Anonymous engineer, finkel, Lubomyr, pelya
android port by Lubomyr, android libSDL by pelya 

Android-port info

On-screen scheme

4 3 ( YX)
2 1 ( BA)

'1'- <Button A> autofire [LSHIFT]
'2'- <Button B> Right mouse click [END]
'3'- <Button X> joystick fire/left click [PAGEDOWN]
'4'- <Button Y> [PAGEUP]
'5' <Button L> [SDLK_RSHIFT]
'6' <Button R> [SDLK_RCTRL]
old textUI vkeybd - '2'+'4'
 
SDL keycode action:
F15 textUI vkeybd
LALT - change input method

Please put kickstarts files kickstarts directory.
Files must be named as kick13.rom kick20.rom kick31.rom
