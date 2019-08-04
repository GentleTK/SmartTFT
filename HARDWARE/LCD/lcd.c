#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "picture.h"
#include "usart.h"
#include "delay.h"	 
  
extern int TimeFlag,ClockFlag,TempFlag;

u8 lcd_id[12]; //���LCD ID�ַ���
//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 
  
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;  


//TFT LCD ��ʼ��
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

	LCD_RST_CLR//LCD��λ
	delay_ms(100);
	LCD_RST_SET
	
	lcddev.width=240;    //LCD ���
	lcddev.height=320;   //LCD �߶�
	lcddev.setxcmd = 0X2A;//����x����ָ��
	lcddev.setycmd = 0X2B;//����y����ָ��
	lcddev.wramcmd = 0X2C;//��ʼдgramָ��
	
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

//LCD��ʾ��������
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
//LCD����
void LCD_Clear(u16 color)
{
	u32 index=0;     
	BlockWrite(0,240,0,320);
	for(index=0;index<76800;index++)
	{
		LCD_WriteRAM(color)
	}	
} 
//LCD������ʾ
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29)
}	 
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   	
	LCD_WR_REG(0X28)
}
//LCD���ù��λ�� 
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
	LCD��ʾ�ַ���
	x,y:�������
	width,height:�����С  
	size:�����С
	*p:�ַ�����ʼ��ַ		
	˵�����ַ����е���ɵģ������ַ������в��ϵ��û��㺯��LCD_DrawPoint();
*******************************************************************************/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
/*******************************************************************************
	LCD��ָ��λ����ʾһ���ַ�   YZ
	x,y:��ʼ����
	num:Ҫ��ʾ���ַ�:" "--->"~"
	size:�����С 12/16/24
	mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
	���ͣ�
	1���ַ����(size/2)���߶�(size)һ�롣
	2���ַ�ȡģΪ����ȡģ����ÿ��ռ�����ֽڣ���󲻹������ֽ�����ռһ�ֽڡ�
	3���ַ���ռ�ռ�Ϊ��ÿ����ռ�ֽڣ�������
	csize=(size/8+((size%8)?1:0))*(size/2)
	�˺�*ǰΪ����ÿ����ռ�ֽ������˺�*��Ϊ�������ַ��߶�һ�룩
*******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{	
	u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)  //������
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;       ////ˢ��һ�� ��ע�⣺PC2LCD2002ȡģ��ʽ���ã�����+����ʽ+˳��+C51��ʽ��20180419
			if(y>=320)return;		//��������
			if((y-y0)==size)   //ˢ��һ���ַ��߶�ʱ20180419
			{
				y=y0;  //�ص���һ��20180419
				x++;   //��ˢ��һ��20180419
				if(x>=240)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}
//LCD���ٻ���
//x,y:����
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);	
	LCD_WriteRAM(color)
}
//LCD����
//x,y:����
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM(POINT_COLOR)
}
//LCD����
//x1,y1:�������
//x2,y2:�յ����� 
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
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
//m^n����  YZ
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/*******************************************************************************
	LCD��ʾ����,��λΪ0,����ʾ  YZ
	x,y :�������	 
	len :���ֵ�λ��
	size:�����С
	color:��ɫ 
	num:��ֵ(0~4294967295);
*******************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;   //LCD_Pow:m^n����
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
	LCD��ʾ����,��λΪ0,������ʾ  YZ
	x,y:�������
	num:��ֵ(0~999999999);	 
	len:����(��Ҫ��ʾ��λ��)
	size:�����С
	mode:
	[7]:0,�����;1,���0.
	[6:1]:����
	[0]:0,�ǵ�����ʾ;1,������ʾ.
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
//LCD��ָ��λ�û�һ��ָ����С��Բ  
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 
//LCD������	  
//(x1,y1),(x2,y2):���εĶԽ�����
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
