 /*
  * UAE - The Un*x Amiga Emulator
  *
  * DispManX interface
  *
  * Raspberry Pi dispmanx by Chips
  * 
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <unistd.h>
#include <signal.h>

#include <SDL.h>
#include <SDL_endian.h>

#include "config.h"
#include "uae.h"
#include "options.h"
#include "memory-uae.h"
#include "xwin.h"
#include "custom.h"
#include "drawing.h"
#include "keyboard.h"
#include "keybuf.h"
#include "gui.h"
#include "debug.h"
#ifdef USE_UAE4ALL_VKBD
#include "vkbd/vkbd.h"
#endif
#include "gp2x.h"
#include "gp2xutil.h"
#include "inputmode.h"
#include "menu_config.h"
#include "menu.h"
#include "debug_uae4all.h"
#include "savestate.h"
#include "bcm_host.h"
#include "thread.h"

bool mouse_state = true;
bool slow_mouse = false;
extern int moved_x;
extern int moved_y;
extern int stylusClickOverride;
int mouseMoving = 0;
int justClicked = 0;
int fcounter = 0;
extern int stylusAdjustX;
extern int stylusAdjustY;
int doStylusRightClick = 0;

extern int gp2xMouseEmuOn;
extern int gp2xButtonRemappingOn;
#ifndef PANDORA
extern int hasGp2xButtonRemapping;
#endif

int show_inputmode = 0;
int show_volumecontrol;
extern int soundVolume;
extern int drawfinished;
int prefs_gfx_framerate, changed_gfx_framerate;

uae_u16 *prSDLScreenPixels;

uae_sem_t vsync_wait_sem;

Uint32 vsync_timing;

char *gfx_mem=NULL;
unsigned gfx_rowbytes=0;

Uint32 uae4all_numframes=0;


/* Uncomment for debugging output */
/* #define DEBUG */

/* SDL variable for output surface */
SDL_Surface *prSDLScreen = NULL;

DISPMANX_DISPLAY_HANDLE_T   dispmanxdisplay;
DISPMANX_MODEINFO_T         dispmanxdinfo;
DISPMANX_RESOURCE_HANDLE_T  dispmanxresource_amigafb_1;
DISPMANX_RESOURCE_HANDLE_T  dispmanxresource_amigafb_2;
DISPMANX_ELEMENT_HANDLE_T   dispmanxelement;
DISPMANX_UPDATE_HANDLE_T    dispmanxupdate;
VC_RECT_T       src_rect;
VC_RECT_T       dst_rect;
VC_RECT_T       blit_rect;


#ifdef USE_GUICHAN
extern char *screenshot_filename;
#endif

DISPMANX_RESOURCE_HANDLE_T  dispmanxresource_menu;
DISPMANX_RESOURCE_HANDLE_T  dispmanxresource_menu2;
DISPMANX_ELEMENT_HANDLE_T   dispmanxelement_menu;


VC_RECT_T       src_rect_menu;
VC_RECT_T       dst_rect_menu;
VC_RECT_T       blit_rect_menu;

unsigned char current_resource_amigafb = 0;

static int red_bits, green_bits, blue_bits;
static int red_shift, green_shift, blue_shift;
static SDL_Color arSDLColors[256];
static int ncolors = 0;

extern int visibleAreaWidth;
extern int mainMenu_displayedLines;


/* Keyboard and mouse */
int uae4all_keystate[256];
#if defined(PANDORA) || defined(ANDROIDSDL)
static int shiftWasPressed = 0;
#define SIMULATE_SHIFT 0x200
#define SIMULATE_RELEASED_SHIFT 0x400
#endif

int vsync_frequency = 0;
int old_time = 0;
int need_frameskip = 0;
int framecnt_hack = 0;

void vsync_callback(unsigned int a, void* b)
{
	vsync_timing=SDL_GetTicks();


	vsync_frequency = vsync_timing - old_time;
	old_time = vsync_timing;
	need_frameskip =  ( vsync_frequency > 31 ) ? (need_frameskip+1) : need_frameskip;
	//printf("d: %i", vsync_frequency     );

	uae_sem_post (&vsync_wait_sem);
}


void inputmode_redraw_dispmanX()
{
	// Still to be done
	#if 0
	SDL_Rect r;
	SDL_Surface* surface;

	r.x=80;
	r.y=prSDLScreen->h-200;
	r.w=160;
	r.h=160;

	if (inputMode[0] && inputMode[1] && inputMode[2])
	{
		if (gp2xMouseEmuOn)
		{
			surface = inputMode[1];
		}
		else if (gp2xButtonRemappingOn)
		{
			surface = inputMode[2];
		}
		else
		{
			surface = inputMode[0];
		}

		SDL_BlitSurface(surface,NULL,prSDLScreen,&r);
	}
	#endif
}


