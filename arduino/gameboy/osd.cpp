#include "gbConfig.h"
#include "def/hardware.h"
#include "osd.h"
#include "dataFlash/gbrom.h"
#include "gb_globals.h"
#include "Emulator/Keyboard/PS2Kbd.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"


//#define BLACK   0
//#define BLUE    4
//#define RED     1
//#define MAGENTA 5
//#define GREEN   2
//#define CYAN    6
//#define YELLOW  3
//#define WHITE   15

/*
 #ifdef COLOR_3B           //       BGR 
  #define BLACK   0x08      // 0000 1000
  #define BLUE    0x0C      // 0000 1100
  #define RED     0x09      // 0000 1001
  #define MAGENTA 0x0D      // 0000 1101
  #define GREEN   0x0A      // 0000 1010
  #define CYAN    0x0E      // 0000 1110
  #define YELLOW  0x0B      // 0000 1011
  #define WHITE   0x0F      // 0000 1111
 #else
  #define BLACK       0xC0      // 1100 0000
  #define BLUE        0xE0      // 1110 0000
  #define RED         0xC2      // 1100 0010
  #define MAGENTA     0xE2      // 1110 0010
  #define GREEN       0xC8      // 1100 1000
  #define CYAN        0xE8      // 1110 1000
  #define YELLOW      0xCA      // 1100 1010
  #define WHITE       0xEA      // 1110 1010
 #endif
*/


//extern int gb_screen_xIni;
//extern int gb_screen_yIni;
//extern unsigned char gb_cache_zxcolor[8];


unsigned char gb_show_osd_main_menu=0;

//extern SDL_Surface * gb_screen;
//extern SDL_Event gb_event;


#define max_gb_osd_screen_values 5
const char * gb_osd_screen_values[max_gb_osd_screen_values]={
 "0",
 "2",
 "4", 
 "8", 
 "16"
};


#define max_gb_main_menu 5
const char * gb_main_menu[max_gb_main_menu]={
 "Load ROM",
 "Speed",
 "Sound",
 "Reset",
 "Return"
};


#define max_gb_speed_sound_menu 7
const char * gb_speed_sound_menu[max_gb_speed_sound_menu]={
 "0",
 "1",
 "2",
 "4",
 "8",
 "16",
 "32"
};

#define max_gb_value_binary_menu 2
const char * gb_value_binary_menu[max_gb_value_binary_menu]={
 "0",
 "1"
};


#define max_gb_speed_videoaudio_options_menu 2
const char * gb_speed_videoaudio_options_menu[max_gb_speed_videoaudio_options_menu]={
 "Keyboard poll",
 "CPU"
};

#define max_gb_osd_sound_menu 2
const char * gb_osd_sound_menu[max_gb_osd_sound_menu]={
 "ON",
 "OFF"
};


#define max_gb_speed_cpu_menu 3
const char * gb_speed_cpu_menu[max_gb_speed_cpu_menu]={
 "Normal",
 "2x",
 "3x"
};


#define max_gb_reset_menu 2
const char * gb_reset_menu[max_gb_reset_menu]={
 "Soft",
 "Hard"
};



#define gb_pos_x_menu 36
#define gb_pos_y_menu 20
#define gb_osd_max_rows 10

void OSDMenuRowsDisplayScroll(const char **ptrValue,unsigned char currentId,unsigned char aMax)
{//Dibuja varias lineas
 vga.setTextColor(WHITE,BLACK);
 for (int i=0;i<gb_osd_max_rows;i++)
 {
  vga.setCursor(gb_pos_x_menu, gb_pos_y_menu+8+(i<<3));
  vga.print("                    ");
 }
 
 for (int i=0;i<gb_osd_max_rows;i++)
 {
  if (currentId >= aMax)
   break;
  if (i == 0)
   vga.setTextColor(CYAN,BLUE);
  else
   vga.setTextColor(WHITE,BLACK);
  vga.setCursor(gb_pos_x_menu, gb_pos_y_menu+8+(i<<3));
  vga.print(ptrValue[currentId]);
  currentId++;
 }     
}

