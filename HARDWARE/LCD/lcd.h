#ifndef __LCD_H
#define __LCD_H		

#include "sys.h"	
#include "delay.h"
#include "stdlib.h" 

//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令  
}_lcd_dev; 	  

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
//LCD的画笔颜色和背景色	   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色

//LCD地址结构体8BIT
typedef struct
{
	u8 LCD_REG;
	u8 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((u32)(0x60000000 | 0X0FFFF))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

///////////背光控制/////////////////
#define LCD_RST PHout(0)	//复位
#define LCD_LED PCout(9)	//背光
#define	LCD_CS	PBout(14)  //片选端口  	    
#define	LCD_RS	PBout(12)  //数据/命令      	   
#define	LCD_WR	PBout(13)  //写数据			
#define	LCD_RD	PBout(15)  //读数据	
//快速刷屏
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


//写LCD寄存器
#define LCD_WR_REG(Index)	{\
	LCD_CS_CLR\
	LCD_RS_CLR\
	LCD_RD_SET\
	DATAOUT(Index|0X200)\
	LCD_WR_CLR\
	LCD_WR_CLR\
	LCD_CS_SET\
}
////写LCD数据
#define LCD_WR_DATA(Data)	{\
	LCD_CS_CLR\
	LCD_RS_SET\
	LCD_RD_SET\
	DATAOUT(Data|0X200)\
	LCD_WR_CLR\
	LCD_WR_SET\
	LCD_CS_SET\
}
//写RGB
#define LCD_WriteRAM(RGB_Code)	{\
	LCD_WR_DATA(RGB_Code>>8)\
	LCD_WR_DATA(RGB_Code&0XFF)\
}

//画笔颜色
#define WHITE         	 0XFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800//无
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //无
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

void ReadID(void);
void LCD_Init(void);
void LCD_Clear(u16 color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);//设置光标
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
