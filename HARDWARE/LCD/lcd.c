#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "picture.h"
#include "usart.h"
#include "delay.h"	 
  
extern int TimeFlag,ClockFlag,TempFlag;

u8 lcd_id[12]; //存放LCD ID字符串
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 
  
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;  


//TFT LCD 初始化
//LCD_RST	LCD_RS	LCD_WR	LCD_CS	LCD_RD	LCD_LED		DB-~DB7
//  PH0		PB12	PB13	PB14	PB15	PC9			PC0~PC7
void LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOH, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//LCD_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//LCD_RS,LCD_WR,LCD_CS,LCD_RD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;//DB0~DB7	LCD_LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	LCD_RST_CLR//LCD复位
	delay_ms(100);
	LCD_RST_SET
	
	lcddev.width=240;    //LCD 宽度
	lcddev.height=320;   //LCD 高度
	lcddev.setxcmd = 0X2A;//设置x坐标指令
	lcddev.setycmd = 0X2B;//设置y坐标指令
	lcddev.wramcmd = 0X2C;//开始写gram指令
	
	delay_ms(120);
	LCD_WR_REG(0x11)//sleep out
	delay_ms(120);
	
	LCD_WR_REG(0x36)//Memory Data Access Control
	LCD_WR_DATA(0x00)
	
	LCD_WR_REG(0x3A)//Interface Pixel Format
	LCD_WR_DATA(0x55)
	
	LCD_WR_REG(0x21)//Display Inversion On
	
	LCD_WR_REG(0xB2)//Porch Setting
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x00)
	LCD_WR_DATA(0x33)
	LCD_WR_DATA(0x33) 
	
	LCD_WR_REG(0xB7)//Gate Control
	LCD_WR_DATA(0x75)

	LCD_WR_REG(0xBB)//VCOMS Setting
	LCD_WR_DATA(0x22)

	LCD_WR_REG(0xC0)//LCM Control
	LCD_WR_DATA(0x2C)
	
	LCD_WR_REG(0xC2)//VDV and VRH Command Enable
	LCD_WR_DATA(0x01)
	
	LCD_WR_REG(0xC3)//VRH Set
	LCD_WR_DATA(0x13)
	
	LCD_WR_REG(0xC4)//VDV Set
	LCD_WR_DATA(0x20)
	
	LCD_WR_REG(0xC6)//Frame Rate Control in Normal Mode
	LCD_WR_DATA(0x09)//0x0F:60Hz	0x09:75Hz
	
	LCD_WR_REG(0xD0)//Power Control 1
	LCD_WR_DATA(0xA4)
	LCD_WR_DATA(0xA1)
	
	LCD_WR_REG(0xE0)//Positive Voltage Gamma Control
	LCD_WR_DATA(0xD0)
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x0A)
	LCD_WR_DATA(0x09)
	LCD_WR_DATA(0x08)
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x2E)
	LCD_WR_DATA(0x44)
	LCD_WR_DATA(0x45)
	LCD_WR_DATA(0x0F)
	LCD_WR_DATA(0x17)
	LCD_WR_DATA(0x16)
	LCD_WR_DATA(0x2B)
	LCD_WR_DATA(0x33)
	
	LCD_WR_REG(0xE1)//Negative Voltage Gamma Control
	LCD_WR_DATA(0xD0)
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x0A)
	LCD_WR_DATA(0x09)
	LCD_WR_DATA(0x08)
	LCD_WR_DATA(0x05)
	LCD_WR_DATA(0x2E)
	LCD_WR_DATA(0x43)
	LCD_WR_DATA(0x45)
	LCD_WR_DATA(0x0F)
	LCD_WR_DATA(0x16)
	LCD_WR_DATA(0x16)
	LCD_WR_DATA(0x2B)
	LCD_WR_DATA(0x33)
	
	LCD_LED=0;
	
	//LCD_Clear(WHITE);
	//LCD_WR_REG(0x29)//Display On
}

