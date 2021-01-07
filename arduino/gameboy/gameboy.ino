//********************************************************
//* Author: ackerman                                     * 
//* Mod port lualiliu                                    *
//* https://github.com/lualiliu/esp32-gameboy            *
//* Arduino IDE 1.8.11                                   *
//* TTGO VGA32 ESP32                                     *
//* https://dl.espressif.com/dl/package_esp32_index.json *
//* ESP32 espressif Systems 1.0.4                        *
//* Libreria fabgl 0.9.0                                 *
//* Libreria Bitluini ESP32lib 0.3.3                     *
//* Visual Studio 1.48.1                                 *
//* PlatformIO IDE 2.2.1                                 *
//*                                                      *
//* Modificar gbConfig.h                                 *
//* Teclas:                                              *
//*  Arriba, abajo, izquierda, derecha                   *
//*  a - A                                               *
//*  z - B                                               *
//*  Barra espaciadora - Start                           *
//*  ENTER - SELECT                                      *
//********************************************************

#include "gbConfig.h"
#include "gb_globals.h"
#include <Arduino.h>
#include "dataFlash/gbrom.h"

#ifdef use_lib_vga_thread
 #include "driver/timer.h"
 #include "soc/timer_group_struct.h"
#endif

//#ifdef lib_compile_fabgl
// #include "fabgl.h" //Para fabgl
// #include "fabutils.h" //Para fabgl
// SoundGenerator soundGenerator;
// SineWaveformGenerator gb_sineArray[3];
// unsigned short int gbFrecMixer_before[3]={0,0,0}; 
//#else
 #ifdef use_lib_fabgl_sound
  #include "fabgl.h" //Para bitluni vga32 con sonido fabgl
  #include "fabutils.h"
  #include "WiFiGeneric.h"
  SoundGenerator soundGenerator;
  SineWaveformGenerator gb_sineArray[3];
  unsigned short int gbFrecMixer_before[3]={0,0,0};  
  unsigned char gb_silence_all_channels=1;
  unsigned char gb_sound_off=0;
 #endif
//#endif

#ifdef lib_compile_vga32
 //#include <ESP32Lib.h> //Para VGA32
 //#include <Ressources/Font6x8.h>//Para VGA32
 #include "MartianVGA.h" 
 #include "ESP32Lib/Ressources/Font6x8.h"//Para VGA32
 #include "def/hardware.h"
#endif
 
//#include <WiFi.h>
//#include <HTTPClient.h>
//#include <Preferences.h>
#include <stdio.h>
//#include <esp_bt.h>
#include "soc/rtc.h"
#include "sdl.h"
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "osd.h"

#ifdef lib_compile_vga32
 #include "PS2Kbd.h" //Para VGA32
 //enum {
 // GFX_MENU  = 0x00,
 // GFX_GBRUN = 0x01
 //};
 //static unsigned char  gb_state_loop=GFX_GBRUN; 
#endif 

//GAMEBOY_HEIGHT 144
//GAMEBOY_WIDTH 160


#ifdef use_lib_200x150
 //Para 200x150 maxk 0x3F bits 0
 #define gbvgaMask8Colores 0x3F
 #ifdef use_lib_vga_low_memory
  #define gbvgaBits8Colores 0xC0
 #else
  #define gbvgaBits8Colores 0
 #endif
#else
 #ifdef use_lib_320x175
  //Para 320x175
  #define gbvgaMask8Colores 0x3F
  #ifdef use_lib_vga_low_memory
   #define gbvgaBits8Colores 0xC0
  #else
   #define gbvgaBits8Colores 0x80
  #endif 
 #endif
#endif 

#ifdef use_lib_fast_vgaBLUE8colors
 static unsigned char gb_paletaBlue[4]={
  ((0 & gbvgaMask8Colores)|gbvgaBits8Colores),
  ((4 & gbvgaMask8Colores)|gbvgaBits8Colores),
  ((6 & gbvgaMask8Colores)|gbvgaBits8Colores),
  ((7 & gbvgaMask8Colores)|gbvgaBits8Colores)
 }; //azul 8 colores
#else
 #ifdef use_lib_fast_vgaRED8colors
  static unsigned char gb_paletaRed[4]={
   ((0 & gbvgaMask8Colores)|gbvgaBits8Colores),
   ((1 & gbvgaMask8Colores)|gbvgaBits8Colores),
   ((5 & gbvgaMask8Colores)|gbvgaBits8Colores),
   ((7 & gbvgaMask8Colores)|gbvgaBits8Colores)
  }; //rojo 8 colores
 #else
  #ifdef use_lib_fast_vga 
   #ifdef use_lib_vga8colors
    static unsigned char gb_paletaGrey8[4]={
     ((0x00 & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x01 & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x02 & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x03 & gbvgaMask8Colores)|gbvgaBits8Colores)
    }; //gris 8 colores      
   #else
    static unsigned char gb_paletaGrey64[4]={
     ((0x00 & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x15 & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x2A & gbvgaMask8Colores)|gbvgaBits8Colores),
     ((0x3F & gbvgaMask8Colores)|gbvgaBits8Colores)
    }; //gris 64 colores   
   #endif     
  #else
   static unsigned char gb_paletaGrey64dotFast[4]={
    0x00,0x15,0x2A,0x3F
   }; //gris 64 colores para vga.dotFast   
  #endif
 #endif
#endif

static unsigned char *gb_mem = NULL; //[0x10000]; //La memoria

#ifdef use_lib_log_serial
 unsigned int gb_free_ram_boot; //RAM libre en el arranque 
#endif 
unsigned long gb_time_cur;
unsigned long gb_time_prev_joy;

unsigned short int gb_lookup_160lines[144]; //Tablas para las 144 lineas

//#ifdef lib_compile_fabgl
// fabgl::VGAController DisplayController; //Seccion FABGL
// fabgl::PS2Controller PS2Controller; //Fin Seccion FABGL
//#endif

 #ifdef lib_compile_vga32
  unsigned char ** ptrVGA; //puntero al backbuffer
  #ifdef use_lib_vga8colors
   const int RED_PIN_3B = 22;
   const int GRE_PIN_3B = 19;
   const int BLU_PIN_3B = 5;
   #ifdef use_lib_vga_low_memory
    VGA3BitI vga;
   #else
    VGA3Bit vga;
   #endif
  #else
   const int redPin[] = {21,22}; //Seccion VGA32
   const int greenPin[] = {18,19};
   const int bluePin[] = {4,5}; 
   #ifdef use_lib_vga_low_memory
    VGA6BitI vga;
   #else
    VGA6Bit vga;  //Fin seccion VGA32
   #endif
  #endif 
 #endif


