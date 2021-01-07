#include <Arduino.h>
#include "gbConfig.h"
#ifndef SDL_H
#define SDL_H

#define GAMEBOY_HEIGHT 144
#define GAMEBOY_WIDTH 160

static unsigned char * pixels = NULL; //(byte *)malloc(5760);
//#ifdef use_lib_vga_thread
// static unsigned char * gb_pixels_backup = NULL;
// //static byte * gb_pixels_backup = NULL; //(byte *)malloc(5760); //GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];
//#endif

//Joystick
//struct StructButtons {
// unsigned char b_select;
// unsigned char b_start;
// unsigned char b_right;
// unsigned char b_left;
// unsigned char b_up;
// unsigned char b_down;
// unsigned char b_a;
// unsigned char b_b;
//};

enum {
 MODE_GRAY = 0x00,
 MODE_FAST_RED  = 0x01 
};

static byte gb_mode_visual=MODE_GRAY;

//JJint sdl_update(void);
//void sdl_init(void);
void SDL_AssignPtrPixels(unsigned char * auxPtr);
//#ifdef use_lib_vga_thread
// void SDL_AssignBackupPtrPixels(unsigned char * auxPtr);
//#endif
unsigned char * SDL_GetPointerPixels();
//void SDLAssignLookup160lines(unsigned short int *ptr); //No se usa fabgl
//void SDL_FlipLineRedFastFabgl(int aLine);
//void SDL_FlipLineFastFabgl(int aLine);
unsigned char SDLGetModeVisual();
void SetModeVisual(byte auxMode);
//void SDL_AssignVGA(uint8_t **aux_scanline_p);
//void SDL_InitBuffers(void);
void sdl_frame(void);
//void sdl_quit(void);
byte* sdl_get_framebuffer(void);
byte sdl_get_buttons(void);
byte sdl_get_directions(void);
//void SDL_Flip_fast();
//void SDL_Flip();
//int jj_sdl_update(fabgl::PS2Controller auxPS2Controller);
//void jj_sdl_joystick(StructButtons * auxButtons);
//byte jj_getColorIndexFromFrameBuffer(byte x, byte y);
#endif
