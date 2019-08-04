#ifndef __INFRARE_H
#define __INFRARE_H

#include "sys.h"
#include "delay.h"
#include "usart.h"

void Signal_Stop(void);
void Signal_Start(void);

void Send_IR(char CustomCode);

#endif