//StructButtons gb_buttons; //Botones del joystick
unsigned char button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;
static int gb_line_prev;
//int gb_line_cur,gb_line_prev;

unsigned char gb_current_ms_poll_joy = gb_ms_joy; //Joystick polling time

//#ifdef lib_compile_fabgl
// void Poll_JoystickFabgl(void);
// void ShowOptionsFabgl(void);
// void LoadLookupFabgl(void);
// void sound_cycleFabgl(void);
//  
// short int gb_frec_array_cur[3];
// uint8_t * gb_scanline_p[144]; //Puntero a scanlines fabgl
//#else
 #ifdef use_lib_fabgl_sound
  short int gb_frec_array_cur[3];
  void sound_cycleFabgl(void);
  void SilenceAllChannels(void);
 #endif
//#endif
#ifdef lib_compile_vga32
 inline void Poll_JoystickVGA32(void);
 //void LoadLookupVGA32(void);
 inline void FlipLineFastVGA32(int aLine);
 //void FlipLineFastRedVGA32(int aLine); 
 //void PollShowMenuVGA32(void);
 //void ShowOptionsVGA32(void); 
#endif

#if defined(use_max_ram)|| defined(use_half_ram)
 unsigned char* gb_bank_switch_array[maxBankSwitch]; //Cache de 4 y 8 bancos
#endif 

unsigned char* gb_ptrMem_raw=NULL; //La dire de Mem
unsigned char* gb_video_buffer_gb;//puntero a la pantalla video gameboy

#ifdef use_lib_vga_thread
 QueueHandle_t vidQueue;
 TaskHandle_t videoTaskHandle;
 volatile bool videoTaskIsRunning = false;
 uint16_t *param;
 unsigned char * gb_video_buffer_backup_gb= NULL;
#endif

//static unsigned char gb_current_frames=0;

//void Poll_runGB(void);
void InitGB(void);
void CargarJuego(unsigned char num);


//********************************
void LoadLookup160lines()
{
 for (unsigned char i=0;i<144;i++)
  gb_lookup_160lines[i]= i*160;
}

#ifdef lib_compile_vga32
 //********************************
inline void FlipLineFastVGA32(int aLine)
 {     
  unsigned char a;  
  //#ifdef use_lib_fast_vga   
  // //#define gbvgaMask8Colores 0x3F
  // //#define gbvgaBits8Colores 0x40   
  //#endif 
  //unsigned short int contOri=0;  
  //unsigned short int contDest=0;  
  if ((aLine<0)||(aLine>143))
   return;
 
  unsigned short int auxOffset= gb_lookup_160lines[aLine];
  //for (contOri=auxOffset; contOri<(auxOffset+160); contOri++,contDest++)
  //#ifdef use_lib_fast_vga 
  // ptrVGA = vga.backBuffer;
  //#endif
  //memcpy((void *)&ptrVGA[aLine][0],(void *)&gb_video_buffer_gb[auxOffset],160);
  for (int x=0;x<160;x++)
  {   
   #ifdef use_lib_fast_vgaBLUE8colors
    a = gb_video_buffer_gb[auxOffset++];
    #ifdef use_lib_vga_low_memory
     vga.dotFast(x,aLine,a);    
    #else
     ptrVGA[aLine][x^2] = gb_paletaBlue[a];
    #endif 
   #else
    #ifdef use_lib_fast_vgaRED8colors
     a = gb_video_buffer_gb[auxOffset++];
     #ifdef use_lib_vga_low_memory
      vga.dotFast(x,aLine,a);    
     #else     
      ptrVGA[aLine][x^2] = gb_paletaRed[a];
     #endif 
    #else
     #ifdef use_lib_fast_vga
      //#define gbvgaMask8Colores 0x3F
      //#define gbvgaBits8Colores 0x40     
      //a = (a & gbvgaMask8Colores)|gbvgaBits8Colores;
      //a = gb_video_buffer_gb[auxOffset++];
      //a = (a|(a<<2)|(a<<4));      
      //a = (a & gbvgaMask8Colores)|gbvgaBits8Colores;
      #ifdef use_lib_vga8colors       
       a = gb_video_buffer_gb[auxOffset++];
       #ifdef use_lib_vga_low_memory
        vga.dotFast(x,aLine,a);
       #else
        ptrVGA[aLine][x^2] = gb_paletaGrey8[a]; //((a & gbvgaMask8Colores)|gbvgaBits8Colores);
       #endif
      #else
       a = gb_video_buffer_gb[auxOffset++];
       #ifdef use_lib_vga_low_memory
        ptrVGA[aLine][x] = gb_paletaGrey64[a];
       #else
        ptrVGA[aLine][x^2] = gb_paletaGrey64[a];
       #endif 
      #endif 
      //ptrVGA[aLine][x^2] = paleta[a];    
     #else      
      //a = (a|(a<<2)|(a<<4));
      #ifdef use_lib_vga8colors
       vga.dotFast(x,aLine,gb_video_buffer_gb[auxOffset++]);
      #else
       a = gb_video_buffer_gb[auxOffset++];
       vga.dotFast(x,aLine,gb_paletaGrey64dotFast[a]);
      #endif
     #endif
    #endif
   #endif   
  }
 }

 //Modo rojo no lo necesito
 ////********************************
 //void FlipLineFastRedVGA32(int aLine)
 //{
 // //unsigned short int contOri=0;  
 // //unsigned short int contDest=0; 
 // //int a;
 // if ((aLine<0)||(aLine>143))
 //  return;
 //
 // unsigned short int auxOffset= gb_lookup_160lines[aLine];
 // //for (contOri=auxOffset; contOri<(auxOffset+160); contOri++,contDest++) 
 // for (unsigned char x=0;x<160;x++)
 // {   
 //  vga.dotFast(x,aLine,gb_video_buffer_gb[auxOffset++]);
 // }   
 //}
#endif

////********************************
//void Poll_runGB()
//{
//}