//LCD显示区域设置
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	LCD_WR_REG(0x2A)
	LCD_WR_DATA(Xstart>>8)
	LCD_WR_DATA(Xstart&0xFF)
	LCD_WR_DATA(Xend>>8)
	LCD_WR_DATA(Xend&0xFF)

	LCD_WR_REG(0x2B)
	LCD_WR_DATA(Ystart>>8)
	LCD_WR_DATA(Ystart&0xFF)
	LCD_WR_DATA(Yend>>8)
	LCD_WR_DATA(Yend&0xFF)
	
	LCD_WR_REG(0x2C)
}
void ReadID(void)
{
	u8 data;
	LCD_RST_CLR
	delay_ms(100);
	LCD_RST_SET
	LCD_WR_REG(0X04)
	LCD_CS_SET
	LCD_RS_SET
	LCD_WR_SET
	LCD_RD_CLR
	LCD_CS_CLR
	LCD_RD_SET
	data=GPIOC->IDR&0XFF;
	lcddev.id=data;
	lcddev.id=data;
	lcddev.id=data;
	lcddev.id<<=8;
	lcddev.id=data;
	sprintf((char*)lcd_id,"LCD ID:0X%X",lcddev.id);
}
//LCD清屏
void LCD_Clear(u16 color)
{
	u32 index=0;     
	BlockWrite(0,240,0,320);
	for(index=0;index<76800;index++)
	{
		LCD_WriteRAM(color)
	}	
} 
//LCD开启显示
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29)
}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   	
	LCD_WR_REG(0X28)
}
//LCD设置光标位置 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_WR_REG(0X2A)
	LCD_WR_DATA(Xpos>>8)
	LCD_WR_DATA(Xpos&0XFF)

	LCD_WR_REG(0X2B)
	LCD_WR_DATA(Ypos>>8)
	LCD_WR_DATA(Ypos&0XFF)

	LCD_WR_REG(0x2C)
}
/*******************************************************************************
	LCD显示字符串
	x,y:起点坐标
	width,height:区域大小  
	size:字体大小
	*p:字符串起始地址		
	说明：字符是有点组成的，故在字符函数中不断调用画点函数LCD_DrawPoint();
*******************************************************************************/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
/*******************************************************************************
	LCD在指定位置显示一个字符   YZ
	x,y:起始坐标
	num:要显示的字符:" "--->"~"
	size:字体大小 12/16/24
	mode:叠加方式(1)还是非叠加方式(0)
	解释：
	1、字符宽度(size/2)＝高度(size)一半。
	2、字符取模为竖向取模，即每列占几个字节，最后不够完整字节数的占一字节。
	3、字符所占空间为：每列所占字节＊列数。
	csize=(size/8+((size%8)?1:0))*(size/2)
	乘号*前为计算每列所占字节数，乘号*后为列数（字符高度一半）
*******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{	
	u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)  //８个点
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;       ////刷下一行 ，注意：PC2LCD2002取模方式设置：阴码+逐列式+顺向+C51格式　20180419
			if(y>=320)return;		//超区域了
			if((y-y0)==size)   //刷满一个字符高度时20180419
			{
				y=y0;  //回到第一行20180419
				x++;   //开刷下一列20180419
				if(x>=240)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}
//LCD快速画点
//x,y:坐标
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);	
	LCD_WriteRAM(color)
}
//LCD画点
//x,y:坐标
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM(POINT_COLOR)
}
//LCD画线
//x1,y1:起点坐标
//x2,y2:终点坐标 
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//m^n函数  YZ
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/*******************************************************************************
	LCD显示数字,高位为0,则不显示  YZ
	x,y :起点坐标	 
	len :数字的位数
	size:字体大小
	color:颜色 
	num:数值(0~4294967295);
*******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;   //LCD_Pow:m^n函数
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
/*******************************************************************************
	LCD显示数字,高位为0,还是显示  YZ
	x,y:起点坐标
	num:数值(0~999999999);	 
	len:长度(即要显示的位数)
	size:字体大小
	mode:
	[7]:0,不填充;1,填充0.
	[6:1]:保留
	[0]:0,非叠加显示;1,叠加显示.
*******************************************************************************/
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;	
	//printf("num = %d\n",num);
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//LCD在指定位置画一个指定大小的圆  
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
//LCD画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}



void Display_Menu(void)
{
	u16 temp,pos,t; 
	BlockWrite((lcddev.width-240)/2,(lcddev.width+240)/2,(lcddev.height-320)/2,(lcddev.height+320)/2);
	for (pos=0;pos<320;pos++) // (pos=0;pos<lcddev.height;pos++)//
	{
		for(t=0;t<240*2;t++) //(t=0;t<lcddev.width*2;t++) 
		{
			temp=gImage_menu[pos*240*2+t]*256+gImage_menu[pos*240*2+t+1];
	
			t++;                                
			LCD_WriteRAM(temp)         
		}
	}
}
