#ifndef _HARDWARE_H
 #define _HARDWARE_H

 #include "../gbConfig.h"

 #define SPEAKER_PIN 25

 #define KEYBOARD_DATA 32
 #define KEYBOARD_CLK 33

 //#define MOUSE_DATA 27
 //#define MOUSE_CLK 26


 // 3 bit pins
 //#define RED_PIN_3B 22
 //#define GRE_PIN_3B 19
 //#define BLU_PIN_3B 5

 // 6 bit pins
 //#define RED_PINS_6B 21, 22
 //#define GRE_PINS_6B 18, 19
 //#define BLU_PINS_6B  4,  5

 // VGA sync pins
 //#define HSYNC_PIN 23
 //#define VSYNC_PIN 15

 #ifdef use_lib_vga8colors  //       BGR 
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

#endif