//*************************
void InitGB()
{
 //remapeo int r = rom_init(gb_rom);
 //int r = rom_init((unsigned char *)gb_rom_tetris);
 //int r = rom_init((unsigned char *)gb_rom_mario);
 //int r = rom_init((unsigned char *)gb_rom_batman);
 //int r = rom_init((unsigned char *)gb_rom_contra);
 //sdl_init(); No se necesita
 //printf("ROM OK!\n");     
 //int r = rom_init(gb_roms_data[0]);
 rom_init(gb_list_rom_data[0]);
 lcdAssignSDLFrameBuffer();
 MemAssignROMPtrMemory();
 mem_init();
 gb_ptrMem_raw = MemGetAddressMem();
 //printf("Mem OK!\n");
 cpu_init();
}

//*********
//* SETUP *
//*********
void setup() 
{ 
 #ifdef use_lib_log_serial
  Serial.begin(115200);
  gb_free_ram_boot = ESP.getFreeHeap();
 #endif 
 #ifdef use_lib_vga_thread
  gb_video_buffer_gb = (unsigned char *)malloc(23040); //pantalla video gameboy 
  gb_video_buffer_backup_gb = (unsigned char *)malloc(23040); //pantalla backup
  SDL_AssignPtrPixels(gb_video_buffer_gb);
  //SDL_AssignBackupPtrPixels(gb_video_buffer_backup_gb);
 #else 
  gb_video_buffer_gb = (unsigned char *)malloc(23040); //pantalla video gameboy 
  SDL_AssignPtrPixels(gb_video_buffer_gb);
 #endif
 

 gb_mem = (unsigned char *)malloc(0x10000);
 MemAssignPtrMem(gb_mem);

 LoadLookup160lines();
 LCDAssignLookup160lines(gb_lookup_160lines);
 //SDLAssignLookup160lines(gb_lookup_160lines); //No se usa fabgl

 #ifdef use_lib_log_serial
  Serial.printf("HEAP BEGIN %d\n", gb_free_ram_boot);  
  Serial.printf("HEAP malloc %d\n", ESP.getFreeHeap());
 #endif 

 #if defined(use_max_ram) || defined(use_half_ram)
  for (unsigned char i=0;i<maxBankSwitch;i++)
   gb_bank_switch_array[i] = (unsigned char *)malloc(0x4000);  
  MemPreparaBankSwitchPtr(gb_bank_switch_array);
 #endif 

 InitGB();
 #ifdef use_lib_log_serial
  Serial.printf("HEAP InitGB %d\n", ESP.getFreeHeap());
 #endif
 
 //#ifdef lib_compile_fabgl  
 // DisplayController.begin(); //Seccion fabgl
 // DisplayController.setResolution(VGA_320x200_75Hz);  
 // PS2Controller.begin(PS2Preset::KeyboardPort0_MousePort1, KbdMode::GenerateVirtualKeys);  
 // PS2Controller.keyboard()->setLayout(&fabgl::UKLayout); 
 // DisplayController.enableBackgroundPrimitiveExecution(false); //Quito background timer
 // DisplayController.enableBackgroundPrimitiveTimeout(false); 
 //#endif
   
  gb_time_cur= gb_time_prev_joy = millis();
  //#ifdef lib_compile_fabgl
  // LoadLookupFabgl(); //Seccion fabgl para scanline displaycontroller
  // SDL_AssignVGA(gb_scanline_p);   
  //#endif 

  #ifdef lib_compile_vga32
   //Ahorro RAM con modo video 200x150
   #ifdef use_lib_vga8colors
    #ifdef use_lib_200x150
     vga.init(vga.MODE200x150.custom(160,144), RED_PIN_3B, GRE_PIN_3B, BLU_PIN_3B, hsyncPin, vsyncPin); //Modo 8 colores              
    #else     
     vga.init(vga.MODE320x175.custom(160,144), RED_PIN_3B, GRE_PIN_3B, BLU_PIN_3B, hsyncPin, vsyncPin); //Se ve bien
    #endif 
   #else
    #ifdef use_lib_200x150     
     vga.init(vga.MODE200x150.custom(160,144), redPin, greenPin, bluePin, hsyncPin, vsyncPin); //Modo 64 colores               
    #else
     vga.init(vga.MODE320x175.custom(160,144), redPin, greenPin, bluePin, hsyncPin, vsyncPin); //Se ve bien
    #endif 
   #endif
   vga.setFont(Font6x8);
   vga.clear(BLACK);
   ptrVGA = vga.backBuffer; //Puntero al buffer Video
   vga.clear(0); //Seccion VGA32
   vTaskDelay(2);
   kb_begin();  
  #endif 

  #ifdef use_lib_log_serial
   Serial.printf ("Memoria %d %d %d %d\n",ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());  
   Serial.printf("Free memory (total, min, largest): %d, %d, %d\n", heap_caps_get_free_size(0), heap_caps_get_minimum_free_size(0), heap_caps_get_largest_free_block(0));
  #endif 
  
  #ifdef use_lib_log_serial   
   Serial.printf("HEAP END %d\n", ESP.getFreeHeap());
  #endif 

  #ifdef use_lib_vga_thread
   vidQueue = xQueueCreate(1, sizeof(uint16_t *));
   xTaskCreatePinnedToCore(&videoTask, "videoTask", 1024 * 4, NULL, 5, &videoTaskHandle, 0);
  #endif 

  // adjust this to center screen in your monitor
  //DisplayController.moveScreen(20, -2);
 //#ifdef lib_compile_fabgl
 // ShowOptionsFabgl();
 // for (unsigned char i=0;i<3;i++)
 // {
 //  soundGenerator.attach(&gb_sineArray[i]);
 //  gb_sineArray[i].enable(true);
 //  gb_sineArray[i].setFrequency(0);
 //  gb_sineArray[i].setVolume(0);
 // }
 // soundGenerator.play(true);
 //#else
  #ifdef use_lib_fabgl_sound
   for (unsigned char i=0;i<3;i++)
   {
    soundGenerator.attach(&gb_sineArray[i]);
    gb_sineArray[i].enable(true);
    gb_sineArray[i].setFrequency(0);
    gb_sineArray[i].setVolume(0);
   }
   soundGenerator.play(true);  
  #endif
 //#endif 
 //gb_line_cur=gb_line_prev=0;
 gb_line_prev=0;
 #ifdef use_lib_log_serial
  #ifndef use_lib_vga8colors
   Serial.printf("RGBMask:%d Sbits:%d\n",vga.RGBAXMask,vga.SBits);
  #endif 
 #endif
} //Fin Setup

