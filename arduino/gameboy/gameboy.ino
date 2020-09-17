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
//*                                                      *
//* Modificar gbConfig.h para usar fabgl o vga32         *
//* Teclas:                                              *
//*  Arriba, abajo, izquierda, derecha                   *
//*  a - A                                               *
//*  z - B                                               *
//*  Barra espaciadora - Start                           *
//*  ENTER - SELECT                                      *
//********************************************************

#include "gbConfig.h"
#ifdef lib_compile_vga32
 #include <ESP32Lib.h> //Para VGA32
 #include <Ressources/Font6x8.h>//Para VGA32
#endif
 
//#include <WiFi.h>
//#include <HTTPClient.h>
//#include <Preferences.h>
#include <stdio.h>
//#include <esp_bt.h>
#include "soc/rtc.h"

#ifdef lib_compile_fabgl
 #include "fabgl.h" //Para fabgl
 #include "fabutils.h" //Para fabgl
 SoundGenerator soundGenerator;
 SineWaveformGenerator gb_sineArray[3];
#endif

#include "sdl.h"
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "gbrom.h"

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

static unsigned char *gb_mem = NULL; //[0x10000]; //La memoria

unsigned int gb_free_ram_boot; //RAM libre en el arranque
unsigned int gb_free_ram_boot_setup; //RAM libre fin setup
unsigned long gb_time_cur;
unsigned long gb_time_prev_joy;

unsigned short int gb_lookup_160lines[144]; //Tablas para las 144 lineas

#ifdef lib_compile_fabgl
 fabgl::VGAController DisplayController; //Seccion FABGL
 fabgl::PS2Controller PS2Controller; //Fin Seccion FABGL
#endif

#ifdef lib_compile_vga32 
 const int redPin[] = {21,22}; //Seccion VGA32
 const int greenPin[] = {18,19};
 const int bluePin[] = {4,5};
 const int hsyncPin = 23;
 const int vsyncPin = 15;
 VGA6Bit gb_vga;  //Fin seccion VGA32
#endif



StructButtons gb_buttons; //Botones del joystick
int gb_line_cur,gb_line_prev;

unsigned char gb_silence=0;

#ifdef lib_compile_fabgl
 void Poll_JoystickFabgl(void);
 void ShowOptionsFabgl(void);
 void LoadLookupFabgl(void);
 void sound_cycleFabgl(void);
  
 short int gb_frec_array_cur[3];
 uint8_t * gb_scanline_p[144]; //Puntero a scanlines fabgl
#endif
#ifdef lib_compile_vga32
 void Poll_JoystickVGA32(void);
 //void LoadLookupVGA32(void);
 void FlipLineFastVGA32(int aLine);
 void FlipLineFastRedVGA32(int aLine); 
 void PollShowMenuVGA32(void);
 void ShowOptionsVGA32(void); 
#endif

#ifdef use_max_ram
 unsigned char* gb_bank_switch_array[maxBankSwitch]; //Cache de 4 y 8 bancos
#endif 

unsigned char* gb_ptrMem_raw=NULL; //La dire de Mem
unsigned char* gb_video_buffer_gb;//puntero a la pantalla video gameboy

void Poll_runGB(void);
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
 void FlipLineFastVGA32(int aLine)
 {
  //unsigned short int contOri=0;  
  //unsigned short int contDest=0; 
  int a;
  if ((aLine<0)||(aLine>143))
   return;
 
  unsigned short int auxOffset= gb_lookup_160lines[aLine];
  //for (contOri=auxOffset; contOri<(auxOffset+160); contOri++,contDest++)
  for (unsigned char x=0;x<160;x++)
  {
   a = gb_video_buffer_gb[auxOffset++];
   a = (a|(a<<2)|(a<<4));
   gb_vga.dotFast(x,aLine,a);
  } 
 }

 //********************************
 void FlipLineFastRedVGA32(int aLine)
 {
  //unsigned short int contOri=0;  
  //unsigned short int contDest=0; 
  //int a;
  if ((aLine<0)||(aLine>143))
   return;
 
  unsigned short int auxOffset= gb_lookup_160lines[aLine];
  //for (contOri=auxOffset; contOri<(auxOffset+160); contOri++,contDest++) 
  for (unsigned char x=0;x<160;x++)
  {   
   gb_vga.dotFast(x,aLine,gb_video_buffer_gb[auxOffset++]);
  }   
 }