void flush_block ()
{
#ifdef USE_UAE4ALL_VKBD
	if (vkbd_mode)
		vkbd_key=vkbd_process();		
#endif
	if (show_inputmode)
		inputmode_redraw_dispmanX();	
	if (drawfinished)
	{
		drawfinished=0;
#ifdef USE_GUICHAN
		if (savestate_state == STATE_DOSAVE)
		  CreateScreenshot(SCREENSHOT);
#endif
		//SDL_Flip(prSDLScreen);
	if (current_resource_amigafb == 1)
	{
		current_resource_amigafb = 0;
		vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_1,
	                                    VC_IMAGE_RGB565,
	                                    visibleAreaWidth * 2,
	                                    gfx_mem,
	                                    &blit_rect );
		dispmanxupdate = vc_dispmanx_update_start( 10 );
		vc_dispmanx_element_change_source(dispmanxupdate,dispmanxelement,dispmanxresource_amigafb_1);

		vc_dispmanx_update_submit(dispmanxupdate,vsync_callback,NULL);

	}
	else
	{
		current_resource_amigafb = 1;
		vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_2,
	                                    VC_IMAGE_RGB565,
	                                    visibleAreaWidth * 2,
	                                    gfx_mem,
	                                    &blit_rect );
		dispmanxupdate = vc_dispmanx_update_start( 10 );
		vc_dispmanx_element_change_source(dispmanxupdate,dispmanxelement,dispmanxresource_amigafb_2);

		vc_dispmanx_update_submit(dispmanxupdate,vsync_callback,NULL);
	}

       /*vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_1,
                                            VC_IMAGE_RGB565,
                                            visibleAreaWidth * 2,
                                            gfx_mem,
                                            &blit_rect );*/


	}
	//SDL_LockSurface (prSDLScreen);

	if(stylusClickOverride)
	{
		justClicked = 0;
		mouseMoving = 0;
	}
	else
	{
		if(justClicked)
		{
			buttonstate[0] = 0;
			buttonstate[2] = 0;
			justClicked = 0;
		}

		if(mouseMoving)
		{
			if(fcounter >= mainMenu_tapDelay)
			{
				if(doStylusRightClick)
			  {
					buttonstate[2] = 1;
        }
				else
			  {
					buttonstate[0] = 1;
  				mouseMoving = 0;
  				justClicked = 1;
  				fcounter = 0;
				}
			}
			fcounter++;
		}
	}
	init_row_map();
}

void black_screen_now(void)
{
	// Avoid to draw next frame
	extern int framecnt;
	framecnt = 1;
	framecnt_hack = 0;

	// Clear screen
	memset (gfx_mem, 0 , visibleAreaWidth * mainMenu_displayedLines * 2 );
	vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_1,
	                                    VC_IMAGE_RGB565,
	                                    visibleAreaWidth * 2,
	                                    gfx_mem,
	                                    &blit_rect );
	vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_2,
	                                    VC_IMAGE_RGB565,
	                                    visibleAreaWidth * 2,
	                                    gfx_mem,
	                                    &blit_rect );
	dispmanxupdate = vc_dispmanx_update_start( 10 );
	vc_dispmanx_element_change_source(dispmanxupdate,dispmanxelement,dispmanxresource_amigafb_1);
	vc_dispmanx_update_submit(dispmanxupdate,NULL,NULL);
	//vc_dispmanx_update_submit_sync(dispmanxupdate);
	need_frameskip =  1;
}

static __inline__ int bitsInMask (unsigned long mask)
{
	/* count bits in mask */
	int n = 0;
	while (mask)
	{
		n += mask & 1;
		mask >>= 1;
	}
	return n;
}

static __inline__ int maskShift (unsigned long mask)
{
	/* determine how far mask is shifted */
	int n = 0;
	while (!(mask & 1))
	{
		n++;
		mask >>= 1;
	}
	return n;
}


