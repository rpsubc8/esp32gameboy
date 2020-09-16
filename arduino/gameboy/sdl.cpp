#include "sdl.h"
#include "lcd.h"

//#define byte unsigned char

//byte pixels[5760]; //GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4
//byte gb_pixels_backup[5760]; //GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];

//static byte * pixels = NULL; //(byte *)malloc(5760);
//static byte * gb_pixels_backup = NULL; //(byte *)malloc(5760); //GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];

static byte button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

//static byte *gb_sdl_vgaBuf;
static uint8_t **gb_sdl_scanline_p;
static unsigned short int * gb_lookup_y; //precalculos cada linea 144

//****************************
void SDLAssignLookup160lines(unsigned short int *ptr)
{
 gb_lookup_y = ptr;
 //for (byte i=0;i<144;i++)
 // gb_lookup_y[i]=(i*160);
}


//****************************
void SDL_AssignVGA(uint8_t **aux_scanline_p)
{
//VGA  gb_sdl_vgaBuf = auxBuf;
  gb_sdl_scanline_p = aux_scanline_p;
}

//********************************************
unsigned char * SDL_GetPointerPixels()
{
 return pixels;
}

//********************************************
//void SDL_InitBuffers()
//{
// //pixels = (byte *)malloc(5760);
// //gb_pixels_backup = (byte *)malloc(5760); //GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];   
// pixels = (unsigned char *)malloc(23040);
// //gb_pixels_backup = (byte *)malloc(23040);
//}

//*******************************
void SDL_AssignPtrPixels(unsigned char * auxPtr)
{//Asignamos punteor, previo reservado con malloc
 pixels = auxPtr;
}

//*******************************
void SDL_FlipLineRedFastFabgl(int aLine)
{
 unsigned short int contOri=0;  
 unsigned short int contDest=0; 
 //unsigned char auxSwap; 
 if ((aLine<0)||(aLine>143))
  return; //6 microsegundos
 
 //unsigned long time_prev= micros();
 
 for (contOri=gb_lookup_y[aLine]; contOri<(gb_lookup_y[aLine]+160); contOri+=4,contDest+=4)
 {
  //auxSwap = pixels[contOri+2];
  //pixels[contOri+2]= pixels[contOri]; //2     
  //pixels[contOri]= auxSwap; //0   

  //auxSwap = pixels[contOri+3];
  //pixels[contOri+3]= pixels[contOri+1]; //3     
  //pixels[contOri+1]= auxSwap;//1

  gb_sdl_scanline_p[aLine][contDest+2]=pixels[contOri];
  gb_sdl_scanline_p[aLine][contDest+3]=pixels[contOri+1];
  gb_sdl_scanline_p[aLine][contDest+0]=pixels[contOri+2];
  gb_sdl_scanline_p[aLine][contDest+1]=pixels[contOri+3];
 }   
 //contOri=gb_lookup_y[aLine];
 //memcpy(gb_sdl_scanline_p[aLine],&pixels[contOri],160);

 //time_prev = micros()-time_prev;
 //printf("Tiempo %d\n",time_prev);  
}

//*******************************
void SDL_FlipLineFastFabgl(int aLine)
{
 unsigned short int contOri=0; 
 unsigned short int contDest=0; 
 unsigned char a;  
 //unsigned char a,b,c,d;  
 if ((aLine<0)||(aLine>143))
  return;
 //contOri=(aLine*160);
 //printf ("Linea %d\n",aLine);
 //unsigned long time_prev= micros(); //9 a 13 micros
  
 for (contOri=gb_lookup_y[aLine]; contOri<(gb_lookup_y[aLine]+160); contOri+=4,contDest+=4)
 {
  //a = pixels[contOri];
  //b = pixels[contOri+1];
  //c = pixels[contOri+2];
  //d = pixels[contOri+3];  
  //pixels[contOri+2]= (a|(a<<2)|(a<<4)); //2  
  //pixels[contOri+3]= (b|(b<<2)|(b<<4)); //3  
  //pixels[contOri]= (c|(c<<2)|(c<<4)); //0  
  //pixels[contOri+1]= (d|(d<<2)|(d<<4)); //1*/
  a = pixels[contOri];
  gb_sdl_scanline_p[aLine][contDest+2]= (a|(a<<2)|(a<<4)); //2  
  a = pixels[contOri+1];
  gb_sdl_scanline_p[aLine][contDest+3]= (a|(a<<2)|(a<<4)); //3  
  a = pixels[contOri+2];
  gb_sdl_scanline_p[aLine][contDest]= (a|(a<<2)|(a<<4)); //0  
  a = pixels[contOri+3];  
  gb_sdl_scanline_p[aLine][contDest+1]= (a|(a<<2)|(a<<4)); //1  
 }   
// contOri=gb_lookup_y[aLine];
// memcpy(gb_sdl_scanline_p[aLine],&pixels[contOri],160);
 

 //borro la linea internamente
 //memset(&pixels[contOri],0,160);//Borra la linea

 //time_prev = micros()-time_prev;
 //printf("Tiempo %d\n",time_prev);  
}

