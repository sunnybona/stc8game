#ifndef SSD1306XLED_H
#define SSD1306XLED_H

#include "globals.h"
// ---------------------	// Vcc,	Pin 1 on SSD1306 Board
// ---------------------	// GND,	Pin 2 on SSD1306 Board
#ifndef SSD1306_SCL
#define SSD1306_SCL		P32	// SCL,	Pin 3 on SSD1306 Board
#endif
#ifndef SSD1306_SDA
#define SSD1306_SDA		P33	// SDA,	Pin 4 on SSD1306 Board
#endif
#ifndef SSD1306_SA
#define SSD1306_SA		0x78	// Slave address
#endif

#define SPEAKER P54
#define LEFT P31
#define RIGHT P55
#define ACT  P30
// ----------------------------------------------------------------------------
void ssd1306_init(void);
void ssd1306_xfer_start(void);
void ssd1306_xfer_stop(void);
void ssd1306_send_byte(uint8_t b);
void ssd1306_send_command(uint8_t command);
void ssd1306_send_data_start(void);
void ssd1306_send_data_stop(void);
void ssd1306_setpos(uint8_t x, uint8_t y);
void ssd1306_fillscreen(uint8_t fill_Data);
void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]);
//void ssd1306_char_f8x16(uint8_t x, uint8_t y,const char ch[]);
//void ssd1306_char_f16x16(uint8_t x, uint8_t y, uint8_t N);
void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t bitmap[]);
uint8_t pgm_read_byte(uint8_t *p);
unsigned long millis();
void delay(unsigned int ms);
int random(int a,int b);
void beep(int bCount,int bDelay);
void itoa(int value,char *str,unsigned char base);
void system_sleep() ;
void waitStart();
// ----------------------------------------------------------------------------
#endif