#include <Arduino.h>
#include "lcd.h"
#include "cpu.h"
#include "interrupt.h"
#include "sdl.h"
#include "mem.h"


static int lcd_line;

static unsigned char * ptr_sdl_framebuffer=NULL;
static unsigned short int * gb_pre_y_scanline; //precalculos cada linea 144 lineas
//const unsigned char kk[40] PROGMEM ={
// 0   , 160, 320, 480, 640, 800, 960,1120,1280,1440,
// 1600,1760,1920,1080,2240,2400,1560,2720,2880,3040,
// 3200,3360,3520,3680,3840,4000,4160,4320,4480,4640,
// 4800,4960,5120,5280,5440,5600,5760,5920,6080,6240
//};

//static int lcd_line;
static int lcd_ly_compare;


/* LCD STAT */
static int ly_int;	/* LYC = LY coincidence interrupt enable */
static int mode2_oam_int;
static int mode1_vblank_int;
static int mode0_hblank_int;
static int ly_int_flag;
static int lcd_mode;

/* LCD Control */
static int lcd_enabled;
static int window_tilemap_select;
static int window_enabled;
static int tilemap_select;
static int bg_tiledata_select;
static int sprite_size;
static int sprites_enabled;
static int bg_enabled;
static int scroll_x, scroll_y;
static int window_x, window_y;

static byte bgpalette[] = {3, 2, 1, 0};
static byte sprpalette1[] = {0, 1, 2, 3};
static byte sprpalette2[] = {0, 1, 2, 3};
const unsigned long colours[4] PROGMEM = {0xFFFFFF, 0xC0C0C0, 0x808080, 0x000000};

struct sprite {
	//int y, x, tile, flags; //Para optimizar
  short int y, x;
  unsigned char tile, flags;  
};

static struct sprite sSprite[10]; //Los sprites globales no usar pila

enum {
	PRIO  = 0x80,
	VFLIP = 0x40,
	HFLIP = 0x20,
	PNUM  = 0x10
};

//*****************************************
void LCDAssignLookup160lines(unsigned short int *ptr)
{
 gb_pre_y_scanline = ptr;
 //for (byte i=0;i<144;i++)
 // gb_pre_y_scanline[i]=(i*160);
}

//*****************************************
void lcdAssignSDLFrameBuffer()
{
 ptr_sdl_framebuffer = sdl_get_framebuffer();
}

//*****************************************
unsigned char lcd_get_stat(void)
{

	return (ly_int)<<6 | lcd_mode;
}

void lcd_write_bg_palette(unsigned char n)
{
	bgpalette[0] = (n>>0)&3;
	bgpalette[1] = (n>>2)&3;
	bgpalette[2] = (n>>4)&3;
	bgpalette[3] = (n>>6)&3;
}

void lcd_write_spr_palette1(unsigned char n)
{
	sprpalette1[0] = 0;
	sprpalette1[1] = (n>>2)&3;
	sprpalette1[2] = (n>>4)&3;
	sprpalette1[3] = (n>>6)&3;
}

void lcd_write_spr_palette2(unsigned char n)
{
	sprpalette2[0] = 0;
	sprpalette2[1] = (n>>2)&3;
	sprpalette2[2] = (n>>4)&3;
	sprpalette2[3] = (n>>6)&3;
}

void lcd_write_scroll_x(unsigned char n)
{
//	printf("x scroll changed to %02x\n", n);
	scroll_x = n;
}

void lcd_write_scroll_y(unsigned char n)
{
	scroll_y = n;
}

int lcd_get_line(void)
{
	return lcd_line;
}

void lcd_write_stat(unsigned char c)
{
	ly_int = !!(c&0x40);
}

void lcd_write_control(unsigned char c)
{
//	printf("LCDC set to %02x\n", c);
//	cpu_print_debug();
	bg_enabled            = !!(c & 0x01);
	sprites_enabled       = !!(c & 0x02);
	sprite_size           = !!(c & 0x04);
	tilemap_select        = !!(c & 0x08);
	bg_tiledata_select    = !!(c & 0x10);
	window_enabled        = !!(c & 0x20);
	window_tilemap_select = !!(c & 0x40);
	lcd_enabled           = !!(c & 0x80);
}

