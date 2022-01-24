#ifndef LCD_H
#define LCD_H
#include <Arduino.h>
//int lcd_cycle(void);
//JJ int lcd_get_line(void);

enum {
 SPEED_NORMAL = 0x00,
 SPEED_TURBO  = 0x01,
 SPEED_MEGATURBO = 0x03
};

static byte gb_speed=SPEED_NORMAL;

void lcdAssignSDLFrameBuffer(void);
void LCDSetSpeed(byte auxSpeed);
void LCDAssignLookup160lines(unsigned short int *ptr);
void drawColorIndexToFrameBuffer(byte x, byte y, byte idx);
int lcd_get_line(void);
unsigned char lcd_get_stat();
void lcd_write_control(unsigned char);
void lcd_write_stat(unsigned char);
void lcd_write_scroll_x(unsigned char);
void lcd_write_scroll_y(unsigned char);
void lcd_write_bg_palette(unsigned char);
void lcd_write_spr_palette1(unsigned char);
void lcd_write_spr_palette2(unsigned char);
void lcd_set_window_y(unsigned char);
void lcd_set_window_x(unsigned char);
void lcd_set_ly_compare(unsigned char);
void lcd_cycle_fast();
#endif