//Maximo 256 elementos
unsigned char ShowTinyMenu(const char *cadTitle,const char **ptrValue,unsigned char aMax)
{ 
 unsigned char aReturn=0;
 unsigned char salir=0;
 vga.fillRect(0,0,160,144,0);
 vga.fillRect(0,0,160,144,0);//Repeat Fix visual defect

 vga.setTextColor(WHITE,BLACK);
 vga.setCursor((gb_pos_x_menu-(20)), gb_pos_y_menu-16);
 vga.print("Gameboy by Ackerman");

 vga.setTextColor(BLACK,WHITE);
 for (int i=0;i<12;i++)
 {  
  vga.setCursor((gb_pos_x_menu+(i*6)), gb_pos_y_menu);
  vga.print(" ");
 }
  
 vga.setCursor(gb_pos_x_menu,gb_pos_y_menu);
 vga.print(cadTitle);
  
 OSDMenuRowsDisplayScroll(ptrValue,0,aMax);
 
 while (salir == 0)
 {
  //SDL_PollEvent(gb_osd_sdl_event);
  //if(SDL_WaitEvent(gb_osd_sdl_event))
  {
   //if(gb_osd_sdl_event->type == SDL_KEYDOWN)
   {
    //switch(gb_osd_sdl_event->key.keysym.sym)
    {
     //case SDLK_UP:
     if (checkAndCleanKey(KEY_CURSOR_UP))
     {
      if (aReturn>0) aReturn--;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
     }
     if (checkAndCleanKey(KEY_CURSOR_DOWN))
     {
      if (aReturn < (aMax-1)) aReturn++;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
     }
     if (checkAndCleanKey(KEY_ENTER))
     {
      salir= 1;
     }
     //case SDLK_KP_ENTER: case SDLK_RETURN: salir= 1;break;
     if (checkAndCleanKey(KEY_ESC))
     {
      salir=1; aReturn= 255;    
     }
     //case SDLK_ESCAPE: salir=1; aReturn= 255; break;
     //default: break;
    }
    //SDL_Flip(gb_osd_sdl_surface);
    //SDL_PollEvent(gb_osd_sdl_event);
   }  
  }  
 } 
 gb_show_osd_main_menu= 0;
 return aReturn;
}

//Menu Load ROM
void ShowTinyLoadROM()
{
 unsigned char aSelNum;     
 aSelNum = ShowTinyMenu("Load ROM",gb_list_rom_title,max_list_rom);
 if (aSelNum == 255)
 {
  vTaskDelay(2);
  return;
 }
 #if defined(use_max_ram) || defined(use_half_ram)
  MemBankSwitchClear();
 #endif
 gb_lcd_line = 0;
 rom_init(gb_list_rom_data[aSelNum]); //First rom
 lcdAssignSDLFrameBuffer();
 MemAssignROMPtrMemory();
 mem_init();
 gb_ptrMem_raw = MemGetAddressMem(); //No hace falta
 //printf("Mem OK!\n");
 cpu_init(); 
 vTaskDelay(2); 
}


//Menu resetear
void ShowTinyResetMenu()
{
 unsigned char aSelNum;
 aSelNum= ShowTinyMenu("Reset",gb_reset_menu,max_gb_reset_menu); 
 if (aSelNum == 1)
  ESP.restart();
 cpu_init();
 vTaskDelay(2);
}


