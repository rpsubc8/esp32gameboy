#ifndef OSDFILE_H
 #define OSDFILE_H
 #include "gbConfig.h"
 
 void changeSna2Flash(unsigned char id);
 void do_tinyOSD(void);
 //void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent);
 void SDLActivarOSDMainMenu();
 void SDLOSDClear(void);
 //SDL_Surface * SDLOSDGetSurface();
 void SDLEsperaTeclado(void);
 #ifdef use_lib_tinyFont
  void SDLprintChar(char car,short int x,short int y,unsigned char color,unsigned char backcolor);
  void SDLprintText(const char *cad,short int x, short int y, unsigned char color,unsigned char backcolor);
 #endif  
#endif
