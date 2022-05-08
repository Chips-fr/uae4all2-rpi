ifneq (,$(filter $(TARGET),pi2 bananapi))
	DEFS += -DUSE_ARMV7 -DUSE_ARMNEON
	CPU_FLAGS += -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
	NEON=1
else ifeq ($(TARGET),pi1)
	CPU_FLAGS += -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard
else ifeq ($(TARGET),armv6)
	CPU_FLAGS += -march=armv6j -mfpu=vfp -mfloat-abi=hard
endif

PROG   = uae4all
O      = o
RM     = rm -f
STRIP  = strip
#CXX    = g++-4.8

all: $(PROG)

FAME_CORE=1
FAME_CORE_C=1
#SDLSOUND=1
NEWSDLSOUND=1
#DEBUG=1
#TEXT_GUI=1
GUICHAN_GUI=1
#ANDROIDSDL=1


SDL_CXXFLAGS = `sdl-config --cflags`
LDFLAGS = -lbcm_host -L/opt/vc/lib -Lguichan/src/.libs -Lguichan/src/sdl/.libs -lguichan_sdl -lguichan -lSDL_image -lz -lpng -lpthread `sdl-config --libs` -lSDL_ttf -lrt
MORE_CXXFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I./guichan/include/
MORE_CXXFLAGS +=   -Isrc -Isrc/gp2x -Isrc/vkbd -Isrc/menu -Isrc/include -Isrc/gp2x/menu -fomit-frame-pointer -Wno-unused -Wno-format -DUSE_SDL -DGCCCONSTFUNC="__attribute__((const))" -DUSE_UNDERSCORE -fexceptions -DUNALIGNED_PROFITABLE -DOPTIMIZED_FLAGS -DSHM_SUPPORT_LINKS=0 -DOS_WITHOUT_MEMORY_MANAGEMENT  -DVKBD_ALWAYS -DUSE_UAE4ALL_VKBD

MORE_CXXFLAGS += -DRASPBERRY -DSIX_AXIS_WORKAROUND
MORE_CXXFLAGS += -DGP2X -DPANDORA
MORE_CXXFLAGS += -DROM_PATH_PREFIX=\"./\" -DDATA_PREFIX=\"./data/\" -DSAVE_PREFIX=\"./saves/\"

#MORE_CXXFLAGS += -fsanitize=address -fsanitize=bounds
#LDFLAGS += -static-libasan -fsanitize=address -fsanitize=bounds


ifndef DEBUG
MORE_CXXFLAGS += -O3 -ffast-math
MORE_CXXFLAGS += -fstrict-aliasing
MORE_CXXFLAGS += -falign-functions=32
MORE_CXXFLAGS += -finline -finline-functions -fno-builtin
else
MORE_CXXFLAGS += -ggdb -DDEBUG_GFX 
endif

MORE_CXXFLAGS+= -DUSE_AUTOCONFIG
MORE_CXXFLAGS+= -DUSE_ZFILE
MORE_CXXFLAGS+= -DFAME_INTERRUPTS_PATCH
# Turrican3 becomes unstable if this is not enabled
MORE_CXXFLAGS+= -DSAFE_MEMORY_ACCESS
#MORE_CXXFLAGS+= -DDEBUG_SAVESTATE

CXXFLAGS  += $(CPU_FLAGS) $(SDL_CXXFLAGS) $(DEFS) $(MORE_CXXFLAGS)

ifdef NEON
	ASFLAGS += -mfpu=neon
endif
ASFLAGS += -mfloat-abi=hard

#	src/raspgfx.o \
#	src/sdlgfx.o \