//********************************************
//void SDL_Flip_fast()
//{//304 microsegundos Reduzco memoria un solo buffer sacrificando un poco velocidad
// //Dibuja un solo color ultra rapido  
// unsigned short int contOri=0; 
// //unsigned long time_prev;
// byte auxSwap;
// //time_prev = micros();
// for (contOri=0; contOri<23040; contOri+=4) 
// {      
//  //gb_sdl_vgaBuf[contOri+2]= gb_pixels_backup[contOri]; //2   
//  //gb_sdl_vgaBuf[contOri+3]= gb_pixels_backup[contOri+1]; //3   
//  //gb_sdl_vgaBuf[contOri]= gb_pixels_backup[contOri+2]; //0   
//  //gb_sdl_vgaBuf[contOri+1]= gb_pixels_backup[contOri+3];//1
//  auxSwap = pixels[contOri+2];
//  pixels[contOri+2]= pixels[contOri]; //2     
//  pixels[contOri]= auxSwap; //0   
//
//  auxSwap = pixels[contOri+3];
//  pixels[contOri+3]= pixels[contOri+1]; //3     
//  pixels[contOri+1]= auxSwap;//1
// }
// contOri=0;
// for (byte j=0;j<144;j++)
// {    
//  //memcpy(gb_sdl_scanline_p[j],&gb_sdl_vgaBuf[contDest],160); //94 microsegundos
//  memcpy(gb_sdl_scanline_p[j],&pixels[contOri],160); //94 microsegundos
//  contOri+=160; //252 micros 
// }
// //time_prev = micros()-time_prev;
// //printf("Tiempo %d\n",time_prev);  
//}
  
//Rutina propia
//void SDL_Flip()
//{//722 microsegundos Sacrifico un poco velocidad usando mismo buffer ahorro memoria
// byte auxColor,col,a,b,c,d; 
// unsigned short int contOri=0;  
// unsigned long time_prev;
// time_prev = micros();
//
// for (contOri=0; contOri<23040; contOri+=4)
// {
//  a = pixels[contOri];
//  b = pixels[contOri+1];
//  c = pixels[contOri+2];
//  d = pixels[contOri+3];
//  auxColor= (a|(a<<2)|(a<<4));
//  pixels[contOri+2]= auxColor; //2
//  auxColor= (b|(b<<2)|(b<<4));
//  pixels[contOri+3]= auxColor; //3
//  auxColor= (c|(c<<2)|(c<<4));
//  pixels[contOri]= auxColor; //0
//  auxColor= (d|(d<<2)|(d<<4));
//  pixels[contOri+1]= auxColor; //1
// }
// 
// contOri=0;
// for (byte j=0;j<144;j++)
// {   
//  //memcpy(gb_scanline_p[j],&gb_buf2[contDest],160); //94 microsegundos
//  memcpy(gb_sdl_scanline_p[j],&pixels[contOri],160); //94 microsegundos
//  contOri+=160; //252 micros        
// }
// 
// //time_prev = micros()-time_prev;
// //printf("Tiempo %d\n",time_prev);  
// 
//}


//Joystick buttons
void jj_sdl_joystick(StructButtons * auxButtons)
{
  button_select =  auxButtons->b_select;
  button_start =  auxButtons->b_start;
  button_right = auxButtons->b_right;
  button_left =  auxButtons->b_left;
  button_up = auxButtons->b_up;
  button_down =  auxButtons->b_down;
  button_a = auxButtons->b_a;
  button_b = auxButtons->b_b;
  return;  
}

byte sdl_get_buttons(void)
{
	//JJreturn (button_start*8) | (button_select*4) | (button_b*2) | button_a;
  return (button_start<<3) | (button_select<<2) | (button_b<<1) | button_a;
}

byte sdl_get_directions(void)
{
	//JJreturn (button_down*8) | (button_up*4) | (button_left*2) | button_right;
  return (button_down<<3) | (button_up<<2) | (button_left<<1) | button_right;  
}

byte* sdl_get_framebuffer(void)
{
	return pixels;
}