//#if defined(lib_compile_fabgl) || defined(use_lib_fabgl_sound)
#ifdef use_lib_fabgl_sound
 void SilenceAllChannels()
 {  
  for (unsigned char i=0;i<3;i++)  
  {
   gb_sineArray[i].setFrequency(0);
   gb_sineArray[i].setVolume(0);   
   gbFrecMixer_before[i] = gb_frec_array_cur[i] = 0;
  }  
 }

 void sound_cycleFabgl()
 {  
  gb_frec_array_cur[0]= ((gb_ptrMem_raw[0xFF14]&0x07)<<8)|gb_ptrMem_raw[0xFF13];
  gb_frec_array_cur[0]= 131072/(2048 - gb_frec_array_cur[0]);
  
  gb_frec_array_cur[1]= ((gb_ptrMem_raw[0xFF19]&0x07)<<8)|gb_ptrMem_raw[0xFF18]; 
  gb_frec_array_cur[1]= 131072/(2048 - gb_frec_array_cur[1]);
  
  gb_frec_array_cur[2]= ((gb_ptrMem_raw[0xFF1E]&0x07)<<8)|gb_ptrMem_raw[0xFF1D]; 
  gb_frec_array_cur[2]= 65536/(2048 - gb_frec_array_cur[2]);

  //if (gb_silence==1)  
  // gb_frec_ch01_cur= gb_frec_ch02_cur= gb_frec_ch03_cur = 0;  

  //for (unsigned char i=0;i<3;i++)
  // gb_sineArray[i].setFrequency(gb_frec_array_cur[i]);

  for (unsigned char i=0;i<3;i++)
  {   
   if (gb_frec_array_cur[i] != gbFrecMixer_before[i])
   {            
    if (gb_frec_array_cur >0)
    {
     gb_sineArray[i].setFrequency(gb_frec_array_cur[i]);
     gb_sineArray[i].setVolume(63);
    }
    else
     gb_sineArray[i].setVolume(0);
    gbFrecMixer_before[i] = gb_frec_array_cur[i];
   }
  }
 }
#endif 

//#ifdef lib_compile_fabgl
// //***********************
// void LoadLookupFabgl()
// {
//  for (byte j=0;j<144;j++)
//   gb_scanline_p[j]=DisplayController.getScanline(j);    
// }

// //***********************
// void ShowOptionsFabgl()
// {
//  int x=170;
//  int y=0;
//  Canvas cv(&DisplayController);
//  cv.selectFont(&fabgl::FONT_8x8);
//  cv.clear();
//  cv.drawText(x, y,   "F1 RESET");
//  cv.drawText(x, y+=8,"F2 REBOOT");
//  cv.drawText(x, y+=8,"F3 RED FAST");
//  cv.drawText(x, y+=8,"F4 GRAY");
//  cv.drawText(x, y+=8,"F5 SILENCE");
//  cv.drawText(x, y+=8,"F6 SOUND");
//  cv.drawText(x, y+=8,"-  NORMAL SPEED");
//  cv.drawText(x, y+=8,"+  TURBO SPEED");
//  cv.drawText(x, y+=8,"*  MEGA SPEED");
//  for (unsigned char i=0;i<max_rom_gb;i++)  
//   cv.drawText(x, y+=8,gb_roms_title[i]);
// // cv.drawText(x, y+=8,"2  Contra");  
// // cv.drawText(x, y+=8,"0  MARIO");
// // cv.drawText(x, y+=8,"1  TETRIS");
// // cv.drawText(x, y+=8,"2  ASTEROIDS");
// // cv.drawText(x, y+=8,"3  ASTRO RABBY");
// // cv.drawText(x, y+=8,"4  PACMAN");
// // cv.drawText(x, y+=8,"5  SPIDERMAN");
// // cv.drawText(x, y+=8,"6  LOOPZ");
// // cv.drawText(x, y+=8,"7  BotBInvite");
// // cv.drawText(x, y+=8,"8  wobbly");
// // cv.drawText(x, y+=8,"9  naavis");
// // cv.drawText(x, y+=8,"0  Batman");
// // cv.drawText(x, y+=8,"1  Lion King");
// // cv.drawText(x, y+=8,"2  Contra");
// // cv.drawText(x, y+=8,"3  Bomberman");
// // cv.drawText(x, y+=8,"4  Donkey Kong");
// // cv.drawText(x, y+=8,"5  Hoshi");
// // cv.drawText(x, y+=8,"6  Metroid");
// // cv.drawText(x, y+=8,"7  Mortal 3");
//  //cv.drawText(x, y+=8,"8  Prince");
//  //cv.drawText(x, y+=8,"9  Wario"); 
//  cv.waitCompletion();  
// }
//#endif

#ifdef lib_compile_vga32
 //************************************************
 //void ShowOptionsVGA32()
 //{
 // int x=10;
 // int y=10;
 // vga.clear(0);
 // vga.setFont(Font6x8);
 // vga.setCursor(x, y);
 // vga.print("F1 RESET"); y+=8; 
 // vga.setCursor(x, y);
 // vga.print("F2 REBOOT"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("F3 RED FAST"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("F4 GRAY"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("F5 SILENCE"); y+=8;
 // vga.setCursor(x, y);  
 // vga.print("F6 SILENCE"); y+=8;
 // vga.setCursor(x, y);    
 // vga.print("-  NORMAL SPEED"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("+  TURBO SPEED"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("*  MEGA SPEED"); y+=8;
 // vga.setCursor(x, y); 
 // Poll_JoystickVGA32();
 // /*vga.print("0  Batman"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("1  Lion King"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("2  Contra"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("3  Bomberman"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("4  Donkey Kong"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("5  Hoshi"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("6  Metroid"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("7  Mortal 3"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("8  Prince"); y+=8;
 // vga.setCursor(x, y);
 // vga.print("9  Wario"); y+=8; */
 //}

 //************************************************
 inline void Poll_JoystickVGA32()
 {
  button_select = checkKey(KEY_ENTER);
  button_start = checkKey(KEY_SPACE);
  button_right = checkKey(KEY_CURSOR_RIGHT);
  button_left = checkKey(KEY_CURSOR_LEFT);
  button_up = checkKey(KEY_CURSOR_UP);
  button_down = checkKey(KEY_CURSOR_DOWN);
  button_a = checkKey(KEY_A);
  button_b = checkKey(KEY_Z);
  //jj_sdl_joystick(&gb_buttons);
  
//   if (checkKey(KEY_F3))
//    SetModeVisual(MODE_FAST_RED); //ROJO rapido
//   if (checkKey(KEY_F4))
//    SetModeVisual(MODE_GRAY);    //Gris
//   if (checkKey(KEY_F5))
//   {  
//    gb_silence=1; //Silencio   
//    #ifdef use_lib_fabgl_sound
//     soundGenerator.play(false);
//    #endif 
//   }
//   if (checkKey(KEY_F6))
//   {    
//    gb_silence=0; //Sonido
//    #ifdef use_lib_fabgl_sound
//     soundGenerator.play(true);
//    #endif 
//   }
//   if (checkKey(KEY_F2))
//    ESP.restart(); //Reinicio hard
//   if (checkKey(KEY_F1))
//    cpu_init(); //Reinicio soft
//   //if (checkKey(KEY_F12))
//   // gb_state_loop = GFX_MENU;
//   if (checkKey(KEY_PLUS))
//    LCDSetSpeed(SPEED_TURBO); //Velocidad Turbo          
//   if (checkKey(KEY_ASTERISK))   
//    LCDSetSpeed(SPEED_MEGATURBO); //Velocidad Mega Turbo     
//   if (checkKey(KEY_MINUS))
//    LCDSetSpeed(SPEED_NORMAL); //Velocidad Normal        
//   
//   if (checkKey(KEY_NUM_0))
//    CargarJuego(0);   
//   if (checkKey(KEY_NUM_1))
//    CargarJuego(1);
//   if (checkKey(KEY_NUM_2))
//    CargarJuego(2);
//   if (checkKey(KEY_NUM_3))
//    CargarJuego(3);
//   if (checkKey(KEY_NUM_4))
//    CargarJuego(4);
//   if (checkKey(KEY_NUM_5))
//    CargarJuego(5);
//   if (checkKey(KEY_NUM_6))
//    CargarJuego(6);
//   if (checkKey(KEY_NUM_7))
//    CargarJuego(7);
//   if (checkKey(KEY_NUM_8))
//    CargarJuego(8);
//   if (checkKey(KEY_NUM_9))
//    CargarJuego(9);  
//  
 }
