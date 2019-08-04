#include "infrare.h"

void Signal_Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);  //Ê¹ÄÜTIM3
}

void Signal_Start(void)
{
	TIM_Cmd(TIM3, ENABLE);  //Ê§ÄÜTIM3
}


void Send_IR(char CustomCode)
{
	int i, j;
	char SysCode=0x00;
	char IR_Buff[4];
	
	IR_Buff[0] = SysCode;
	IR_Buff[1] = SysCode;
	IR_Buff[2] = CustomCode;
	IR_Buff[3] = ~CustomCode;
	
	Signal_Start();
	delay_ms(9);
	//delay_us(9000);
	Signal_Stop();
	delay_ms(4);
	delay_us(500);
	//delay_us(4500);
	
	for(i = 1;i <= 4;i++)
	{
		for(j = 1;j <= 8;j++)
		{
			Signal_Stop();
			delay_us(560);
			if(IR_Buff[i]&0x01)
			{
				Signal_Start();
				delay_us(565);
			}
			else
			{
				Signal_Start();
				delay_ms(1);
				delay_us(690);
				//delay_us(1690);
			} 
			IR_Buff[i] = IR_Buff[i]>>1;
		}
	}
	Signal_Start();
	delay_us(560);
	Signal_Stop();
}