//************************************
unsigned char SDLGetModeVisual()
{
 return gb_mode_visual;
}

//************************************
void SetModeVisual(byte auxMode)
{
 gb_mode_visual= auxMode;
}

//*************************************
void sdl_frame(void)
{
 //SDL_Flip(pixels);
 //memcpy(gb_pixels_backup,pixels,5760);
 //memset(pixels,0,5760);//Borra hasta 144 lineas //Tarda 26 micros 
/* gb_time_current_vga = millis();
 if ((gb_time_prev_vga - gb_time_current_vga)>gb_fps_vga)
 {
  gb_time_prev_vga = gb_time_current_vga;
  switch (gb_mode_visual)
  {
   case MODE_GRAY: SDL_Flip(); break;
   case MODE_FAST_RED: SDL_Flip_fast(); break;
   default: SDL_Flip(); break;
  }
 }
 */
 //memcpy(gb_pixels_backup,pixels,23040);
 memset(pixels,0,23040);//Borra hasta 144 lineas //Tarda 26 micros  
}





//Codigo sobra
 /*
  //auxY=0;
  for(y = 0;y<GAMEBOY_HEIGHT;y++)
  {
    //auxX=0;
    //dato=0;    
    for(x = 0;x<GAMEBOY_WIDTH;x++)
    {
        //JJ video update tft.drawPixel(y, x, color[getColorIndexFromFrameBuffer(y, x)]);
        //auxCanvas.setPixel(x,y,Color::Red);
//        auxColor = getColorIndexFromFrameBuffer(y, x)+48;
        //auxColor= ((3-getColorIndexFromFrameBuffer(x, y))<<6);
        auxColor= (3-jj_getColorIndexFromFrameBuffer(x, y));        
        auxColor= (auxColor|(auxColor<<2)|(auxColor<<4));
        auxBuf[contDest++]= auxColor;
        //auxCanvas.setPenColor(auxColor,auxColor,auxColor);
        //auxCanvas.setPixel(x,y);
        //auxBuf[(y*GAMEBOY_WIDTH)+x]=auxColor;
        
        //aux= getColorIndexFromFrameBuffer(y, x)+48;
        //auxCanvas.drawChar(auxX,auxY,aux);
        //if (aux!=0)
        //{
        // printf("%d",aux);
        // dato=1;
        //}
        //auxX+=8;
    }
    //auxY+=GAMEBOY_WIDTH;
        //if (dato!=0)
        // printf("\n");    
    //auxY+=8;
  }

  */



//JJ #include "SPI.h"
//JJ video#include "Adafruit_GFX.h"
//JJ#include "Adafruit_ILI9341.h"

//JJ#define _cs   22   // 3 goes to TFT CS
//JJ#define _dc   21   // 4 goes to TFT DC
//JJ#define _mosi 23  // 5 goes to TFT MOSI
//JJ#define _sclk 19  // 6 goes to TFT SCK/CLK
//JJ#define _rst  18  // ESP RST to TFT RESET
//JJ#define _miso 25    // Not connected
//JJ#define _led   5
//       3.3V     // Goes to TFT LED  
//       5v       // Goes to TFT Vcc
//       Gnd      // Goes to TFT Gnd        

// Use hardware SPI (on ESP D4 and D8 as above)
//Adafruit_ILI9341 tft = Adafruit_ILI9341(_CS, _DC);
// If using the breakout, change pins as desired
//JJ videoAdafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst, _miso);

//JJ video void backlighting(bool state) {
//JJ    if (!state) {
//JJ        digitalWrite(_led, LOW);
//JJ    }
//JJ    else {
//JJ        digitalWrite(_led, HIGH);
//JJ    }
//JJ}


//fabgl::VGAController jj_my_DisplayController;
//fabgl::Canvas jj_my_canvas(&jj_my_DisplayController);
//fabgl::Canvas jj_my_canvas;


  //byte x,y;
  //unsigned int auxX=0;
  //unsigned int auxY=0;
  //uint8_t aux,dato,salir;    
  /*salir=0;
  for(y = 0;y<GAMEBOY_HEIGHT;y++)
  {
   for(x = 0;x<GAMEBOY_WIDTH;x++)
   {
    aux= getColorIndexFromFrameBuffer(x, y);
    if (aux!=0){salir=1; break;}
   }
   if (salir!=0) break;
  }
  if (salir==0) return;*/
  //memset(&auxBuf[0],0,23040);
  //3972 micros
  /*auxColor= (a|(a<<2)|(a<<4));
  auxBuf[contDest++]= auxColor;
  auxColor= (b|(b<<2)|(b<<4));
  auxBuf[contDest++]= auxColor;  
  auxColor= (c|(c<<2)|(c<<4));
  auxBuf[contDest++]= auxColor;  
  auxColor= (d|(d<<2)|(d<<4));
  auxBuf[contDest++]= auxColor;*/


 //tft.fillScreen(ILI9341_BLACK);
 //int i,j;
 //uint8_t aux;
 //for(i = 0;i<GAMEBOY_WIDTH;i++)
 //{
 // for(j = 0;j<GAMEBOY_HEIGHT;j++)
 // {
   //JJ video update tft.drawPixel(j, i, color[getColorIndexFromFrameBuffer(j, i)]);        
   //printf ("%d",color[getColorIndexFromFrameBuffer(j, i)]);
   //aux= getColorIndexFromFrameBuffer(j, i);
   //if (aux!=0)
   // printf("%d",aux);
 // }
  //if (aux!=0)
  // printf("\n");