#endif

//********************************
void Poll_runGB()
{
 gb_time_cur= millis();
 if ((gb_time_cur - gb_time_prev_joy)>gb_ms_joy)
 {
  gb_time_prev_joy = gb_time_cur;  
  #ifdef lib_compile_fabgl
   Poll_JoystickFabgl(); //fabgl
  #endif
  #ifdef lib_compile_vga32
   Poll_JoystickVGA32();
  #endif 
 }    
 // put your main code here, to run repeatedly:    
 cpu_cycle();
 //CPUIncCycle (1);
 lcd_cycle_fast(); //lcd_cycle();   
 timer_cycle();
 
 gb_line_cur= lcd_get_line(); 
 if (gb_line_cur != gb_line_prev)
 {
  gb_line_prev = gb_line_cur;
  switch (SDLGetModeVisual())
  { 
   #ifdef lib_compile_fabgl
    case MODE_GRAY: SDL_FlipLineFastFabgl(gb_line_cur); break;
    case MODE_FAST_RED: SDL_FlipLineRedFastFabgl(gb_line_cur); break;    
   #endif
   #ifdef lib_compile_vga32
    case MODE_GRAY: FlipLineFastVGA32(gb_line_cur); break;
    case MODE_FAST_RED: FlipLineFastRedVGA32(gb_line_cur); break;
   #endif 
   default: break;
  }
  #ifdef lib_compile_fabgl
   if ((gb_line_cur==144)&&(gb_silence==0))
    sound_cycleFabgl();   
  #endif
 }
}

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
 rom_init(gb_roms_data[0]);
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
 gb_free_ram_boot = ESP.getFreeHeap();
 gb_video_buffer_gb = (unsigned char *)malloc(23040); //pantalla video gameboy
 SDL_AssignPtrPixels(gb_video_buffer_gb);

 gb_mem = (unsigned char *)malloc(0x10000);
 MemAssignPtrMem(gb_mem);

 LoadLookup160lines();
 LCDAssignLookup160lines(gb_lookup_160lines);
 SDLAssignLookup160lines(gb_lookup_160lines); 

 printf("HEAP BEGIN %d\n", gb_free_ram_boot);  
 printf("HEAP malloc %d\n", ESP.getFreeHeap());

 #ifdef use_max_ram
  for (unsigned char i=0;i<maxBankSwitch;i++)
   gb_bank_switch_array[i] = (unsigned char *)malloc(0x4000);  
  MemPreparaBankSwitchPtr(gb_bank_switch_array);
 #endif 

 InitGB();
 printf("HEAP InitGB %d\n", ESP.getFreeHeap());
 
 #ifdef lib_compile_fabgl  
  DisplayController.begin(); //Seccion fabgl
  DisplayController.setResolution(VGA_320x200_75Hz);  
  PS2Controller.begin(PS2Preset::KeyboardPort0_MousePort1, KbdMode::GenerateVirtualKeys);  
  PS2Controller.keyboard()->setLayout(&fabgl::UKLayout); 
  DisplayController.enableBackgroundPrimitiveExecution(false); //Quito background timer
  DisplayController.enableBackgroundPrimitiveTimeout(false); 
 #endif
   
  gb_time_cur= gb_time_prev_joy = millis();
  #ifdef lib_compile_fabgl
   LoadLookupFabgl(); //Seccion fabgl para scanline displaycontroller
   SDL_AssignVGA(gb_scanline_p);   
  #endif 

  #ifdef lib_compile_vga32      
   gb_vga.init(gb_vga.MODE200x150, redPin, greenPin, bluePin, hsyncPin, vsyncPin); //Ahorro mas RAM   
   gb_vga.clear(0); //Seccion VGA32
   kb_begin();   
  #endif 
  
  printf ("Memoria %d %d %d %d\n",ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());
  printf("Free memory (total, min, largest): %d, %d, %d\n", heap_caps_get_free_size(0), heap_caps_get_minimum_free_size(0), heap_caps_get_largest_free_block(0));

  gb_free_ram_boot_setup = ESP.getFreeHeap();
  printf("HEAP END %d\n", gb_free_ram_boot_setup);

  // adjust this to center screen in your monitor
  //DisplayController.moveScreen(20, -2);
 #ifdef lib_compile_fabgl
  ShowOptionsFabgl();
  for (unsigned char i=0;i<3;i++)
  {
   soundGenerator.attach(&gb_sineArray[i]);
   gb_sineArray[i].enable(true);
   gb_sineArray[i].setFrequency(0);
  }
  soundGenerator.play(true);
 #endif 
 gb_line_cur=gb_line_prev=0;
}

