//#define lib_compile_vga32
#define lib_compile_fabgl

//buffer circular 4  8 elementos
#ifdef lib_compile_fabgl
 #define maxBankSwitch 4
#endif
#ifdef lib_compile_vga32
 #define maxBankSwitch 8 
#endif

//Milliseconds read joystick 1000 div 60 fps
#define gb_ms_joy 16