static int init_colors (void)
{
	int i;

#ifdef DEBUG_GFX
	dbg("Function: init_colors");
#endif

		/* Truecolor: */
		//red_bits = bitsInMask(prSDLScreen->format->Rmask);
		//green_bits = bitsInMask(prSDLScreen->format->Gmask);
		//blue_bits = bitsInMask(prSDLScreen->format->Bmask);
		//red_shift = maskShift(prSDLScreen->format->Rmask);
		//green_shift = maskShift(prSDLScreen->format->Gmask);
		//blue_shift = maskShift(prSDLScreen->format->Bmask);
		red_bits = 5;
		green_bits = 6;
		blue_bits = 5;
		red_shift = 5 + 6;
		green_shift = 5;
		blue_shift = 0;
		alloc_colors64k (red_bits, green_bits, blue_bits, red_shift, green_shift, blue_shift);
		for (i = 0; i < 4096; i++)
			xcolors[i] = xcolors[i] * 0x00010001;

	return 1;
}

int graphics_setup (void)
{
    bcm_host_init();
    /* Initialize the SDL library */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 0;
    }
    // FDA should above gfxmem used ?
    //gfx_mem = (char *) calloc( 1, PREFS_GFX_WIDTH * PREFS_GFX_HEIGHT* 2 );

    return 1;
}

void graphics_dispmanshutdown (void)
{
    dispmanxupdate = vc_dispmanx_update_start( 10 );
    vc_dispmanx_element_remove( dispmanxupdate, dispmanxelement);
    vc_dispmanx_update_submit_sync(dispmanxupdate);
}


void graphics_subinit (void)
{
	VC_DISPMANX_ALPHA_T alpha = { (DISPMANX_FLAGS_ALPHA_T ) (DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS), 
                             255, /*alpha 0->255*/
                             0 };

	uint32_t                    vc_image_ptr;

	if (prSDLScreen == NULL)
	{
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		return;
	}
	else
	{
		//prSDLScreenPixels=(uae_u16 *)prSDLScreen->pixels;
		//SDL_LockSurface(prSDLScreen);
		//SDL_UnlockSurface(prSDLScreen);
		//SDL_Flip(prSDLScreen);
		//SDL_ShowCursor(SDL_DISABLE);
		/* Initialize structure for Amiga video modes */
		//gfx_mem = (char *)prSDLScreen->pixels;
		//gfx_rowbytes = prSDLScreen->pitch;

		// check if resolution hasn't change in menu. otherwise free the resources so that they will be re-generated with new resolution.
		if ((dispmanxresource_amigafb_1 != 0) && ((blit_rect.width != visibleAreaWidth) || (blit_rect.height != mainMenu_displayedLines)))
		{
			printf("Resolution change detected.\n");
			graphics_dispmanshutdown();
			vc_dispmanx_resource_delete( dispmanxresource_amigafb_1 );
			vc_dispmanx_resource_delete( dispmanxresource_amigafb_2 );
			dispmanxresource_amigafb_1 = 0;
			dispmanxresource_amigafb_2 = 0;
		}


		if (dispmanxresource_amigafb_1 == 0)
		{
			dispmanxdisplay = vc_dispmanx_display_open( 0 );
			vc_dispmanx_display_get_info( dispmanxdisplay, &dispmanxdinfo);

			printf("Emulation resolution: Width %i Height: %i\n",visibleAreaWidth,mainMenu_displayedLines);

			dispmanxresource_amigafb_1 = vc_dispmanx_resource_create( VC_IMAGE_RGB565,  visibleAreaWidth,   mainMenu_displayedLines,  &vc_image_ptr);
			dispmanxresource_amigafb_2 = vc_dispmanx_resource_create( VC_IMAGE_RGB565,  visibleAreaWidth,   mainMenu_displayedLines,  &vc_image_ptr);
			vc_dispmanx_rect_set( &blit_rect, 0, 0, visibleAreaWidth,mainMenu_displayedLines);
			vc_dispmanx_resource_write_data(  dispmanxresource_amigafb_1,
		                                    VC_IMAGE_RGB565,
		                                    visibleAreaWidth *2,
		                                    gfx_mem,
		                                    &blit_rect );
			vc_dispmanx_rect_set( &src_rect, 0, 0, visibleAreaWidth << 16, mainMenu_displayedLines << 16 );



		}
		// Currently only fullscreen is available in uae4all2
		//if (mainMenu_screenformat == 0)
		if (1)
		{
			vc_dispmanx_rect_set( &dst_rect, (dispmanxdinfo.width * 2)/100,
									(dispmanxdinfo.height * 2)/100 ,
									dispmanxdinfo.width - (dispmanxdinfo.width * 4)/100 ,
									dispmanxdinfo.height - (dispmanxdinfo.height * 7)/100 );
			// For debug
			//vc_dispmanx_rect_set( &dst_rect, (dispmanxdinfo.width /2),
		        //                     (dispmanxdinfo.height * 3)/100 ,
		        //                     (dispmanxdinfo.width - (dispmanxdinfo.width * 6)/100 )/2,
		        //                     (dispmanxdinfo.height - (dispmanxdinfo.height * 7)/100 )/2);

		}
		else
		{
			vc_dispmanx_rect_set( &dst_rect, ((dispmanxdinfo.width * 17)/100) ,
									(dispmanxdinfo.height * 3)/100 ,
									(dispmanxdinfo.width - ((dispmanxdinfo.width * 36)/100)) ,
									dispmanxdinfo.height - (dispmanxdinfo.height * 7)/100 );
		}


		dispmanxupdate = vc_dispmanx_update_start( 10 );
		dispmanxelement = vc_dispmanx_element_add( dispmanxupdate,
		                                        dispmanxdisplay,
		                                        2000,               // layer
		                                        &dst_rect,
		                                        dispmanxresource_amigafb_1,
		                                        &src_rect,
		                                        DISPMANX_PROTECTION_NONE,
		                                        &alpha,
		                                        NULL,             // clamp
		                                        DISPMANX_NO_ROTATE );

		vc_dispmanx_update_submit(dispmanxupdate,NULL,NULL);
		//dispmanxupdate = vc_dispmanx_update_start( 10 );
	}
	lastmx = lastmy = 0;
	newmousecounters = 0;
	gfx_rowbytes = visibleAreaWidth *2;
}