#ifdef lib_compile_fabgl
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

  for (unsigned char i=0;i<3;i++)
   gb_sineArray[i].setFrequency(gb_frec_array_cur[i]);
 }

 //***********************
 void LoadLookupFabgl()
 {
  for (byte j=0;j<144;j++)
   gb_scanline_p[j]=DisplayController.getScanline(j);    
 }

 //***********************
 void ShowOptionsFabgl()
 {
  int x=170;
  int y=0;
  Canvas cv(&DisplayController);
  cv.selectFont(&fabgl::FONT_8x8);
  cv.clear();
  cv.drawText(x, y,   "F1 RESET");
  cv.drawText(x, y+=8,"F2 REBOOT");
  cv.drawText(x, y+=8,"F3 RED FAST");
  cv.drawText(x, y+=8,"F4 GRAY");
  cv.drawText(x, y+=8,"F5 SILENCE");
  cv.drawText(x, y+=8,"F6 SOUND");
  cv.drawText(x, y+=8,"-  NORMAL SPEED");
  cv.drawText(x, y+=8,"+  TURBO SPEED");
  cv.drawText(x, y+=8,"*  MEGA SPEED");
  for (unsigned char i=0;i<max_rom_gb;i++)  
   cv.drawText(x, y+=8,gb_roms_title[i]);
 // cv.drawText(x, y+=8,"2  Contra");  
 // cv.drawText(x, y+=8,"0  MARIO");
 // cv.drawText(x, y+=8,"1  TETRIS");
 // cv.drawText(x, y+=8,"2  ASTEROIDS");
 // cv.drawText(x, y+=8,"3  ASTRO RABBY");
 // cv.drawText(x, y+=8,"4  PACMAN");
 // cv.drawText(x, y+=8,"5  SPIDERMAN");
 // cv.drawText(x, y+=8,"6  LOOPZ");
 // cv.drawText(x, y+=8,"7  BotBInvite");
 // cv.drawText(x, y+=8,"8  wobbly");
 // cv.drawText(x, y+=8,"9  naavis");
 // cv.drawText(x, y+=8,"0  Batman");
 // cv.drawText(x, y+=8,"1  Lion King");
 // cv.drawText(x, y+=8,"2  Contra");
 // cv.drawText(x, y+=8,"3  Bomberman");
 // cv.drawText(x, y+=8,"4  Donkey Kong");
 // cv.drawText(x, y+=8,"5  Hoshi");
 // cv.drawText(x, y+=8,"6  Metroid");
 // cv.drawText(x, y+=8,"7  Mortal 3");
  //cv.drawText(x, y+=8,"8  Prince");
  //cv.drawText(x, y+=8,"9  Wario"); 
  cv.waitCompletion();  
 }
#endif

