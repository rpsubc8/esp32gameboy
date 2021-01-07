#ifndef _GB_ROM_H
 #define _GB_ROM_H

 //#include <stddef.h>
 #include "roms/romLastCrown.h"
 #include "roms/romRetroid.h"

 #define max_list_rom 2


 //roms
 //Titulos
 static const char * gb_list_rom_title[max_list_rom]={
  "LastCrown",
  "Retroid"
 };

 //Datos rom
 static const unsigned char * gb_list_rom_data[max_list_rom]={
  gb_rom_LastCrown,
  gb_rom_Retroid
 };

 //Tamanio en bytes
 //static const int gb_list_rom_size[max_list_rom]={
 // 262144//,
 // 262144
 //};

#endif
