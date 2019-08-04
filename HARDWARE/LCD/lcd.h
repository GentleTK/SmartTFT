#ifndef __LCD_H
#define __LCD_H		

#include "sys.h"	
#include "delay.h"
#include "stdlib.h" 

//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	wramcmd;		//��ʼдgramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��  
}_lcd_dev; 	  

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

//LCD��ַ�ṹ��8BIT
typedef struct
{
	u8 LCD_REG;
	u8 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((u32)(0x60000000 | 0X0FFFF))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

///////////�������/////////////////
#define LCD_RST PHout(0)	//��λ
#define LCD_LED PCout(9)	//����
#define	LCD_CS	PBout(14)  //Ƭѡ�˿�  	    
#define	LCD_RS	PBout(12)  //����/����      	   
#define	LCD_WR	PBout(13)  //д����			
#define	LCD_RD	PBout(15)  //������	
//����ˢ��
#define LCD_RST_SET  GPIOH->BSRRL=GPIO_Pin_0;
#define LCD_RST_CLR  GPIOH->BSRRH=GPIO_Pin_0;
#define LCD_CS_SET   GPIOB->BSRRL=GPIO_Pin_14;
#define LCD_CS_CLR   GPIOB->BSRRH=GPIO_Pin_14;
#define LCD_RS_SET   GPIOB->BSRRL=GPIO_Pin_12;
#define LCD_RS_CLR   GPIOB->BSRRH=GPIO_Pin_12;
#define LCD_WR_SET   GPIOB->BSRRL=GPIO_Pin_13;
#define LCD_WR_CLR   GPIOB->BSRRH=GPIO_Pin_13;
#define LCD_RD_SET   GPIOB->BSRRL=GPIO_Pin_15;
#define LCD_RD_CLR   GPIOB->BSRRH=GPIO_Pin_15;
#define DATAOUT(x)   GPIOC->ODR=x;


//дLCD�Ĵ���
#define LCD_WR_REG(Index)	{\
	LCD_CS_CLR\
	LCD_RS_CLR\
	LCD_RD_SET\
	DATAOUT(Index|0X200)\
	LCD_WR_CLR\
	LCD_WR_CLR\
	LCD_CS_SET\
}
////дLCD����
#define LCD_WR_DATA(Data)	{\
	LCD_CS_CLR\
	LCD_RS_SET\
	LCD_RD_SET\
	DATAOUT(Data|0X200)\
	LCD_WR_CLR\
	LCD_WR_SET\
	LCD_CS_SET\
}
//дRGB
#define LCD_WriteRAM(RGB_Code)	{\
	LCD_WR_DATA(RGB_Code>>8)\
	LCD_WR_DATA(RGB_Code&0XFF)\
}

//������ɫ
#define WHITE         	 0XFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800//��
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

void ReadID(void);
void LCD_Init(void);
void LCD_Clear(u16 color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);//���ù��
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);

u32 LCD_Pow(u8 m,u8 n);
void LCD_DrawPoint(u16 x,u16 y);
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);

void draw_templine(void);
void Display_Menu(void);
#endif
