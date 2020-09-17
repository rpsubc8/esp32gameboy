//lib_compile_fabgl - compile fabgl library, keyboard with sound
//lib_compile_vga32 - compile vga32, keyb not sound
#define lib_compile_fabgl
//#define lib_compile_vga32

//use_max_ram - max ram comsume, all bank cache
//user_min_ram - min ram consume, not bank cache
#define use_max_ram
//#define use_min_ram

//buffer circular 4  8 elementos solo si se usa max ram
#ifdef use_max_ram
 #ifdef lib_compile_fabgl 
  #define maxBankSwitch 4
 #endif
 #ifdef lib_compile_vga32
  #define maxBankSwitch 8
 #endif
#endif 

#ifdef lib_compile_vga32
 //Pin keyboard PS2
 #define KEYBOARD_DATA 32
 #define KEYBOARD_CLK 33
#endif 

//Milliseconds read joystick 1000 div 60 fps
#define gb_ms_joy 16
