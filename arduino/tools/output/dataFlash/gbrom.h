#ifndef _GB_ROM_H
 #define _GB_ROM_H

 //#include <stddef.h>
 #include "roms/romonehour.h"

 #define max_list_rom 1


 //roms
 //Titulos
 static const char * gb_list_rom_title[max_list_rom]={
  "onehour"
 };

 //Datos rom
 static const unsigned char * gb_list_rom_data[max_list_rom]={
  gb_rom_onehour
 };

 //Tamanio en bytes
 //static const int gb_list_rom_size[max_list_rom]={
 // 32768
 //};

#endif