OBJS =	\
	src/audio.o \
	src/autoconf.o \
	src/blitfunc.o \
	src/blittable.o \
	src/blitter.o \
	src/cfgfile.o \
	src/cia.o \
	src/savedisk.o \
	src/savestate.o \
	src/custom.o \
	src/disk.o \
	src/drawing.o \
	src/ersatz.o \
	src/expansion.o \
	src/filesys.o \
	src/fsdb.o \
	src/fsdb_unix.o \
	src/fsusage.o \
	src/gfxutil.o \
	src/hardfile.o \
	src/keybuf.o \
	src/main.o \
	src/memory.o \
	src/missing.o \
	src/native2amiga.o \
	src/gui.o \
	src/od-joy.o \
	src/scsi-none.o \
	src/raspgfx.o \
	src/writelog.o \
	src/zfile.o \
	src/menu/fade.o \
	src/vkbd/vkbd.o \
	src/gp2x/memcpy.o \
	src/gp2x/memset.o \
	src/gp2x/gp2x.o \
	src/gp2x/inputmode.o \
	src/gp2x/menu/menu_helper.o \
	src/gp2x/menu/menu_config.o \
	src/gp2x/menu/menu.o

ifdef NEON
	OBJS += src/neon_helper.o
endif

ifdef GUICHAN_GUI
CXXFLAGS+= -DUSE_GUICHAN
OBJS += src/menu_guichan/menu_guichan.o \
	src/menu_guichan/menuTabMain.o \
	src/menu_guichan/menuTabFloppy.o \
	src/menu_guichan/menuTabHD.o \
	src/menu_guichan/menuTabDisplaySound.o \
	src/menu_guichan/menuTabSavestates.o \
	src/menu_guichan/menuTabControl.o \
	src/menu_guichan/menuTabCustomCtrl.o \
	src/menu_guichan/menuMessage.o \
	src/menu_guichan/menuLoad_guichan.o \
	src/menu_guichan/menuConfigManager.o \
	src/menu_guichan/uaeradiobutton.o \
	src/menu_guichan/uaedropdown.o
ifdef PANDORA
OBJS += src/menu_guichan/sdltruetypefont.o
endif
else
OBJS += src/gp2x/menu/menu_fileinfo.o \
	src/gp2x/menu/menu_load.o \
	src/gp2x/menu/menu_main.o \
	src/gp2x/menu/menu_savestates.o \
	src/gp2x/menu/menu_misc.o \
	src/gp2x/menu/menu_controls.o \
	src/gp2x/menu/menu_display.o \
	src/gp2x/menu/menu_memory_disk.o
endif

ifdef SDLSOUND
CXXFLAGS+= -DUSE_SDLSOUND
OBJS += src/sound.o
else
ifdef NEWSDLSOUND
OBJS += src/sound_sdl_new.o
else
OBJS += src/sound_gp2x.o
endif
endif

ifdef FAME_CORE
ifdef FAME_CORE_C
CXXFLAGS+=-DUSE_FAME_CORE -DUSE_FAME_CORE_C -DFAME_IRQ_CLOCKING -DFAME_CHECK_BRANCHES -DFAME_EMULATE_TRACE -DFAME_DIRECT_MAPPING -DFAME_BYPASS_TAS_WRITEBACK -DFAME_ACCURATE_TIMING -DFAME_GLOBAL_CONTEXT -DFAME_FETCHBITS=8 -DFAME_DATABITS=8 -DFAME_NO_RESTORE_PC_MASKED_BITS
CXXFLAGS+=-DWITH_TESTMODE

src/m68k/fame/famec.o: src/m68k/fame/famec.cpp
OBJS += src/m68k/fame/famec.o
else
CXXFLAGS+=-DUSE_FAME_CORE
src/m68k/fame/fame.o: src/m68k/fame/fame.asm
	nasm src/m68k/fame/fame.asm
OBJS += src/m68k/fame/fame.o
endif
OBJS += src/m68k/fame/m68k_intrf.o
OBJS += src/m68k/m68k_cmn_intrf.o
else
OBJS += \
	src/m68k/uae/newcpu.o \
	src/m68k/uae/readcpu.o \
	src/m68k/uae/cpudefs.o \
	src/m68k/uae/fpp.o \
	src/m68k/uae/cpustbl.o \
	src/m68k/uae/cpuemu.o
endif

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS) -lpng

#ifndef DEBUG
#	$(STRIP) $(PROG)
#endif

clean:
	$(RM) $(PROG) $(OBJS)
