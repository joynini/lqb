#include "key.h"
#include "led.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "tim.h"
#include "adc.h"
uint8_t key_value;
uint8_t key_up;
uint8_t key_down;
uint8_t key_old;
uint8_t view;

uint8_t parark[2]={1,1};//0:r 1:k
uint8_t parark_flag=0;
uint8_t lock_flag=0;

__IO uint32_t uwTickkey;
__IO uint32_t uwTicklongkey;

__IO uint32_t uwTickfrq;
__IO uint32_t uwTickfrq1;
__IO uint32_t uwTicklock;
float gadc;
uint8_t mode;//低频0，高频1

uint8_t mode_flag;
uint8_t oldmode_flag;

uint8_t finish_flag = 0;
uint8_t N;//次数
//adc采样
//输入捕获

//uint32_t cr_val;
//uint32_t frq;

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//if(htim->Instance==TIM3)
//{
// cr_val=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);
//	__HAL_TIM_SetCounter(htim,0);
// frq=(80000000/80)/cr_val;
// HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);
//}

//}



 
float ccrl_val1a,ccrl_val1b = 0;
uint32_t frq1,frq2 = 0;
float duty1,duty2 = 0;
//??????????
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)			
{
	if(htim->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)//??????,????????
		{
			ccrl_val1a = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2);	//??	
			ccrl_val1b = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);	//??
		
			__HAL_TIM_SetCounter(htim,0);		    //?????
			frq1 = (80000000/80)/ccrl_val1a;		//????
			duty1 = (ccrl_val1b/ccrl_val1a) * 100;  //?????
			HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);	//??????????(????)
			HAL_TIM_IC_Start(htim,TIM_CHANNEL_2);	//??????????
		
		}
	}
}





double get_adc(void)
{
		uint16_t adc=0;
		HAL_ADC_Start(&hadc2);
		adc=HAL_ADC_GetValue(&hadc2);
	return adc*3.3/4096;
}

float duty37;
float v7=251.2;
uint8_t counter_change=200;

float compere=20;
void calculate()
{
 if(uwTick-uwTickfrq<100)return;
 uwTickfrq=uwTick;
	
		 static int i=0;
	
		 gadc=get_adc();
if(lock_flag==0)
{ if(gadc<1)
			duty37=10;
		 else if(gadc>1&&gadc<3)
			duty37=37.5*gadc-27.5;
		 else if(gadc>3)
			 duty37=85;
	 }
		  compere=duty37/100*counter_change;
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, compere);
		 char text[40];

 if(oldmode_flag==0&&mode_flag==1&&finish_flag == 0)//低频到高频；
	 { 
		 i++;
		if(i<41) 
		{	counter_change=80000000/(4000+100*i)/100;
				__HAL_TIM_SET_AUTORELOAD(&htim2, (counter_change-1));
			v7=(4000+100*i)*2*3.14*parark[0]/100/parark[1];
  
		}
		if(i>=41)
		{
			i=0;
			finish_flag = 1;
		v7=(8000)*2*3.14*parark[0]/100/parark[1];
		}
			
   }
 if(oldmode_flag==1&&mode_flag==0&&finish_flag == 0)//高频到低频
 { 
	 
	  i++;
		if(i<41) 
		{	counter_change=80000000/(8000-100*i)/100;
 	__HAL_TIM_SET_AUTORELOAD(&htim2, counter_change);
		//	v7=(8000-100*i)*2*3.14*parark[0]/100/parark[1];
		}
		if(i>=41)
		{
			i=0;
			finish_flag = 1;
	  // v7=(4000)*2*3.14*parark[0]/100/parark[1];
		}
		

		 sprintf(text,"     V= %.1f      ",v7);
  LCD_DisplayStringLine(Line5, (unsigned char *)text);
  } 
}



uint8_t key_scan(void)
{
	uint8_t keyval;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	keyval=1;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	keyval=2;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
	keyval=3;
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
	keyval=4;

  return keyval;

}

