#include <STC8G.H>

#include "ssd1306xled.h"
#include "font6x8.h"
#include <stdlib.h>
#include <intrins.h>
#include <stdio.h>

#define FOSC 24000000UL
#define T1M (65536-FOSC/1000)
unsigned long timeCnt;


void waitStart()
{
  unsigned long waitFrame=timeCnt;
  while(ACT==1)
  {
	  if(timeCnt-waitFrame>2000){
		  waitFrame=timeCnt;
	  }
	  else if(timeCnt-waitFrame>1000) {
		  ssd1306_char_f6x8(20, 4, "press A start");
		  
	  }
	  else{
	   ssd1306_char_f6x8(20, 4, "             ");
	   
	  }
	  
  }
}

void itoa(int value,char *s,unsigned char base)
{
	sprintf(s,"%d",value);
}
	
void system_sleep() {
 
}

void beep(int bCount,int bDelay){
	int i,i2;
  for (i = 0; i<=bCount; i++)
	{
		SPEAKER=1;
		for(i2=0; i2<bDelay; i2++)
	  {
	       _nop_();_nop_();
		}
	       
	  SPEAKER=0;
	  for(i2=0; i2<bDelay; i2++)
	  {
	      _nop_();_nop_();
	  }
	}
}

void Timer0_Isr(void) interrupt 1
{
	timeCnt++;
}

void Timer0_Init(void)		//1毫秒@24.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = T1M;				//设置定时初始值
	TH0 = T1M>>8;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA=1;
}

unsigned long millis()
{
	return timeCnt;
}

int random(int a,int b)
{
	return(rand()%b-a);
}

void delay(unsigned int ms)
{
	unsigned long startCnt=millis();
	while((timeCnt-startCnt)<ms);
}

void Wait()
{
	while(!(I2CMSST & 0X40));
	I2CMSST &= ~0X40;
}

void IIC_Start(){
	I2CMSCR=0X01;
	Wait();
}

	
void IIC_Ack(){
	I2CMSCR=0X03;
	Wait();
}

void IIC_Stop(){
	I2CMSCR=0X06;
	Wait();
}


void IIC_WriteByte(uint8_t IIC_Data){
	I2CTXD=IIC_Data;
	I2CMSCR=0X02;
	Wait();
	
	
}



void IIC_Write(uint8_t DevAddr,uint8_t Cmd,uint8_t dat){
	IIC_Start();
	IIC_WriteByte(DevAddr);
	IIC_Ack();
	IIC_WriteByte(Cmd);
	IIC_Ack();
	
	IIC_WriteByte(dat);
	IIC_Ack();
	
	IIC_Stop();
}



void ssd1306_init(void){
	P3M0=0X00;
	P3M1=0X00;
	P5M0=0X00;
	P5M1=0X00;
	P_SW2 =0X80;
  I2CCFG=0Xc1;
//	I2CCFG=0Xe0;
	I2CMSST=0X00;
	Timer0_Init();
	ssd1306_send_command(0xAE); // display off
	ssd1306_send_command(0x00); // Set Memory Addressing Mode
	ssd1306_send_command(0x10); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_send_command(0x40); // Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_send_command(0x81); // Set COM Output Scan Direction
	ssd1306_send_command(0xCF); // ---set low column address
	ssd1306_send_command(0xA1); // ---set high column address
	ssd1306_send_command(0xC8); // --set start line address
	ssd1306_send_command(0xA6); // --set contrast control register
	ssd1306_send_command(0xA8);
	ssd1306_send_command(0x3F); // --set segment re-map 0 to 127
	ssd1306_send_command(0xD3); // --set normal display
	ssd1306_send_command(0x00); // --set multiplex ratio(1 to 64)
	ssd1306_send_command(0xD5); // 
	ssd1306_send_command(0x80); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_send_command(0xD9); // -set display offset
	ssd1306_send_command(0xF1); // -not offset
	ssd1306_send_command(0xDA); // --set display clock divide ratio/oscillator frequency
	ssd1306_send_command(0x12); // --set divide ratio
	ssd1306_send_command(0xDB); // --set pre-charge period
	ssd1306_send_command(0x40); // 
	ssd1306_send_command(0x20); // --set com pins hardware configuration
	ssd1306_send_command(0x02);
	ssd1306_send_command(0x8D); // --set vcomh
	ssd1306_send_command(0x14); // 0x20,0.77xVcc
	ssd1306_send_command(0xA4); // --set DC-DC enable
	ssd1306_send_command(0xA6); // 
	ssd1306_send_command(0xAF); // --turn on oled panel 
}

void ssd1306_xfer_start(void){
	IIC_Start();
}

void ssd1306_xfer_stop(void){
	IIC_Stop();
}

void ssd1306_send_byte(uint8_t b){
	IIC_WriteByte(b);
	IIC_Ack();
}

void ssd1306_send_command(uint8_t command){
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);  // Slave address, SA0=0
	ssd1306_send_byte(0x00);	// write command
	ssd1306_send_byte(command);
	ssd1306_xfer_stop();
}

void ssd1306_send_data_start(void){
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);
	ssd1306_send_byte(0x40);	//write data
}

void ssd1306_send_data_stop(void){
	ssd1306_xfer_stop();
}

void ssd1306_setpos(uint8_t x, uint8_t y)
{
	ssd1306_xfer_start();
	ssd1306_send_byte(SSD1306_SA);  //Slave address,SA0=0
	ssd1306_send_byte(0x00);	//write command

	ssd1306_send_byte(0xb0+y);
	ssd1306_send_byte(((x&0xf0)>>4)|0x10); // |0x10
	ssd1306_send_byte((x&0x0f)|0x01); // |0x01

	ssd1306_xfer_stop();
}

void ssd1306_fillscreen(uint8_t fill_Data){
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		ssd1306_send_command(0xb0+m);	//page0-page1
		ssd1306_send_command(0x00);		//low column start address
		ssd1306_send_command(0x10);		//high column start address
		ssd1306_send_data_start();
		for(n=0;n<128;n++)
		{
			ssd1306_send_byte(fill_Data);
		}
		ssd1306_send_data_stop();
	}
}



void ssd1306_char_f6x8(uint8_t x, uint8_t y, const char ch[]){
  uint8_t c,i,j=0;
  while(ch[j] != '\0')
  {
    c = ch[j] - 32;
    if (c >0) c = c - 12;
    if (c >15) c = c - 6;
    if (c>40) c=c-9;
    if(x>126)
    {
      x=0;
      y++;
    }
    ssd1306_setpos(x,y);
    ssd1306_send_data_start();
    for(i=0;i<6;i++)
    {
      ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[c*6+i]));
    }
    ssd1306_send_data_stop();
    x += 6;
    j++;
  }
}

void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
{
	uint16_t j = 0;
	uint8_t y;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		uint8_t x;
		ssd1306_setpos(x0,y);
		ssd1306_send_data_start();
		for (x = x0; x < x1; x++)
		{
			ssd1306_send_byte(~pgm_read_byte(&bitmap[j++]));
		}
		ssd1306_send_data_stop();
	}
}

uint8_t pgm_read_byte(uint8_t *p)
{
	return *p;
}