int graphics_init (void)
{
	int i,j;

	// this will hold the state of the mouse emulation toggle.  The start button
	// will enable mouse emulation, which will allow the joystick to move the 
	// mouse point (probably badly, but there you go).
	gp2xButtonRemappingOn = 0;
	show_volumecontrol = 0;

	uae_sem_init (&vsync_wait_sem, 0, 1);

	graphics_subinit ();

	check_all_prefs();	

    if (!init_colors ())
		return 0;

    buttonstate[0] = buttonstate[1] = buttonstate[2] = 0;
    for (i = 256; i--;)
		uae4all_keystate[i] = 0;
  shiftWasPressed = 0;
  
    return 1;
}


static void graphics_subshutdown (void)
{
#ifndef AROS
    //SDL_FreeSurface(prSDLScreen);
#endif
    graphics_dispmanshutdown();
    vc_dispmanx_resource_delete( dispmanxresource_amigafb_1 );
	vc_dispmanx_resource_delete( dispmanxresource_amigafb_2 );
    vc_dispmanx_display_close( dispmanxdisplay );
    SDL_FreeSurface(prSDLScreen);
    #if DEBUG_GFX
    dbgf("Killed %d %d %d %d\n", dispmanxupdate , dispmanxelement, dispmanxresource_amigafb_1 , dispmanxdisplay);
    #endif
}

void graphics_leave (void)
{
    graphics_subshutdown ();
	SDL_VideoQuit();
    dumpcustom ();
}

/* Decode KeySyms. This function knows about all keys that are common
 * between different keyboard languages. */