void lcd_set_ly_compare(unsigned char c)
{
	lcd_ly_compare = c;
}

void lcd_set_window_y(unsigned char n) {
	window_y = n;
}

void lcd_set_window_x(unsigned char n) {
	window_x = n;
}

static void swap(struct sprite *a, struct sprite *b)
{
	struct sprite c;

	 c = *a;
	*a = *b;
	*b =  c;
}

static void sort_sprites(unsigned char n)
{
	//JJ optimice int swapped, i;
  //int i;
  //int swapped;
  unsigned char swapped;
  unsigned char i;  //maximo 256 sprites  
	do
	{
		swapped = 0;
		for(i = 0; i < n-1; i++)
		{
			if(sSprite[i].x < sSprite[i+1].x)
			{
				swap(&sSprite[i], &sSprite[i+1]);
				swapped = 1;
			}
		}
	}
	while(swapped);
}

//void drawColorIndexToFrameBuffer(int x, int y, byte idx, byte *b) {
//void drawColorIndexToFrameBuffer(byte x, byte y, byte idx, byte *b)
void drawColorIndexToFrameBuffer(byte x, byte y, byte idx)
{
  //unsigned short int offset = x + y * 160;
  //b[offset >> 2] |= (idx << ((offset & 3) << 1));
  unsigned short int offset= gb_pre_y_scanline[y]+x;
  ptr_sdl_framebuffer[offset]= (~idx&0x03); //aplicamos negativo
}

//void drawColorIndexToFrameBuffer(int x, int y, byte idx, byte *b) {
//  unsigned short int offset = x + y * 160;
//  if (offset<23040)
//   b[offset] = idx;
//}

static void draw_bg_and_window(int line)
{
	int x;  
  //byte x;

	for(x = 0; x < 160; x++)
	{
		unsigned int map_select, map_offset, tile_num, tile_addr, xm, ym;
		unsigned char b1, b2, mask, colour;

		/* Convert LCD x,y into full 256*256 style internal coords */
		if(line >= window_y && window_enabled && line - window_y < 144)
		{
			xm = x;
			ym = line - window_y;
			map_select = window_tilemap_select;
		}
		else {
			if(!bg_enabled)
			{
				//b[line*640 + x] = 0;
        drawColorIndexToFrameBuffer(x,line,0);
				return;
			}
			xm = (x + scroll_x)%256;
			ym = (line + scroll_y)%256;
			map_select = tilemap_select;
		}

		/* Which pixel is this tile on? Find its offset. */
		/* (y/8)*32 calculates the offset of the row the y coordinate is on.
		 * As 256/32 is 8, divide by 8 to map one to the other, this is the row number.
		 * Then multiply the row number by the width of a row, 32, to find the offset.
		 * Finally, add x/(256/32) to find the offset within that row. 
		 */
		map_offset = (ym/8)*32 + xm/8;    //optimice
    //map_offset = ((ym>>3)<<5) + (xm>>3);
    //map_offset = ((ym>>3)<<2) + (xm>>3);

		tile_num = mem_get_raw(0x9800 + map_select*0x400 + map_offset);
		if(bg_tiledata_select)
			tile_addr = 0x8000 + (tile_num*16);
		else
			tile_addr = 0x9000 + ((signed char)tile_num)*16; //((signed char)tile_num)*16

		b1 = mem_get_raw(tile_addr+(ym%8)*2);
		b2 = mem_get_raw(tile_addr+(ym%8)*2+1);
		mask = 128>>(xm%8);
		colour = (!!(b2&mask)<<1) | !!(b1&mask);
		//b[line*640 + x] = colours[bgpalette[colour]];
	  drawColorIndexToFrameBuffer(x,line,bgpalette[colour]);
	}
}

