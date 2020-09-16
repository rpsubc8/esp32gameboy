//Tetris
#ifndef GB_ROM_H
 #define GB_ROM_H 

 #define max_rom_gb 10
 
 #include "roms/gbRom0.h"
 #include "roms/gbRom1.h"
 //#include "gbRom2.h"
 //#include "gbRom3.h"
 //#include "gbRom4.h"
 //#include "gbRom5.h"
 //#include "gbRom6.h"
 //#include "gbRom7.h"
 //#include "gbRom8.h"
 //#include "gbRom9.h"
 

 //10 roms Titulo
 const char * gb_roms_title[max_rom_gb] PROGMEM={
  "0 LastCrown",
  "1 Retroid",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  ""
 };

 //10 roms puntero a datos
 const unsigned char * gb_roms_data[max_rom_gb] PROGMEM={
  gb_rom_lastCrown_data,
  gb_rom_retroid_data,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
 }; 
 
 //#include "gbRomMario.h"
 //#include "gbRomTetris.h"
 /*#include "gbRomAsteroids.h"
 #include "gbRomAstroRabby.h"
 #include "gbRomPacman.h"
 #include "gbRomSpiderman.h"
 #include "gbRomLoopz.h"
 #include "gbRomBotBInvite.h"
 #include "gbRomWobbly.h"
 #include "gbRomNaavis.h"*/

 //#include "gbRomBatman.h"
 /*#include "gbRomLionking.h"*/
 //#include "gbRomContra.h" 
 //#include "gbRomBomberman.h" 
 /*#include "gbRomDonkeykong.h" 
 #include "gbRomHoshi.h" 
 #include "gbRomMetroid.h" 
 #include "gbRomMortal3.h" 
 //#include "gbRomPrince.h" 
 //#include "gbRomWario.h" 
 */
 //const int gb_rom_size=65537;

#endif