static int kc_decode (SDL_keysym *prKeySym)
{
// Chips: We should not use sym but scancode instead as latest FS-UAE !
    switch (prKeySym->sym)
    {
    case SDLK_b: return AK_B;
    case SDLK_c: return AK_C;
    case SDLK_d: return AK_D;
    case SDLK_e: return AK_E;
    case SDLK_f: return AK_F;
    case SDLK_g: return AK_G;
    case SDLK_h: return AK_H;
    case SDLK_i: return AK_I;
    case SDLK_j: return AK_J;
    case SDLK_k: return AK_K;
    case SDLK_l: return AK_L;
    case SDLK_n: return AK_N;
    case SDLK_o: return AK_O;
    case SDLK_p: return AK_P;
    case SDLK_r: return AK_R;
    case SDLK_s: return AK_S;
    case SDLK_t: return AK_T;
    case SDLK_u: return AK_U;
    case SDLK_v: return AK_V;
    case SDLK_x: return AK_X;

    case SDLK_0: return AK_0;
    case SDLK_1: return AK_1;
    case SDLK_2: return AK_2;
    case SDLK_3: return AK_3;
    case SDLK_4: return AK_4;
    case SDLK_5: return AK_5;
    case SDLK_6: return AK_6;
    case SDLK_7: return AK_7;
    case SDLK_8: return AK_8;
    case SDLK_9: return AK_9;

    case SDLK_KP0: return AK_NP0;
    case SDLK_KP1: return AK_NP1;
    case SDLK_KP2: return AK_NP2;
    case SDLK_KP3: return AK_NP3;
    case SDLK_KP4: return AK_NP4;
    case SDLK_KP5: return AK_NP5;
    case SDLK_KP6: return AK_NP6;
    case SDLK_KP7: return AK_NP7;
    case SDLK_KP8: return AK_NP8;

    case SDLK_KP9: return AK_NP9;
    case SDLK_KP_DIVIDE: return AK_NPDIV;
    case SDLK_KP_MULTIPLY: return AK_NPMUL;
    case SDLK_KP_MINUS: return AK_NPSUB;
    case SDLK_KP_PLUS: return AK_NPADD;
    case SDLK_KP_PERIOD: return AK_NPDEL;
    case SDLK_KP_ENTER: return AK_ENT;

    case SDLK_F1: return AK_F1;
    case SDLK_F2: return AK_F2;
    case SDLK_F3: return AK_F3;
    case SDLK_F4: return AK_F4;
    case SDLK_F5: return AK_F5;
    case SDLK_F6: return AK_F6;
    case SDLK_F7: return AK_F7;
    case SDLK_F8: return AK_F8;
    case SDLK_F9: return AK_F9;
    case SDLK_F10: return AK_F10;

    case SDLK_BACKSPACE: return AK_BS;
    case SDLK_DELETE: return AK_DEL;
    case SDLK_LCTRL: return AK_CTRL;
    case SDLK_RCTRL: return AK_RCTRL;
    case SDLK_TAB: return AK_TAB;
    case SDLK_LALT: return AK_LALT;
    case SDLK_RALT: return AK_RALT;
    case SDLK_RMETA: return AK_RAMI;
    case SDLK_LMETA: return AK_LAMI;
    case SDLK_RETURN: return AK_RET;
    case SDLK_SPACE: return AK_SPC;
    case SDLK_LSHIFT: return AK_LSH;
    case SDLK_RSHIFT: return AK_RSH;
    case SDLK_ESCAPE: return AK_ESC;

    case SDLK_INSERT: return AK_HELP;
    case SDLK_HOME: return AK_NPLPAREN;
    case SDLK_END: return AK_NPRPAREN
;
    case SDLK_CAPSLOCK: return AK_CAPSLOCK;

    case SDLK_UP: return AK_UP;
    case SDLK_DOWN: return AK_DN;
    case SDLK_LEFT: return AK_LF;
    case SDLK_RIGHT: return AK_RT;

    case SDLK_PAGEUP:
    case SDLK_RSUPER:	return AK_RAMI;  /* PgUp mapped to right amiga */

    case SDLK_PAGEDOWN:
    case SDLK_LSUPER:	return AK_LAMI;  /* PgDn mapped to left amiga */

    default: return -1;
    }
}


/*
 * Handle keys specific to French (and Belgian) keymaps.
 *
 * Number keys are broken
 */
static int decode_fr (SDL_keysym *prKeySym)
{
    switch(prKeySym->sym) {
	case SDLK_a:		return AK_Q;
	case SDLK_m:		return AK_SEMICOLON;
	case SDLK_q:		return AK_A;
	case SDLK_y:		return AK_Y;
	case SDLK_w:		return AK_Z;
	case SDLK_z:		return AK_W;
	case SDLK_LEFTBRACKET:	return AK_LBRACKET;
	case SDLK_RIGHTBRACKET: return AK_RBRACKET;
	case SDLK_COMMA:	return AK_M;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_PERIOD:
	case SDLK_SEMICOLON:	return AK_COMMA;
	case SDLK_RIGHTPAREN:	return AK_MINUS;
	case SDLK_EQUALS:	return AK_SLASH;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	case SDLK_SLASH:	return AK_PERIOD;      // Is it true ?
	case SDLK_MINUS:	return AK_EQUAL;
	case SDLK_BACKSLASH:	return AK_SLASH;
	case SDLK_COLON:	return AK_PERIOD;
	case SDLK_EXCLAIM:	return AK_BACKSLASH;   // Not really...
	default: printf("Unknown key: %i\n",prKeySym->sym); return -1;
    }
}


