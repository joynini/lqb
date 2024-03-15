#include "led.h"
#include "key.h"
__IO uint32_t uwTickbuling;
void led_dsp(uint8_t uled)
{
HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOC, uled<<8,GPIO_PIN_RESET);
HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);


}
static int j=0;

void led_buling(void)
{
	if(oldmode_flag!=mode_flag)
	{
		for(j=0;j<10;j++)
		{
		if(uwTick-uwTickbuling<500)return;
		uwTickbuling=uwTick;
		static int i=0;
		i++;
		if (i%2)led_dsp(0x02);
		else led_dsp(0x00);
		}
	}


}