void key_proc(void)
{
  if(uwTick-uwTickkey<100) return;
	uwTickkey=uwTick;
	
	key_value= key_scan();
	key_down=	key_value&(key_value^key_old);
	key_up=	~key_value&(key_value^key_old);
	key_old=key_value;
	//旧的模式，高频低频
	
  if(oldmode_flag!=mode_flag&&uwTick-uwTicklock>5000)
	{ oldmode_flag=mode_flag;

	}
	
	if(key_down)
	{ uwTicklongkey= uwTick;
	}
		if(uwTick-uwTicklongkey<800)
		{
			switch(key_up)
			{   
				case 1:
				view++;
				LCD_Clear(Black);
				if(view>2)view=0;
			
				break;
				case 2:
					//led_dsp(0x02);
				if(view==0)
					{//锁
						if(oldmode_flag!=mode_flag)//如果按下了b2切换模式
						{ 
							if(uwTick-uwTicklock<5000)return;	 
//							oldmode_flag=mode_flag;
//							uwTicklock=uwTick;
						
						}
						 else if(oldmode_flag==mode_flag)//如果没有在切换模式，则切换模式
						{ 
						  mode_flag=mode_flag^1;
							uwTicklock=uwTick;
							finish_flag=0;
							N++;
						}
					}
				if(view==1)
				{	
				parark_flag=1^parark_flag;
				
				}
				break;
				case 3:
					if(view==1)
					{	parark[parark_flag]++;
				  if(parark[parark_flag]>10)parark[parark_flag]=1;
					//led_dsp(0x04);
					}
				break;
				case 4:
				if(view==0&&lock_flag==1)	
				{	lock_flag=0;
					led_dsp(0x01);
			  }
				if(view==1)	
				{	if(parark[parark_flag]>0)
					{
					parark[parark_flag]--;
					}
				if(parark[parark_flag]<1)parark[parark_flag]=10;
			  }
				
				led_dsp(0x08);
				
				break;

			}
		}
		else
		{
			switch(key_value)
			{
				case 2:
					//led_dsp(0xff);
			  break;
				case 4:
					if(view==0)
				{ lock_flag=1;
				//锁定占空比；
			
				}
			}
		
		}




}


void lcd_proc()
{char text[40];
	char a='L';
	if(oldmode_flag==1)
	a='H';
	if(oldmode_flag==0)
	a='L';
//	char duty;
//	float v;
if (view==0&&oldmode_flag==mode_flag&&lock_flag!=1)
{ sprintf(text,"        DATA        ");
  LCD_DisplayStringLine(Line1, (unsigned char *)text);
  sprintf(text,"     M= %c       ",a);
  LCD_DisplayStringLine(Line3, (unsigned char *)text);
  sprintf(text,"     P= %d %%       ",(int)duty37);
  LCD_DisplayStringLine(Line4, (unsigned char *)text);
  sprintf(text,"     V= %.1f      ",v7);
  LCD_DisplayStringLine(Line5, (unsigned char *)text);
  led_dsp(0x01);
}
if (view==1)
{sprintf(text,"        PARA        ");
  LCD_DisplayStringLine(Line1, (unsigned char *)text);
  sprintf(text,"     R= %d       ", parark[0]);
  LCD_DisplayStringLine(Line3, (unsigned char *)text);
  sprintf(text,"     K= %d        ",parark[1]);
  LCD_DisplayStringLine(Line4, (unsigned char *)text);
}
float ML;
float MH;
ML=4000*2*3.14*parark[0]/100/parark[1];
MH=8000*2*3.14*parark[0]/100/parark[1];
if (view==2)
{sprintf(text,"        RECD        ");
  LCD_DisplayStringLine(Line1, (unsigned char *)text);
  sprintf(text,"     N= %d       ",N);
  LCD_DisplayStringLine(Line3, (unsigned char *)text);
  sprintf(text,"     MH= %.1f        ",MH);
  LCD_DisplayStringLine(Line4, (unsigned char *)text);
  sprintf(text,"     ML= %.1f      ",ML);
 LCD_DisplayStringLine(Line5, (unsigned char *)text);
}
if(view==0&&oldmode_flag!=mode_flag)
	led_buling();

if(lock_flag==1)
{
 led_dsp(0x04);
}
}