#ifndef _KEY_H_
#define _KEY_H_
#include "main.h"
void key_proc(void);
void lcd_proc(void);
extern uint8_t parark[2];
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void calculate(void);
extern uint8_t mode_flag;
extern uint8_t oldmode_flag;
#endif