#endif 

//#ifdef lib_compile_fabgl
// //************************************************
// void Poll_JoystickFabgl()
// {     
//  //unsigned long time_prev;
//  //time_prev = micros();   
//  auto keyboard = PS2Controller.keyboard();
//  if (keyboard->isKeyboardAvailable())
//  {
//   gb_buttons.b_select = keyboard->isVKDown(fabgl::VK_KP_ENTER);    
//   gb_buttons.b_start = keyboard->isVKDown(fabgl::VK_SPACE);
//   gb_buttons.b_right = keyboard->isVKDown(fabgl::VK_RIGHT);
//   gb_buttons.b_left = keyboard->isVKDown(fabgl::VK_LEFT);
//   gb_buttons.b_up = keyboard->isVKDown(fabgl::VK_UP);
//   gb_buttons.b_down = keyboard->isVKDown(fabgl::VK_DOWN);
//   gb_buttons.b_a = keyboard->isVKDown(fabgl::VK_a);
//   gb_buttons.b_b = keyboard->isVKDown(fabgl::VK_z);   
////   if (keyboard->isVKDown(fabgl::VK_ESCAPE) || keyboard->isVKDown(fabgl::VK_F12))
////    gb_state_loop = GFX_MENU;
//
//   if (keyboard->isVKDown(fabgl::VK_F3))
//    SetModeVisual(MODE_FAST_RED); //ROJO rapido
//   if (keyboard->isVKDown(fabgl::VK_F4))
//    SetModeVisual(MODE_GRAY);    //Gris
//   if (keyboard->isVKDown(fabgl::VK_F5))
//   {
//    soundGenerator.play(false);
//    gb_silence= 1; //Silencio
//   }
//   if (keyboard->isVKDown(fabgl::VK_F6))
//   {
//    soundGenerator.play(true);
//    gb_silence= 0; //Sonido    
//   }
//   if (keyboard->isVKDown(fabgl::VK_F2))
//    ESP.restart(); //Reinicio hard
//   if (keyboard->isVKDown(fabgl::VK_F1))
//    cpu_init(); //Reinicio soft
//   if (keyboard->isVKDown(fabgl::VK_PLUS) || keyboard->isVKDown(fabgl::VK_KP_PLUS))   
//    LCDSetSpeed(SPEED_TURBO); //Velocidad Turbo
//   if (keyboard->isVKDown(fabgl::VK_ASTERISK ) || keyboard->isVKDown(fabgl::VK_KP_MULTIPLY))       
//    LCDSetSpeed(SPEED_MEGATURBO);//Velocidad Mega Turbo      
//   if (keyboard->isVKDown(fabgl::VK_MINUS) || keyboard->isVKDown(fabgl::VK_KP_MINUS))   
//    LCDSetSpeed(SPEED_NORMAL); //Velocidad Normal    
//           
//   if (keyboard->isVKDown(fabgl::VK_0))
//    CargarJuego(0);   
//   if (keyboard->isVKDown(fabgl::VK_1))   
//    CargarJuego(1);
//   if (keyboard->isVKDown(fabgl::VK_2))
//    CargarJuego(2);
//   if (keyboard->isVKDown(fabgl::VK_3))   
//    CargarJuego(3);
//   if (keyboard->isVKDown(fabgl::VK_4))
//    CargarJuego(4);
//   if (keyboard->isVKDown(fabgl::VK_5))
//    CargarJuego(5);
//   if (keyboard->isVKDown(fabgl::VK_6))
//    CargarJuego(6);
//   if (keyboard->isVKDown(fabgl::VK_7))
//    CargarJuego(7);
//   if (keyboard->isVKDown(fabgl::VK_8))
//    CargarJuego(8);
//   if (keyboard->isVKDown(fabgl::VK_9))
//    CargarJuego(9);
//    
//   //if (keyboard->isVKDown(fabgl::VK_l))
//   //{//Cargar rom
//   // //DownloadROM("http://192.168.0.36/mario.gb","mario.gb");
//   //}
//   //b_start, b_select, b_a, b_b, b_down, b_up, b_left, b_right
//  }
//
//  jj_sdl_joystick(&gb_buttons);
//  //printf("start %d right %d\n",button_start,button_right);
//  //time_prev = micros()-time_prev;
//  //printf("Teclado %d\n",time_prev);     
// }
//#endif

//****************************************
void CargarJuego(unsigned char num)
{
 if (gb_list_rom_data[num] != NULL)
 {  
  rom_init(gb_list_rom_data[num]);  
  //lcdAssignSDLFrameBuffer();  
  MemAssignROMPtrMemory();  
  mem_init();  
  //gb_ptrMem_raw = MemGetAddressMem();  
  cpu_init();
  vTaskDelay(2);
 }
}