static int decode_us (SDL_keysym *prKeySym)
{
    switch(prKeySym->sym)
    {
	/* US specific */
    case SDLK_a: return AK_A;
    case SDLK_m: return AK_M;
    case SDLK_q: return AK_Q;
    case SDLK_y: return AK_Y;
    case SDLK_w: return AK_W;
    case SDLK_z: return AK_Z;
    case SDLK_LEFTBRACKET: return AK_LBRACKET;
    case SDLK_RIGHTBRACKET: return AK_RBRACKET;
    case SDLK_COMMA: return AK_COMMA;
    case SDLK_PERIOD: return AK_PERIOD;
    case SDLK_SLASH: return AK_SLASH;
    case SDLK_SEMICOLON: return AK_SEMICOLON;
    case SDLK_MINUS: return AK_MINUS;
    case SDLK_EQUALS: return AK_EQUAL;
	/* this doesn't work: */
    case SDLK_BACKQUOTE: return AK_QUOTE;
    case SDLK_QUOTE: return AK_BACKQUOTE;
    case SDLK_BACKSLASH: return AK_BACKSLASH;
    default: 	return -1;
    }
}

/*
 * Handle keys specific to German keymaps.
 */
static int decode_de (SDL_keysym *prKeySym)
{
    switch(prKeySym->sym) {
	case SDLK_a:		return AK_A;
	case SDLK_m:		return AK_M;
	case SDLK_q:		return AK_Q;
	case SDLK_w:		return AK_W;
	case SDLK_y:		return AK_Z;
	case SDLK_z:		return AK_Y;
	/* German umlaut oe */
	case SDLK_WORLD_86:	return AK_SEMICOLON;
	/* German umlaut ae */
	case SDLK_WORLD_68:	return AK_QUOTE;
	/* German umlaut ue */
	case SDLK_WORLD_92:	return AK_LBRACKET;
	case SDLK_PLUS:
	case SDLK_ASTERISK:	return AK_RBRACKET;
	case SDLK_COMMA:	return AK_COMMA;
	case SDLK_PERIOD:	return AK_PERIOD;
	case SDLK_LESS:
	case SDLK_GREATER:	return AK_LTGT;
	case SDLK_HASH:		return AK_NUMBERSIGN;
	/* German sharp s */
	case SDLK_WORLD_63:	return AK_MINUS;
	case SDLK_QUOTE:	return AK_EQUAL;
	case SDLK_CARET:	return AK_BACKQUOTE;
	case SDLK_MINUS:	return AK_SLASH;
	default: return -1;
    }
}

int keycode2amiga(SDL_keysym *prKeySym)
{
    int iAmigaKeycode = kc_decode(prKeySym);
    if (iAmigaKeycode == -1)
            return decode_us(prKeySym);
    return iAmigaKeycode;
}

static int refresh_necessary = 0;

