#ifndef _GB_GLOBALS_H
 #define _GB_GLOBALS_H
 #include "gbConfig.h"
 #include "MartianVGA.h"

 #ifdef lib_compile_vga32  
  #define hsyncPin 23
  #define vsyncPin 15
  #ifdef use_lib_vga8colors 
   #ifdef use_lib_vga_low_memory
    extern VGA3BitI vga;
   #else
    extern VGA3Bit vga;
   #endif
  #else
   #ifdef use_lib_vga_low_memory
    extern VGA6BitI vga;
   #else
    extern VGA6Bit vga;  //Fin seccion VGA32
   #endif
  #endif 
 #endif


 extern unsigned char gb_show_osd_main_menu;
 extern unsigned char gb_lcd_current_frame;
 extern int gb_lcd_line;

 extern unsigned char *gb_ptrMem_raw;

 //gamepad
 extern unsigned char button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

 extern unsigned char gb_current_ms_poll_joy;
 #ifdef use_lib_fabgl_sound
  extern unsigned char gb_silence_all_channels;
  extern unsigned char gb_sound_off;
 #endif 

 #ifdef use_lib_vga_thread
  extern unsigned char* gb_video_buffer_backup_gb;
 #endif

#endif

 