#ifdef lib_compile_vga32
 //************************************************
 void ShowOptionsVGA32()
 {
  int x=10;
  int y=10;
  gb_vga.clear(0);
  gb_vga.setFont(Font6x8);
  gb_vga.setCursor(x, y);
  gb_vga.print("F1 RESET"); y+=8; 
  gb_vga.setCursor(x, y);
  gb_vga.print("F2 REBOOT"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("F3 RED FAST"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("F4 GRAY"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("F5 SILENCE"); y+=8;
  gb_vga.setCursor(x, y);  
  gb_vga.print("F6 SILENCE"); y+=8;
  gb_vga.setCursor(x, y);    
  gb_vga.print("-  NORMAL SPEED"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("+  TURBO SPEED"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("*  MEGA SPEED"); y+=8;
  gb_vga.setCursor(x, y); 
  Poll_JoystickVGA32();
  /*gb_vga.print("0  Batman"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("1  Lion King"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("2  Contra"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("3  Bomberman"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("4  Donkey Kong"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("5  Hoshi"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("6  Metroid"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("7  Mortal 3"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("8  Prince"); y+=8;
  gb_vga.setCursor(x, y);
  gb_vga.print("9  Wario"); y+=8; */
 }

 //************************************************
 void Poll_JoystickVGA32()
 {
  gb_buttons.b_select = checkKey(KEY_ENTER);
  gb_buttons.b_start = checkKey(KEY_SPACE);
  gb_buttons.b_right = checkKey(KEY_CURSOR_RIGHT);
  gb_buttons.b_left = checkKey(KEY_CURSOR_LEFT);
  gb_buttons.b_up = checkKey(KEY_CURSOR_UP);
  gb_buttons.b_down = checkKey(KEY_CURSOR_DOWN);
  gb_buttons.b_a = checkKey(KEY_A);
  gb_buttons.b_b = checkKey(KEY_Z);
  
   if (checkKey(KEY_F3))
    SetModeVisual(MODE_FAST_RED); //ROJO rapido
   if (checkKey(KEY_F4))
    SetModeVisual(MODE_GRAY);    //Gris
   if (checkKey(KEY_F5))      
    gb_silence=1; //Silencio   
   if (checkKey(KEY_F6))
    gb_silence=0; //Sonido   
   if (checkKey(KEY_F2))
    ESP.restart(); //Reinicio hard
   if (checkKey(KEY_F1))
    cpu_init(); //Reinicio soft
   //if (checkKey(KEY_F12))
   // gb_state_loop = GFX_MENU;
   if (checkKey(KEY_PLUS))
    LCDSetSpeed(SPEED_TURBO); //Velocidad Turbo          
   if (checkKey(KEY_ASTERISK))   
    LCDSetSpeed(SPEED_MEGATURBO); //Velocidad Mega Turbo     
   if (checkKey(KEY_MINUS))
    LCDSetSpeed(SPEED_NORMAL); //Velocidad Normal        
   
   if (checkKey(KEY_NUM_0))
    CargarJuego(0);   
   if (checkKey(KEY_NUM_1))
    CargarJuego(1);
   if (checkKey(KEY_NUM_2))
    CargarJuego(2);
   if (checkKey(KEY_NUM_3))
    CargarJuego(3);
   if (checkKey(KEY_NUM_4))
    CargarJuego(4);
   if (checkKey(KEY_NUM_5))
    CargarJuego(5);
   if (checkKey(KEY_NUM_6))
    CargarJuego(6);
   if (checkKey(KEY_NUM_7))
    CargarJuego(7);
   if (checkKey(KEY_NUM_8))
    CargarJuego(8);
   if (checkKey(KEY_NUM_9))
    CargarJuego(9);  
    
  jj_sdl_joystick(&gb_buttons);
 }
#endif 

#ifdef lib_compile_fabgl
 //************************************************
 void Poll_JoystickFabgl()
 {     
  //unsigned long time_prev;
  //time_prev = micros();   
  auto keyboard = PS2Controller.keyboard();
  if (keyboard->isKeyboardAvailable())
  {
   gb_buttons.b_select = keyboard->isVKDown(fabgl::VK_KP_ENTER);    
   gb_buttons.b_start = keyboard->isVKDown(fabgl::VK_SPACE);
   gb_buttons.b_right = keyboard->isVKDown(fabgl::VK_RIGHT);
   gb_buttons.b_left = keyboard->isVKDown(fabgl::VK_LEFT);
   gb_buttons.b_up = keyboard->isVKDown(fabgl::VK_UP);
   gb_buttons.b_down = keyboard->isVKDown(fabgl::VK_DOWN);
   gb_buttons.b_a = keyboard->isVKDown(fabgl::VK_a);
   gb_buttons.b_b = keyboard->isVKDown(fabgl::VK_z);   
//   if (keyboard->isVKDown(fabgl::VK_ESCAPE) || keyboard->isVKDown(fabgl::VK_F12))
//    gb_state_loop = GFX_MENU;

   if (keyboard->isVKDown(fabgl::VK_F3))
    SetModeVisual(MODE_FAST_RED); //ROJO rapido
   if (keyboard->isVKDown(fabgl::VK_F4))
    SetModeVisual(MODE_GRAY);    //Gris
   if (keyboard->isVKDown(fabgl::VK_F5))
   {
    soundGenerator.play(false);
    gb_silence= 1; //Silencio
   }
   if (keyboard->isVKDown(fabgl::VK_F6))
   {
    soundGenerator.play(true);
    gb_silence= 0; //Sonido    
   }
   if (keyboard->isVKDown(fabgl::VK_F2))
    ESP.restart(); //Reinicio hard
   if (keyboard->isVKDown(fabgl::VK_F1))
    cpu_init(); //Reinicio soft
   if (keyboard->isVKDown(fabgl::VK_PLUS) || keyboard->isVKDown(fabgl::VK_KP_PLUS))   
    LCDSetSpeed(SPEED_TURBO); //Velocidad Turbo
   if (keyboard->isVKDown(fabgl::VK_ASTERISK ) || keyboard->isVKDown(fabgl::VK_KP_MULTIPLY))       
    LCDSetSpeed(SPEED_MEGATURBO);//Velocidad Mega Turbo      
   if (keyboard->isVKDown(fabgl::VK_MINUS) || keyboard->isVKDown(fabgl::VK_KP_MINUS))   
    LCDSetSpeed(SPEED_NORMAL); //Velocidad Normal    
           
   if (keyboard->isVKDown(fabgl::VK_0))
    CargarJuego(0);   
   if (keyboard->isVKDown(fabgl::VK_1))   
    CargarJuego(1);
   if (keyboard->isVKDown(fabgl::VK_2))
    CargarJuego(2);
   if (keyboard->isVKDown(fabgl::VK_3))   
    CargarJuego(3);
   if (keyboard->isVKDown(fabgl::VK_4))
    CargarJuego(4);
   if (keyboard->isVKDown(fabgl::VK_5))
    CargarJuego(5);
   if (keyboard->isVKDown(fabgl::VK_6))
    CargarJuego(6);
   if (keyboard->isVKDown(fabgl::VK_7))
    CargarJuego(7);
   if (keyboard->isVKDown(fabgl::VK_8))
    CargarJuego(8);
   if (keyboard->isVKDown(fabgl::VK_9))
    CargarJuego(9);
    
   //if (keyboard->isVKDown(fabgl::VK_l))
   //{//Cargar rom
   // //DownloadROM("http://192.168.0.36/mario.gb","mario.gb");
   //}
   //b_start, b_select, b_a, b_b, b_down, b_up, b_left, b_right
  }

  jj_sdl_joystick(&gb_buttons);
  //printf("start %d right %d\n",button_start,button_right);
  //time_prev = micros()-time_prev;
  //printf("Teclado %d\n",time_prev);     
 }
#endif

//****************************************
void CargarJuego(unsigned char num)
{
 if (gb_roms_data[num] != NULL)
 {  
  rom_init(gb_roms_data[num]);  
  //lcdAssignSDLFrameBuffer();  
  MemAssignROMPtrMemory();  
  mem_init();  
  //gb_ptrMem_raw = MemGetAddressMem();  
  cpu_init();  
 }
}

#ifdef lib_compile_vga32
 //****************************************
 void PollShowMenuVGA32()
 {//Muestra Menu
  //gb_state_loop = GFX_GBRUN;
  ShowOptionsVGA32();  
  
  //auto menu = new Menu;
  //menu->run(&DisplayController);
 }
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
 Poll_runGB();
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


//gb_vga.init(gb_vga.MODE320x200, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
//gb_vga.init(gb_vga.MODE320x175, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
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