void handle_events (void)
{
    SDL_Event rEvent;
    int iAmigaKeyCode;
    int i, j;
    int iIsHotKey = 0;

    /* Handle GUI events */
    gui_handle_events ();
    
    while (SDL_PollEvent(&rEvent))
    {
		switch (rEvent.type)
		{
		case SDL_QUIT:
			uae_quit();
			break;
		case SDL_KEYDOWN:
			if(rEvent.key.keysym.sym==SDLK_PAGEUP)
				slow_mouse=true;
			if(gp2xMouseEmuOn)
			{
				if(rEvent.key.keysym.sym==SDLK_RSHIFT)
				{
					uae4all_keystate[AK_LALT] = 1;
					record_key(AK_LALT << 1);
				}
				if(rEvent.key.keysym.sym==SDLK_RCTRL || rEvent.key.keysym.sym==SDLK_END || rEvent.key.keysym.sym==SDLK_HOME)
				{
					uae4all_keystate[AK_RALT] = 1;
					record_key(AK_RALT << 1);
				}
				if(rEvent.key.keysym.sym==SDLK_PAGEDOWN)
				{
					uae4all_keystate[AK_DN] = 1;
					record_key(AK_DN << 1);
				}
			}
			
			if (rEvent.key.keysym.sym==SDLK_LALT)
			{
#ifdef USE_UAE4ALL_VKBD
				if (!vkbd_mode)
#endif
				{
					// only do this if the virtual keyboard isn't visible
					// state moves thus:
					// joystick mode (with virt keyboard on L and R)
					// mouse mode (with mouse buttons on L and R)
					// if specified:
					// remapping mode (with whatever's been supplied)
					// back to start of state
#ifndef PANDORA
					if (!hasGp2xButtonRemapping)
					{
						// skip the remapping state
						gp2xMouseEmuOn = !gp2xMouseEmuOn;
					}
					else
					{
#endif
						// start condition is gp2xMouseEmuOn = 0, gp2xButtonRemappingOn = 0
						if (!gp2xButtonRemappingOn && !gp2xMouseEmuOn)
						{
							// move to mouse emu mode
							gp2xMouseEmuOn = 1;
							gp2xButtonRemappingOn = 0;
						}
						else if (gp2xMouseEmuOn && !gp2xButtonRemappingOn)
						{
						// move to button remapping mode
						gp2xMouseEmuOn = 0;
						gp2xButtonRemappingOn = 1;
						}
						else if (!gp2xMouseEmuOn && gp2xButtonRemappingOn)
						{
							gp2xMouseEmuOn = 0;
							gp2xButtonRemappingOn = 0;
						}
							if (!gp2xMouseEmuOn)
								togglemouse();
#ifndef PANDORA
					}
#endif

				show_inputmode = 1;
				}
			}
#ifdef USE_UAE4ALL_VKBD
			else if ((!gp2xMouseEmuOn) && (!gp2xButtonRemappingOn) && (!vkbd_mode) && (vkbd_button2!=(SDLKey)0))
			{
				if (vkbd_button2)
					rEvent.key.keysym.sym=vkbd_button2;
				else
					break;
			}
#endif
#ifndef PANDORA
			if (gp2xButtonRemappingOn)
#endif
			{
				if (rEvent.key.keysym.sym==SDLK_RSHIFT || rEvent.key.keysym.sym==SDLK_RCTRL)
					doStylusRightClick = 1;
			}
			if (rEvent.key.keysym.sym!=SDLK_UP && rEvent.key.keysym.sym!=SDLK_DOWN && rEvent.key.keysym.sym!=SDLK_LEFT &&
				rEvent.key.keysym.sym!=SDLK_RIGHT && rEvent.key.keysym.sym!=SDLK_PAGEUP && rEvent.key.keysym.sym!=SDLK_PAGEDOWN &&
				rEvent.key.keysym.sym!=SDLK_HOME && rEvent.key.keysym.sym!=SDLK_END && rEvent.key.keysym.sym!=SDLK_LALT &&
				rEvent.key.keysym.sym!=SDLK_LCTRL && rEvent.key.keysym.sym!=SDLK_RSHIFT && rEvent.key.keysym.sym!=SDLK_RCTRL)
			{
				iAmigaKeyCode = keycode2amiga(&(rEvent.key.keysym));
				if (iAmigaKeyCode >= 0)
				{
#ifdef PANDORA
				  if(iAmigaKeyCode & SIMULATE_SHIFT)
			    {
            // We need to simulate shift
            iAmigaKeyCode = iAmigaKeyCode & 0x1ff;
            shiftWasPressed = uae4all_keystate[AK_LSH];
            if(!shiftWasPressed)
            {
              uae4all_keystate[AK_LSH] = 1;
              record_key(AK_LSH << 1);
            }
			    }
				  if(iAmigaKeyCode & SIMULATE_RELEASED_SHIFT)
			    {
            // We need to simulate released shift
            iAmigaKeyCode = iAmigaKeyCode & 0x1ff;
            shiftWasPressed = uae4all_keystate[AK_LSH];
            if(shiftWasPressed)
            {
              uae4all_keystate[AK_LSH] = 0;
              record_key((AK_LSH << 1) | 1);
            }
			    }
#endif
					if (!uae4all_keystate[iAmigaKeyCode])
					{
						uae4all_keystate[iAmigaKeyCode] = 1;
						record_key(iAmigaKeyCode << 1);
					}
				}
			}
			break;
		case SDL_KEYUP:
			if(rEvent.key.keysym.sym==SDLK_PAGEUP)
				slow_mouse = false;
			if(gp2xMouseEmuOn)
			{
				if(rEvent.key.keysym.sym==SDLK_RSHIFT)
				{
					uae4all_keystate[AK_LALT] = 0;
					record_key((AK_LALT << 1) | 1);
				}
				if(rEvent.key.keysym.sym==SDLK_RCTRL || rEvent.key.keysym.sym==SDLK_END || rEvent.key.keysym.sym==SDLK_HOME)
				{
					uae4all_keystate[AK_RALT] = 0;
					record_key((AK_RALT << 1) | 1);
				}
				if(rEvent.key.keysym.sym==SDLK_PAGEDOWN)
				{
					uae4all_keystate[AK_DN] = 0;
					record_key((AK_DN << 1) | 1);
				}
			}
#ifndef PANDORA
			if (gp2xButtonRemappingOn)
#endif
			{
				if (rEvent.key.keysym.sym==SDLK_RSHIFT || rEvent.key.keysym.sym==SDLK_RCTRL)
			  {
					doStylusRightClick = 0;
  				mouseMoving = 0;
  				justClicked = 0;
  				fcounter = 0;
  				buttonstate[2] = 0;
				}
			}
			if (rEvent.key.keysym.sym==SDLK_LALT)
			{
				show_inputmode = 0;
			}
			if (rEvent.key.keysym.sym!=SDLK_UP && rEvent.key.keysym.sym!=SDLK_DOWN && rEvent.key.keysym.sym!=SDLK_LEFT &&
				rEvent.key.keysym.sym!=SDLK_RIGHT && rEvent.key.keysym.sym!=SDLK_PAGEUP && rEvent.key.keysym.sym!=SDLK_PAGEDOWN &&
				rEvent.key.keysym.sym!=SDLK_HOME && rEvent.key.keysym.sym!=SDLK_END && rEvent.key.keysym.sym!=SDLK_LALT &&
				rEvent.key.keysym.sym!=SDLK_LCTRL && rEvent.key.keysym.sym!=SDLK_RSHIFT && rEvent.key.keysym.sym!=SDLK_RCTRL)
			{
				iAmigaKeyCode = keycode2amiga(&(rEvent.key.keysym));
				if (iAmigaKeyCode >= 0)
				{
#ifdef PANDORA
				  if(iAmigaKeyCode & SIMULATE_SHIFT)
			    {
            // We needed to simulate shift
            iAmigaKeyCode = iAmigaKeyCode & 0x1ff;
            if(!shiftWasPressed)
            {
              uae4all_keystate[AK_LSH] = 0;
              record_key((AK_LSH << 1) | 1);
              shiftWasPressed = 0;
            }
			    }
				  if(iAmigaKeyCode & SIMULATE_RELEASED_SHIFT)
			    {
            // We needed to simulate released shift
            iAmigaKeyCode = iAmigaKeyCode & 0x1ff;
            if(shiftWasPressed)
            {
              uae4all_keystate[AK_LSH] = 1;
              record_key(AK_LSH << 1);
              shiftWasPressed = 0;
            }
			    }
#endif
					uae4all_keystate[iAmigaKeyCode] = 0;
					record_key((iAmigaKeyCode << 1) | 1);
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		if (!gp2xButtonRemappingOn)
			buttonstate[(rEvent.button.button-1)%3] = 1;
			break;
		case SDL_MOUSEBUTTONUP:
		if (!gp2xButtonRemappingOn)
			buttonstate[(rEvent.button.button-1)%3] = 0;
			break;
		case SDL_MOUSEMOTION:
			mouse_state = true;
			if(gp2xButtonRemappingOn)
			{
				lastmx = rEvent.motion.x*2 - mainMenu_stylusOffset + moved_x + stylusAdjustX >> 1;
				lastmy = rEvent.motion.y*2 - mainMenu_stylusOffset + moved_y + stylusAdjustY >> 1;
				//mouseMoving = 1;
			}
			else if(slow_mouse)
			{
				lastmx += rEvent.motion.xrel;
				lastmy += rEvent.motion.yrel;
				if(rEvent.motion.x == 0)
					lastmx -= 2;
				if(rEvent.motion.y == 0)
					lastmy -= 2;
				if(rEvent.motion.x == visibleAreaWidth-1)
					lastmx += 2;
				if(rEvent.motion.y == mainMenu_displayedLines-1)
					lastmy += 2;
			}
			else
			{
				int mouseScale = mainMenu_mouseMultiplier;
				if (mouseScale > 99)
					mouseScale /= 100;

				lastmx += rEvent.motion.xrel * mouseScale;
				lastmy += rEvent.motion.yrel * mouseScale;
				if(rEvent.motion.x == 0)
					lastmx -= mouseScale;
				if(rEvent.motion.y == 0)
					lastmy -= mouseScale;
				if(rEvent.motion.x == visibleAreaWidth-1)
					lastmx += mouseScale;
				if(rEvent.motion.y == mainMenu_displayedLines-1)
					lastmy += mouseScale;
			}
			newmousecounters = 1;
			break;
		}
    }

    if (mouse_state==false)
    {
		mouse_x = 0;
		mouse_y = 0;
		newmousecounters = 1;
    }
}

int needmousehack (void)
{
    return 1;
}

int lockscr (void)
{
    //SDL_LockSurface(prSDLScreen);
    return 1;
}

void unlockscr (void)
{
    //SDL_UnlockSurface(prSDLScreen);
}

void gui_purge_events(void)
{
	int counter = 0;

	SDL_Event event;
	SDL_Delay(150);
	// Strangely PS3 controller always send events, so we need a maximum number of event to purge.
	while(SDL_PollEvent(&event) && counter < 100)
	{
		counter++;
		SDL_Delay(10);
	}
	keybuf_init();
}