//static void draw_sprites(byte *b, int line, int nsprites, struct sprite *s)
static void draw_sprites(int line, int nsprites)
{
	int i;

	for(i = 0; i < nsprites; i++)
	{
		unsigned int b1, b2, tile_addr, sprite_line, x;

		/* Sprite is offscreen */
		if(sSprite[i].x < -7)
			continue;

		/* Which line of the sprite (0-7) are we rendering */
		sprite_line = sSprite[i].flags & VFLIP ? (sprite_size ? 15 : 7)-(line - sSprite[i].y) : line - sSprite[i].y;

		/* Address of the tile data for this sprite line */
		tile_addr = 0x8000 + (sSprite[i].tile*16) + sprite_line*2;

		/* The two bytes of data holding the palette entries */
		b1 = mem_get_raw(tile_addr);
		b2 = mem_get_raw(tile_addr+1);

		/* For each pixel in the line, draw it */
		for(x = 0; x < 8; x++)
		{
			unsigned char mask, colour;
			byte *pal;

			if((sSprite[i].x + x) >= 160)
				continue;

			mask = sSprite[i].flags & HFLIP ? 128>>(7-x) : 128>>x;
			colour = ((!!(b2&mask))<<1) | !!(b1&mask);
			if(colour == 0)
				continue;


			pal = (sSprite[i].flags & PNUM) ? sprpalette2 : sprpalette1;
			/* Sprite is behind BG, only render over palette entry 0 */
			if(sSprite[i].flags & PRIO)
			{
				//unsigned int temp = b[line*640+(x + s[i].x)];
        //unsigned int temp = ptr_sdl_framebuffer[line*160+(x + sSprite[i].x)];
        unsigned int temp = ptr_sdl_framebuffer[gb_pre_y_scanline[line]+(x + sSprite[i].x)];
				if(temp != colours[bgpalette[0]])
					continue;
			}
			//b[line*640+(x + s[i].x)] = colours[pal[colour]];
		  drawColorIndexToFrameBuffer(x + sSprite[i].x,line,pal[colour]);
		}
	}
}


//Optimizado
static void jj_render_line(byte line)
{
  //byte i, c = 0;
  int i, c = 0;
  //struct sprite s[10];
  //byte *b = sdl_get_framebuffer();
  for(i = 0; i<40; i++)
  {
    int y;
    y = mem_get_raw(0xFE00 + (i<<2)) - 16;
    if(line < y || line >= y + 8+(sprite_size<<3))
      continue;
    sSprite[c].y     = y;
    sSprite[c].x     = mem_get_raw(0xFE00 + (i<<2) + 1)-8;
    sSprite[c].tile  = mem_get_raw(0xFE00 + (i<<2) + 2);
    sSprite[c].flags = mem_get_raw(0xFE00 + (i<<2) + 3);
    c++;
    if(c == 10)
      break;
  }
  if(c)
    sort_sprites(c);
  //Draw the background layer
  draw_bg_and_window(line);
  draw_sprites(line, c);
}


/*static void draw_stuff(void)
{
  byte *b = sdl_get_framebuffer();
	int y, tx, ty;

	for(ty = 0; ty < 24; ty++)
	{
	for(tx = 0; tx < 16; tx++)
	{
	for(y = 0; y<8; y++)
	{
		unsigned char b1, b2;
		int tileaddr = 0x8000 +  ty*0x100 + tx*16 + y*2;

		b1 = mem_get_raw(tileaddr);
		b2 = mem_get_raw(tileaddr+1);   
		//b[(ty*640*8)+(tx*8) + (y*640) + 0 + 0x1F400] = colours[(!!(b1&0x80))<<1 | !!(b2&0x80)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 1 + 0x1F400] = colours[(!!(b1&0x40))<<1 | !!(b2&0x40)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 2 + 0x1F400] = colours[(!!(b1&0x20))<<1 | !!(b2&0x20)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 3 + 0x1F400] = colours[(!!(b1&0x10))<<1 | !!(b2&0x10)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 4 + 0x1F400] = colours[(!!(b1&0x8))<<1 | !!(b2&0x8)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 5 + 0x1F400] = colours[(!!(b1&0x4))<<1 | !!(b2&0x4)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 6 + 0x1F400] = colours[(!!(b1&0x2))<<1 | !!(b2&0x2)];
		//b[(ty*640*8)+(tx*8) + (y*640) + 7 + 0x1F400] = colours[(!!(b1&0x1))<<1 | !!(b2&0x1)];   
	}
	}
	}
}*/



