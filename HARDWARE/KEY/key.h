#ifndef _KEY_H
#define _KEY_H

#include "sys.h"

#define KEY1 PCin(12)
#define KEY2 PAin(15)
#define KEY3 PCin(10)
#define KEY4 PCin(11)

#define KEY1_PRES	1
#define KEY2_PRES	2
#define KEY3_PRES	3
#define KEY4_PRES	4
void KEY_Init(void);
u8 KEY_Scan(u8);

#endif