//#ifdef lib_compile_vga32
// //****************************************
// void PollShowMenuVGA32()
// {//Muestra Menu
//  //gb_state_loop = GFX_GBRUN;
//  ShowOptionsVGA32();  
//  
//  //auto menu = new Menu;
//  //menu->run(&DisplayController);
// }
//#endif 

#ifdef use_lib_vga_thread
//Video con hilos va lento
void videoTask(void *unused)
{
 videoTaskIsRunning = true;
 uint16_t *param;
 unsigned short int auxOffset;
 unsigned char a;
 while (1) 
 {
  xQueuePeek(vidQueue, &param, portMAX_DELAY);
  if ((int)param == 1)
   break;
  //a= millis(); 
  if (
      (gb_lcd_line != 144)
      ||
      (gb_lcd_current_frame & 1)
     )
  {
   videoTaskIsRunning = false;
  }
  else
  { 
   videoTaskIsRunning = true;

  for (int y = 0; y < 144; y++)
  {
   auxOffset= gb_lookup_160lines[y];
   for (int x=0; x<160;x++)
   {
    a = gb_video_buffer_backup_gb[auxOffset++];
    #ifdef use_lib_fast_vgaBLUE8colors
     ptrVGA[y][x^2] = gb_paletaBlue[a];
    #else
     #ifdef use_lib_fast_vgaRED8colors
      ptrVGA[y][x^2] = gb_paletaRed[a];
     #else
      #ifdef use_lib_fast_vga
       #ifdef use_lib_vga8colors
        ptrVGA[y][x^2] = gb_paletaGrey8[a];
       #else
        ptrVGA[y][x^2] = gb_paletaGrey64[a];
       #endif
      #else
       #ifdef use_lib_vga8colors
        vga.dotFast(x,y,a);
       #else        
        vga.dotFast(x,y,gb_paletaGrey64dotFast[a]);
       #endif       
      #endif 
     #endif
    #endif 
     

    //a = gb_video_buffer_backup_gb[auxOffset++];
    //a = gb_video_buffer_backup_gb[auxOffset++];
    //a = gb_video_buffer_backup_gb[(y*160)+x];        
    //vga.dotFast(x,y,a);
    //a = gb_video_buffer_backup_gb[(y*160)+x];
    //a = (a|(a<<2)|(a<<4));
    //auxOffset++;
    //vga.dotFast(x,y,a);
   }
  }
  }
   
  xQueueReceive(vidQueue, &param, portMAX_DELAY);
  videoTaskIsRunning = false;   
 }
 videoTaskIsRunning = false;
 vTaskDelete(NULL);
 while (1){
 }     
}//fin videoTask
#endif


//******************
//* MAIN PRINCIPAL *
//******************
void loop() 
{ 
 //switch (gb_state_loop)
 //{
 // case GFX_MENU: PollShowMenuVGA32(); break;
 // case GFX_GBRUN: Poll_runGB(); break;
 // default: break;
 //} 

 gb_time_cur= millis();
 if ((gb_time_cur - gb_time_prev_joy) >= gb_current_ms_poll_joy)
 {
  gb_time_prev_joy = gb_time_cur;  
  //#ifdef lib_compile_fabgl
  // Poll_JoystickFabgl(); //fabgl
  //#endif
  #ifdef lib_compile_vga32
   Poll_JoystickVGA32();
   do_tinyOSD();
  #endif 
 }
 // put your main code here, to run repeatedly:    
 cpu_cycle();
 //CPUIncCycle (1); 
 lcd_cycle_fast(); //lcd_cycle(); 
 timer_cycle();

 //gb_line_cur= lcd_get_line(); 
 //if (gb_line_cur != gb_line_prev)
 if (gb_lcd_line != gb_line_prev)
 {  
  gb_line_prev = gb_lcd_line;
  //gb_line_prev = gb_line_cur;

   
 #ifdef gb_frame_crt_skip
  if (gb_lcd_current_frame & 1)
 #endif 
  {
   #ifndef use_lib_vga_thread
    FlipLineFastVGA32(gb_lcd_line);
   #endif
   /*switch (SDLGetModeVisual())
   { 
    #ifdef lib_compile_fabgl
     case MODE_GRAY: SDL_FlipLineFastFabgl(gb_line_cur); break;
     case MODE_FAST_RED: SDL_FlipLineRedFastFabgl(gb_line_cur); break;    
    #endif
    #ifdef lib_compile_vga32
     #ifndef use_lib_vga_thread
      case MODE_GRAY: FlipLineFastVGA32(gb_line_cur); break;    
      case MODE_FAST_RED: FlipLineFastRedVGA32(gb_line_cur); break;    
     #endif
    #endif 
    default: break;
   }*/
  }

  //if (gb_line_cur==144)
  // gb_current_frames++;
   
  //#if defined(lib_compile_fabgl) || defined (use_lib_fabgl_sound)
  #ifdef use_lib_fabgl_sound   
   if ((gb_silence_all_channels == 1)||(gb_sound_off == 1))
   {
    SilenceAllChannels();
   }
   else
   { 
    if (gb_lcd_line == 144)
    {    
     sound_cycleFabgl();
    }     
   }
  #endif
 }

 //Poll_runGB();
 
 #ifdef use_lib_vga_thread
  xQueueSend(vidQueue, &param, portMAX_DELAY);
  while (videoTaskIsRunning)
  {
  }
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
  vTaskDelay(0); // important to avoid task watchdog timeouts - change this to slow down emu  
 #endif 
}









//Codigo no se usa
 //static uint8_t * gb_bufWIFI=NULL;
 //static unsigned char * gb_rom_wifi=NULL;
 //preferences.begin("GAMEBOY",false);
 //preferences.begin("VIC20", false);
 // Turn off peripherals to gain memory (?do they release properly)
 //esp_bt_controller_deinit(); //Consume mas RAM con estas opciones
 //esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);  
 //printf("Total PSRAM: %d\n", ESP.getPsramSize());
 //printf("Free PSRAM: %d\n", ESP.getFreePsram()); 
 //gb_bufWIFI = (uint8_t*) malloc(128); //Reservo para WIFI
 //gb_rom_wifi = (unsigned char*) malloc(0x4000); 

 //for (byte i=0;i<4;i++) 
 // gb_buf_vga_array[i] = (byte *)malloc(5760); 


//rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M); //Fuerzo 240 mhz placa TTGO VGA32
//VGA gb_buf_vga = (byte *)malloc(23040);
//VGA if (gb_buf_vga == NULL)
//VGA  printf("ERROR mem\n");  
//SDL_InitBuffers();

//gb_pointer_pixels_vga32 = SDL_GetPointerPixels();  
//unsigned char *gb_pointer_pixels_vga32 = NULL;//puntero a pixels 



 //Serial.begin(115200);    
  //DisplayController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_15);
//vga.init(vga.MODE200x150, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
//vga.setFont(Font6x8);
//vga.println("Hello World!");
  // put your setup code here, to run once:
  //DisplayController.setResolution(VGA_320x200_75Hz, -1, -1, DOUBLEBUFFERING);
  //DisplayController.setResolution(VGA_256x384_60Hz);
  //DisplayController.setResolution(VGA_320x200_75HzRetro);
  // Setup mouse
  //PS2Controller.begin();
  //PS2Controller.mouse()->setupAbsolutePositioner(canvas.getWidth(), canvas.getHeight(), true, &DisplayController);
  //DisplayController.setMouseCursor((CursorName)cursor);
  //canvas.setBrushColor(Color::Blue);

//  Canvas cv(&DisplayController);  
//  cv.setBrushColor(Color::Black);
//  cv.clear();
//  cv.selectFont(&fabgl::FONT_8x8);
//  cv.setGlyphOptions(GlyphOptions().FillBackground(true));
  //canvas.setBrushColor(Color::Black);
  //canvas.clear();
  //canvas.selectFont(&fabgl::FONT_8x8);
  //canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
  
  //showCursorPos(PS2Controller.mouse()->status());  
  //canvas.setPenColor(Color::White); //Creo el primer cuadrado
  //rect = Rect(1, 1, 160, 144);

   //ConectarWiFi();

 //volatile int gb_dutty_ch01=0;
 //volatile int gb_volumen01=0;
 //volatile int gb_duracion01=0;
//volatile int gb_dutty_ch02=0;
 //volatile int gb_volumen02=0;
 //volatile int gb_duracion02=0;
//volatile int gb_volumen03=0;   

   //uint8_t * p = gb_scanline_p[j];
   //long * pOrigen= (long *)(gb_buf2);  
   /*long * pDestino= (long *)(p);
   for (uint_fast8_t k=0;k<40;k++)
   {
    pDestino[k] = pOrigen[contDest++];
   } */  
   /*for (uint_fast8_t k=0;k<160;k+=4,contDest+=4)
   {    
    p[k+2] = gb_buf2[contDest];
    p[k+3] = gb_buf2[contDest+1];
    p[k] = gb_buf2[contDest+2];
    p[k+1] = gb_buf2[contDest+3];        
   }*/   

   //JJ_assign_canvas(canvas);     


     //canvas.clear();
  //for (uint16_t k=0;k<(160*144);k++)
  // gb_buf2[k]=k;



// void JJ_Flip(byte *screen){  
//  int i,j;
//  for(i = 0;i<GAMEBOY_WIDTH;i++){
//    for(j = 0;j<GAMEBOY_HEIGHT;j++){
//        //JJ video update tft.drawPixel(j, i, color[getColorIndexFromFrameBuffer(j, i)]);
//      }
//    }
//    //memset(pixels,0,GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4*sizeof(byte));
//}


//const int redPin[] = {21,22};
//const int greenPin[] = {18,19};
//const int bluePin[] = {4,5};
//const int hsyncPin = 23;
//const int vsyncPin = 15;
//VGA14Bit  vga;  

//auto gb_buf = new RGB222[144 * 160];
//byte * gb_buf2 = (byte *)gb_buf;
//byte gb_buf2[23040];

//VGA static byte * gb_buf_vga = NULL;
//static byte * gb_buf_vga_array[4]= {NULL,NULL,NULL,NULL};


 //gb_time_cur= millis();
// if ((gb_time_cur - gb_time_prev_vga)>gb_fps_vga)
// {    
//  gb_time_prev_vga = gb_time_cur;
  //unsigned long time_prev = micros();
  
  //JJ_SDL_Flip(gb_buf_vga,gb_scanline_p);  
  
  //JJ_SDL_Flip(gb_buf_vga_array);  
  //DisplayController.writeScreen(rect,gb_buf); //Tarda 1975 micros (writeScreen) y 5553 en total(Flip  y writeScreen)    


  /*unsigned short int contDest=0;
  for (byte j=0;j<144;j++)
  {   
   //memcpy(gb_scanline_p[j],&gb_buf2[contDest],160); //94 microsegundos
   memcpy(gb_scanline_p[j],&gb_buf_vga[contDest],160); //94 microsegundos
   contDest+=160; //252 micros        
  }*/
    
  /*unsigned short int contDest=0;
  for (byte i=0;i<4;i++)
  {
   contDest=0;
   for (byte j=0;j<36;j++)
   {      
    memcpy(DisplayController.getScanline(j),&gb_buf_vga_array[i][contDest],160); //94 microsegundos
    contDest+=160; //252 micros        
   }    
  }*/
  
  //time_prev = micros()-time_prev; //Con optimizacion pasa a 2812 micros
  //printf("Tiempo %d\n",time_prev);  //Con scanlines tarda 757 micros en dibujar vs 1975 del WriteScreen
  
  //sprintf(gbCadDest,"%d %d",gbContLoop,cpu_get_cycles());
  //canvas.setPenColor(Color::White);
  //canvas.setBrushColor(Color::Black);
  //canvas.drawText(100,168,gbCadDest);
// } 
 //gbContLoop++;
 // if (DOUBLEBUFFERING)
 //  canvas.swapBuffers();       



   //gb_dutty_ch01_cur = (mem_get_raw(0xFF11)>>6);
  //gb_vol_ch01_cur = (mem_get_raw(0xFF12)>>4)&0x0F;
  //gb_enable_ch01_cur = (mem_get_raw(0xFF14)>>7)&0x01;
  //gb_dur_ch01_cur = mem_get_raw(0xFF11)&0x1F;
  //gb_dur_ch01_cur = ((64-gb_dur_ch01_cur)/256);
  //gb_duruse_ch01_cur = (mem_get_raw(0xFF14)>>5)&0x01;