//JJ modificado
//int jj_lcd_cycle(void)
void lcd_cycle_fast()
{   
  int cycles = cpu_get_cycles();
  int this_frame, subframe_cycles;
  static unsigned int prev_line;
  //static byte prev_line;

  switch (gb_speed)
  {
   case SPEED_NORMAL: break;
   case SPEED_TURBO: cycles = cycles<<1; break;
   case SPEED_MEGATURBO: cycles = cycles*3; break;
   default: break;
  }
  //JJ if(sdl_update())
  //JJ  return 0;

  //this_frame = cycles % (70224/4);  
  //lcd_line = this_frame / (456/4);
  this_frame = cycles % 17556;
  lcd_line = this_frame / 114;

  //if (prev_line == lcd_line)
  // return 1;
  //printf ("Frame %d line %d\n",this_frame, lcd_line );

  if(this_frame < 51) //204/4
    lcd_mode = 2;
  else if(this_frame < 71) //284/4
    lcd_mode = 3;
  else if(this_frame < 114) //456/4)
    lcd_mode = 0;
  if(lcd_line >= 144)
    lcd_mode = 1;

  if(lcd_line != prev_line && lcd_line < 144)
    jj_render_line(lcd_line); //Optimizado  

  if(ly_int && lcd_line == lcd_ly_compare)
    interrupt(INTR_LCDSTAT);

  if(prev_line == 143 && lcd_line == 144)
  {
    //draw_stuff();
    interrupt(INTR_VBLANK);
    sdl_frame();
    //printf("INTR VIDEO\n");
  }
  prev_line = lcd_line;

  

  /*for (uint8_t i=0;i<144;i++)
  {
   render_line(i);  
  }
  interrupt(INTR_VBLANK);
  sdl_frame();
  printf("INTR\n");
  prev_line = lcd_line;*/
  //JJ return 1;
  //return;
}

//*********************************
void LCDSetSpeed(byte auxSpeed)
{
 gb_speed = auxSpeed;
}

//Codigo sobra
/*static void render_line(int line)
{
  int i, c = 0;

  struct sprite s[10];
  byte *b = sdl_get_framebuffer();

  for(i = 0; i<40; i++)
  {
    int y;

    y = mem_get_raw(0xFE00 + (i*4)) - 16;
    if(line < y || line >= y + 8+(sprite_size*8))
      continue;

    s[c].y     = y;
    s[c].x     = mem_get_raw(0xFE00 + (i*4) + 1)-8;
    s[c].tile  = mem_get_raw(0xFE00 + (i*4) + 2);
    s[c].flags = mem_get_raw(0xFE00 + (i*4) + 3);
    c++;

    if(c == 10)
      break;
  }

  if(c)
    sort_sprites(s, c);

  //Draw the background layer
  draw_bg_and_window(b, line);

  draw_sprites(b, line, c, s);


}*/


/*int lcd_cycle(void)
{
  int cycles = cpu_get_cycles();
  int this_frame, subframe_cycles;
  //JJ static int prev_line;
  static uint8_t prev_line;

//JJ  if(sdl_update()) //No lo necesitamos, era para leer joystick
//JJ    return 0;

  this_frame = cycles % (70224/4);
  lcd_line = this_frame / (456/4);

  //printf ("%d %d ",this_frame, lcd_line );

  if(this_frame < 204/4)
    lcd_mode = 2;
  else if(this_frame < 284/4)
    lcd_mode = 3;
  else if(this_frame < 456/4)
    lcd_mode = 0;
  if(lcd_line >= 144)
    lcd_mode = 1;
    
  if(lcd_line != prev_line && lcd_line < 144)
    render_line(lcd_line);

  if(ly_int && lcd_line == lcd_ly_compare)
    interrupt(INTR_LCDSTAT);

  if(prev_line == 143 && lcd_line == 144)
  {
    //draw_stuff();
    interrupt(INTR_VBLANK);
    sdl_frame();
   // printf("INTR\n");
  }
  prev_line = lcd_line;
  return 1;
}
*/
