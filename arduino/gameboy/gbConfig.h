#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Ya no se usa lib_compile_fabgl
 //lib_compile_vga32 - compile vga32
 #define lib_compile_vga32

 //Usar sonido con libreria fabgl aunque se elija bitluni vga32 
 //#define use_lib_vga8colors
 #define use_lib_vga64colors
 //Modo vga sin doble buffer problemas con modos fast
 //#define use_lib_vga_low_memory 
 #define use_lib_200x150
 //#define use_lib_320x175 

 //Opciones para bitluni vga32 con sonido
 //#define use_lib_fabgl_sound

 //modos solo para bitluni vga opcion lib_compile_vga32
 //#define use_lib_fast_vga
 //#define use_lib_fast_vgaBLUE8colors
 //#define use_lib_fast_vgaRED8colors
 
 //Modo hilos muy lento por ahora
 //#define use_lib_vga_thread
 
 //No usa font bitluni, ahorra memoria
 //#define use_lib_tinyFont

 //log serial usb
 #define use_lib_log_serial

 //skip frames fast mode
 //#define gb_frame_crt_skip

 //use_max_ram - max ram comsume, all bank cache
 //use_half_ram - half ram consume, half bank cache
 //user_min_ram - min ram consume, not bank cache
 //#define use_max_ram
 //#define use_half_ram
 #define use_min_ram

 //Bank switch optimice speed
 //#define usb_lib_optimice_checkchange_bankswitch

 //buffer circular 4  8 elementos solo si se usa max ram
 #ifdef use_max_ram
  //#ifdef lib_compile_fabgl 
  // #define maxBankSwitch 4
  //#endif
  //#ifdef lib_compile_vga32
   #define maxBankSwitch 8
  //#endif
 #else
  #ifdef use_half_ram
   //#ifdef lib_compile_fabgl 
   // #define maxBankSwitch 4
   //#endif
   //#ifdef lib_compile_vga32
    #define maxBankSwitch 4
   //#endif
  #endif
 #endif 

 #ifdef lib_compile_vga32
  //Pin keyboard PS2
  #define KEYBOARD_DATA 32
  #define KEYBOARD_CLK 33
 #endif 

 #ifdef use_lib_fast_vgaBLUE8colors
  #ifndef use_lib_fast_vga
   #define use_lib_fast_vga
  #endif 
 #endif

 #ifdef use_lib_fast_vgaRED8colors
  #ifndef use_lib_fast_vga
   #define use_lib_fast_vga
  #endif 
 #endif

 //Milliseconds read joystick 1000 div 60 fps
 #define gb_ms_joy 16

#endif