//gb_dutty_ch02_cur = (mem_get_raw(0xFF16)>>6);
  //gb_vol_ch02_cur = (mem_get_raw(0xFF17)>>4)&0x0F;
  //gb_enable_ch02_cur = (mem_get_raw(0xFF24)>>7)&0x01;  
  //gb_dur_ch02_cur = mem_get_raw(0xFF16)&0x1F;
  //gb_dur_ch02_cur = ((64-gb_dur_ch02_cur)/256);
  //gb_duruse_ch02_cur = (mem_get_raw(0xFF19)>>5)&0x01;  
  //gb_vol_ch03_cur = (mem_get_raw(0xFF1C)>>5)&0x0F;  



   /*gb_frecuencia01 = gb_frec_ch01_cur; 
   gb_volumen01 = gb_vol_ch01_cur;
   gb_dutty_ch01 = gb_dutty_ch01_cur;

   gb_frecuencia02 = gb_frec_ch02_cur;    
   gb_volumen02 = gb_vol_ch02_cur;
   gb_dutty_ch02 = gb_dutty_ch02_cur;

  gb_frecuencia03 = gb_frec_ch03_cur;    
  gb_volumen03 = gb_vol_ch03_cur;*/


//vga.init(vga.MODE320x200, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
//vga.init(vga.MODE320x175, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
//jj = (byte *)calloc(1, 0x10000); 
  //printf ("int %d byte %d fast %d uint8 %d\n",sizeof(int),sizeof(byte),sizeof(uint_fast8_t),sizeof(uint8_t));

//********************************
/*bool DownloadROM(char const * URL, char const * filename)
{
 //AutoSuspendInterrupts autoInt;
 bool success = false;
 MEMResetROM();
 printf ("url %s file &s OK\n",URL,filename);
 HTTPClient http; 
 http.begin(URL);
 int httpCode = http.GET();
 if (httpCode == HTTP_CODE_OK) {
  printf ("HTTP OK\n");
  int len = http.getSize();
  printf ("LEN %d\n",len);
  WiFiClient * stream = http.getStreamPtr();
  int dsize = 0;
  printf ("Comienzo descarga\n");
  while (http.connected() && (len > 0 || len == -1)) 
  {
   size_t size = stream->available();
   //printf ("Stream size %d\n",size);
   if (size) 
   {
    int c = stream->readBytes(gb_bufWIFI, fabgl::imin(sizeof(gb_bufWIFI), size));
    MEMUpload(gb_bufWIFI,c);
    dsize += c;
    if (len > 0)
     len -= c;   
   }   
   //free(buf);
   success = (len == 0 || (len == -1 && dsize > 0));
  }
  printf ("Fin descarga\n");
 }
 else
 {
  printf ("ERROR HTTP_CODE_OK\n");
 }
 
 rom_initWIFI(gb_rom_wifi);
 MemInitPendingWIFI();
 mem_initWIFI();
 cpu_init();
 printf ("Ciclos %d",cpu_get_cycles());
 //MemPrintROMWIFI();
 
 //fabgl::resumeInterrupts();
 return success;
}
*/


//********************************
/*void ConectarWiFi()
{
 fabgl::suspendInterrupts();
 char cad_SSID[32]="";//Poner red
 char cad_psw[32]="";//Poner clave
 WiFi.begin(cad_SSID, cad_psw);
 for (int i = 0; i < 6 && WiFi.status() != WL_CONNECTED; ++i)
 {
  WiFi.reconnect();
  delay(1000);
 }
 printf("WIFI SSID %s\n",cad_SSID);
 printf("WIFI psw %s\n",cad_psw);
 if (WiFi.status() == WL_CONNECTED)
  printf("WIFI Conectado\n");
 else 
  printf("WIFI NO Conectado\n"); 
 fabgl::resumeInterrupts();
 if (WiFi.status() != WL_CONNECTED) 
  printf("Failed to connect WiFi. Try again!\n");
} 
*/

  /*soundGenerator.attach(&sine01);
  soundGenerator.attach(&sine02);
  soundGenerator.attach(&sine03);
  sine01.enable(true);
  sine02.enable(true);
  sine03.enable(true);
  sine01.setFrequency(0);
  sine02.setFrequency(0);
  sine03.setFrequency(0);

  //sine01.setFrequency(gb_frec_ch01_cur);  
  //sine02.setFrequency(gb_frec_ch02_cur);      
  //sine03.setFrequency(gb_frec_ch03_cur);         
   //SineWaveformGenerator sine01;
 //SineWaveformGenerator sine02;
 //SineWaveformGenerator sine03;
 //static int gb_frec_ch01_cur; 
 //static int gb_frec_ch02_cur;
 //static int gb_frec_ch03_cur; 
*/
//char gbCadDest[200]="\0";
//Preferences preferences;
//#define DOUBLEBUFFERING 1
//fabgl::Canvas canvas(&DisplayController);
//#include "app.h"
//void ConectarWiFi(void);
 //********************************
 //void LoadLookupVGA32()
 //{
 // for (unsigned char i=0;i<144;i++)
 //  gb_lookup_vga32[i]= i*160;
 //}
 //unsigned short int gb_lookup_vga32[144];

 //switch (num)
 //{  
  //case 0: rom_init(gb_rom_batman); break;
  //case 1: rom_init(gb_rom_bomberman); break;
  //case 2: rom_init(gb_rom_contra); break;
  //case 0: rom_init(gb_rom_mario); break;
  //case 1: rom_init(gb_rom_tetris); break;
  //case 2: rom_init(gb_rom_Asteroids); break;
  //case 3: rom_init(gb_rom_AstroRabby); break;
  //case 4: rom_init(gb_rom_pacman); break;
  //case 5: rom_init(gb_rom_spiderman); break;
  //case 6: rom_init(gb_rom_loopz); break;
  //case 7: rom_init(gb_rom_BotBInvite); break;
  //case 8: rom_init(gb_rom_wobbly); break;
  //case 9: rom_init(gb_rom_naavis); break;  
  //case 0: rom_init(gb_rom_batman); break;
  //case 1: rom_init(gb_rom_lionking); break;
  //case 2: rom_init(gb_rom_contra); break;*/
  //case 3: rom_init(gb_rom_bomberman); break;
  //case 4: rom_init(gb_rom_donkeykong); break;
  //case 5: rom_init(gb_rom_hoshi); break;
  //case 6: rom_init(gb_rom_metroid); break;
  //case 7: rom_init(gb_rom_mortal); break;
  //case 8: rom_init(gb_rom_prince); break;
  //case 9: rom_init(gb_rom_wario); break;    
  //default: break;
 //}