// }
 //memset(pixels,0,GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4*sizeof(byte)); 


//void sdl_init(void)
//{
  //JJ videotft.begin();
  //JJpinMode(_led, OUTPUT);
  //JJbacklighting(true);
  
  // read diagnostics (optional but can help debug problems)
  //JJuint8_t x = tft.readcommand8(ILI9341_RDMODE);
  //JJSerial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  //JJx = tft.readcommand8(ILI9341_RDMADCTL);
  //JJSerial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  //JJx = tft.readcommand8(ILI9341_RDPIXFMT);
  //JJSerial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  //JJx = tft.readcommand8(ILI9341_RDIMGFMT);
  //JJSerial.print("Image Format: 0x"); Serial.println(x, HEX);
  //JJx = tft.readcommand8(ILI9341_RDSELFDIAG);
  //JJSerial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  //JJtft.fillScreen(ILI9341_RED);

  //JJgpio_pad_select_gpio(GPIO_NUM_14);
  //JJgpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);

  //JJgpio_pad_select_gpio(GPIO_NUM_27);
  //JJgpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
//}
//JJ int sdl_update(void){  
//JJ  //tft.fillScreen(ILI9341_RED);
//JJ    //JJ joystick button_start = !gpio_get_level(GPIO_NUM_14);
//JJ    //JJ button_right = !gpio_get_level(GPIO_NUM_27);
//JJ  return 0;
//JJ}  


  /* 
  if(frames == 0)
    gettimeofday(&tv1, NULL);
  
  frames++;
  if(frames % 1000 == 0)
  {
    gettimeofday(&tv2, NULL);
    printf("Frames %d, seconds: %d, fps: %d\n", frames, tv2.tv_sec - tv1.tv_sec, frames/(tv2.tv_sec - tv1.tv_sec));
  }
 */

//byte getColorIndexFromFrameBuffer(int x, int y);

//byte getColorIndexFromFrameBuffer(int x, int y) {
//  int offset = x + y * 160;
//  return (byte)((pixels[offset >> 2] >> ((offset & 3) << 1)) & 3);
//}

//byte jj_getColorIndexFromFrameBuffer(byte x, byte y) {
//  //JJ optimice int offset = x + y * 160;
//  unsigned short int offset = x + (y * 160);
//  return (byte)((gb_pixels_backup[offset >> 2] >> ((offset & 3) << 1)) & 3);
//} 

//const int color[] = {0x000000, 0x555555, 0xAAAAAA, 0xFFFFFF};





/*for (contOri=0; contOri<5760; contOri++) //Bajado a 870 micros
 {
  col= ~(gb_pixels_backup[contOri]);
  a= (col & 0x03);
  b= ((col>>2)& 0x03);
  c= ((col>>4)& 0x03);
  d= ((col>>6)& 0x03);
  auxColor= (a|(a<<2)|(a<<4));
  auxBuf[contDest+2]= auxColor; //2
  auxColor= (b|(b<<2)|(b<<4));
  auxBuf[contDest+3]= auxColor; //3
  auxColor= (c|(c<<2)|(c<<4));
  auxBuf[contDest]= auxColor; //0
  auxColor= (d|(d<<2)|(d<<4));
  auxBuf[contDest+1]= auxColor; //1
  contDest+=4;
 }*/


 //void SDL_Flip(byte *screen)
//{
// //unsigned long time_prev;
// //time_prev = micros();     
// memcpy(gb_pixels_backup,pixels,5760);
// memset(pixels,0,5760);//Borra hasta 144 lineas //Tarda 26 micros
// //time_prev = micros()-time_prev;
// //printf("Tiempo %d\n",time_prev);
//}
