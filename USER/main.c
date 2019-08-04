#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "pwm.h"
#include "rtc.h"
#include "stdio.h"
#include "infrare.h"
#include "ds18b20.h"

u8 key;
int cnt=0,cnt2=0;
int sec, sec2,min, hou;
u8 tbuf[40];
int TimeFlag,ClockFlag,TempFlag,IRFlag,MenuFlag=0;
short temperature;
int count1=0,count2=0,count3=0;	
int xt[6];//存储时间点的像素点坐标值
int yt[6];//存储温度点的像素点坐标值

void Draw_XY(void);//画坐标轴
void Clear_white(void);//清掉上一条曲线
void Temp_Control(void);//温度控制，发送红外码给子系统
void Time_Menu(void);	//菜单显示
void Time_Display(void);//电子数字表显示

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(16);
	uart_init(115200);
	KEY_Init();											//按键初始化
	PWM_Init(99,18);									//软件调制红外载波，38KHz
	DS18B20_Init();										//DS18B20初始化
	LCD_Init();											//TFT LCD初始化
	My_RTC_Init();		 								//初始化RTC
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	//配置WAKE UP中断,1秒钟中断一次
	POINT_COLOR=BLUE;									//设置画笔颜色
	while(1)
	{
		
		Temp_Control();
		key=KEY_Scan(0);
		if(key)
		{
			switch(key)
			{
				case 1:
						LCD_Clear(WHITE);
						count2=0;
						count1++;
						if(1 == count1)
						{
							
							TimeFlag = 1;
							ClockFlag = TempFlag = MenuFlag = sec = 0;
						}
						else if(2 == count1)
						{
							
							ClockFlag = 1;
							TimeFlag = MenuFlag = TempFlag = sec = 0;
						}
						else if(3 == count1)
						{
							
							MenuFlag = 1;
							TempFlag = TimeFlag = ClockFlag = count1 = 0;
						}
						break;
				case 2:
						count1=0;
						LCD_Clear(WHITE);
						count2++;
						if(1 == count2)
						{
							MenuFlag = TimeFlag = ClockFlag = sec = 0;
							TempFlag = 1;
						}
						else if(2 == count2)
						{
							TempFlag = TimeFlag = ClockFlag = count2 = 0;
							MenuFlag = 1;
						}
						break;
				case 3:
						LCD_Clear(WHITE);
						count1=count2=0;
						count3++;
						if(key==KEY3_PRES&&count3==1)
						{
							LCD_LED=1;
							MenuFlag=1;
							IRFlag=1;
							TempFlag = TimeFlag = ClockFlag = sec = 0;
							LCD_DisplayOn();
						}
						else if(key==KEY3_PRES&&count3==2)
						{
							LCD_LED=0;
							MenuFlag = TempFlag = TimeFlag = ClockFlag = 0;
							LCD_DisplayOff();
							count1=count2=count3=0;
						}
						break;
				default:break;
			}
		}
		Time_Menu();
		Time_Display();
		draw_templine();
	}
}
/*温度控制，发送红外码给子系统*/
void Temp_Control(void)
{
	u8 num;
	if(IRFlag)
	{
		LCD_ShowString(190,20,200,16,16,"  . C");
		temperature = DS18B20_Get_Temp();
		if(temperature < 300)//扩大10倍，实际温度为30度！！
		{
			num = 1;
		}
		else if(temperature>300 && temperature<350)
		{
			num = 2;
		}
		else if(temperature > 350)
		{
			num = 3;
		}
		if(temperature<0)
		{
			LCD_ShowChar(30+40,40,'-',16,0);			//显示负号
			temperature=-temperature;					//转为正数
			
		}	
		LCD_ShowNum(190,20,temperature/10,2,16);	//显示正数部分	    
		LCD_ShowNum(190+24,20,temperature%10,1,16);	//显示小数部分 	
		
		switch(num)//发送键码，对应1~9
		{
			case 1:Send_IR(0x0c);delay_ms(100);break;
			case 2:Send_IR(0x18);delay_ms(100);break;
			case 3:Send_IR(0x5e);delay_ms(100);break;
			default:break;
		}
	}
	TIM_SetCompare2(TIM3,33);	
}
/*显示菜单*/
void Time_Menu(void)
{
	if(MenuFlag)
	{
		Display_Menu();
		sec=0;
	}		
}
/*显示时间*/
void Time_Display(void)
{
	
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	uint8_t hour, minute, second;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	if(TimeFlag)
	{
		sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
		LCD_ShowString(30,100,210,24,24,tbuf);	
		sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
		LCD_ShowString(30,130,210,24,24,tbuf);	
		sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.RTC_WeekDay); 
		LCD_ShowString(30,160,210,24,24,tbuf);
	}
	if(ClockFlag)
	{
		//clock_display(RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
		hour = RTC_TimeStruct.RTC_Hours;
		minute = RTC_TimeStruct.RTC_Minutes;
		second = RTC_TimeStruct.RTC_Seconds;
		clock_display(hour,minute,second);
	}	
}
/*将上一条实时温度曲线清除*/
void Clear_white(void)
{
	int i, j;
	for(i=45;i<220;i++)
	{
		for(j=60;j<215;j++)
		{
			LCD_Fast_DrawPoint(i, j, WHITE);
		}
	}
}
/*画温度曲线*/
void draw_templine(void)
{
	int x, y;
	if(TempFlag)
	{
		Draw_XY();
		if(cnt!=6)//未采集到6个时间点的温度 （4 8 12 16 20 24）
		{
			x=45+3*sec;
			y=220-3*(temperature/10);
			if(x<=220&&y>60)
				LCD_DrawPoint(x,y);
			delay_ms(100);
			sec++;
			if(sec==60)//六秒清一次屏，画下一段
			{
				Clear_white();
				sec=0;
				sec2++;
			}
			if(sec2==10)//一分钟
			{
				min++;
				sec2=0;
			}
			if(min==60)//一小时
			{
				hou++;
				min=0;
			}
			if(hou!=0&&hou%4==0)//采集6个时间点的温度
			{
				xt[cnt]=x;
				yt[cnt]=y;
				cnt++;
			}
		}
		else//cnt=6表示已采集6个时间点的温度值，清屏后画线
		{
			Clear_white();
			LCD_DrawLine(xt[cnt2],yt[cnt2],xt[cnt2+1],yt[cnt2+1]);
			cnt2++;
			if(cnt2==4)
			{
				cnt=cnt2=0;//清零，开始下一次采集
			}
		}
	}
}
/*画温度曲线坐标轴*/
void Draw_XY(void)
{
	int i;
	LCD_DrawLine(40,220,40,60);
	LCD_DrawLine(40,220,220,220);
	LCD_ShowChar(30,40,'c',24,0);
	LCD_ShowChar(220,190,'t',24,0);
	LCD_ShowNum(40,225,0,1,16);
	for (i = 1; i <= 6; i++)
	{
			LCD_DrawLine(40+i*30,220,40+i*30,215);		//横坐标刻度
			LCD_ShowNum(40+i*30-8,225,i*10,2,16);
	}
	for (i = 1; i <= 3; i++)							//纵坐标刻度
	{
			LCD_DrawLine(40,220-i*45,45,220-i*45);	
			LCD_ShowNum(20,220-i*45-10,i*15,2,16);
	}
}