//Menu velocidad emulador
void ShowTinySpeedMenu()
{
 unsigned char aSelNum,aSelNumSpeed;
 aSelNum = ShowTinyMenu("SPEED OPTIONS",gb_speed_videoaudio_options_menu,max_gb_speed_videoaudio_options_menu);
 if (aSelNum == 255)
  return;
 switch (aSelNum)
 {
  case 0: aSelNumSpeed= ShowTinyMenu("Keyboard Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
   gb_current_ms_poll_joy = (aSelNumSpeed<<1);   
   break;
  case 1: aSelNumSpeed= ShowTinyMenu("CPU SPEED",gb_speed_cpu_menu,max_gb_speed_cpu_menu);   
   if (aSelNumSpeed == 255)
    return;
   switch (aSelNumSpeed)
   {
    case 0: LCDSetSpeed(SPEED_NORMAL); break;
    case 1: LCDSetSpeed(SPEED_TURBO); break;
    case 2: LCDSetSpeed(SPEED_MEGATURBO); break;
    default: break;
   }   
   //gb_current_ms_poll_keyboard= (aSelNumSpeed<<1);
   break;   
  default: break;
 } 
}




//Ajustar pantalla
void ShowTinySoundMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("Sound",gb_osd_sound_menu,max_gb_osd_sound_menu);
 if (aSelNum == 255)
  return;
 //gb_silence_all_channels = (aSelNum == 0)?1:0;
 #ifdef use_lib_fabgl_sound
  gb_sound_off = (aSelNum == 0)?0:1;
 #endif 

  
/* unsigned char aSelNum, auxCol; 
 aSelNum= ShowTinyMenu("Screen Adjust",gb_osd_screen,max_gb_osd_screen);
 auxCol= ShowTinyMenu("Pixels",gb_osd_screen_values,max_gb_osd_screen_values);
 auxCol = auxCol<<1; //x2
 gb_screen_xOffset = auxCol; 
 /*switch (aSelNum)
 {
  case 2:
   //OSD_PreparaPaleta8Colores();
   OSD_PreparaPaleta64para8Colores();
   memcpy(gb_cache_zxcolor,gb_const_colornormal8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_colornormal8_bright,8);
   #endif
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 3:
   OSD_PreparaPaleta64para8Colores();
   memcpy(gb_cache_zxcolor,gb_const_monoBlue8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoBlue8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   //for (int k=0;k<256;k++)
   // printf("%d %d %d\n",gb_osd_sdl_palette[k].r,gb_osd_sdl_palette[k].g,gb_osd_sdl_palette[k].b);
   //for (int k=0;k<8;k++)
   // printf("%d ",gb_cache_zxcolor[k]);
   //printf("\n");    
   return;
   break;
  case 4:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_monoGreen8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoGreen8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 5:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_monoRed8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_monoRed8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
  case 6:
   OSD_PreparaPaleta64para8Colores();   
   memcpy(gb_cache_zxcolor,gb_const_grey8,8);
   #ifdef use_lib_use_bright
    memcpy(gb_cache_zxcolor_bright,gb_const_grey8_bright,8);
   #endif 
   SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;          
 }
 auxCol= ShowTinyMenu("Cols or pixels",gb_osd_screen_values,max_gb_osd_screen_values);
 switch (aSelNum)
 {
  case 0: gb_screen_xIni = -(auxCol<<3); break;
  case 1: gb_screen_yIni = (auxCol<=3)? (-auxCol):(-3); break;  
  default: break;
 }
 */
}


//*******************************************
void SDLActivarOSDMainMenu()
{     
 gb_show_osd_main_menu= 1;   
}

//Very small tiny osd
void do_tinyOSD() 
{
 unsigned char aSelNum;
 if (checkAndCleanKey(KEY_F1))
 {
  gb_show_osd_main_menu= 1;
  #ifdef use_lib_fabgl_sound  
   gb_silence_all_channels = 1;   
  #endif    
  return;
 }

 if (gb_show_osd_main_menu == 1)
 {
  aSelNum = ShowTinyMenu("MAIN MENU",gb_main_menu,max_gb_main_menu);
  switch (aSelNum)
  {   
   case 0: ShowTinyLoadROM(); break;
   case 1: ShowTinySpeedMenu(); break;
   case 2: ShowTinySoundMenu(); break;   
   case 3: ShowTinyResetMenu(); break;
   default: break;
  }
 }
 #ifdef use_lib_fabgl_sound
  gb_silence_all_channels = 0;
 #endif 
}

