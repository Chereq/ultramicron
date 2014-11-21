#include "stm32l1xx_tim.h"
#include "main.h"

void tim10_sound_activate(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	TIM_Cmd(TIM10, ENABLE);
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Enable); // ��������� �������
	TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
	TIM_SetCompare1(TIM10, 2 );

	Power.Sound_active=ENABLE;
}

void tim10_sound_deactivate(void)
{	
			TIM_ITConfig(TIM10, TIM_IT_Update, DISABLE);
			TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Disable); // ��������� �������    
			TIM_Cmd(TIM10, DISABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, DISABLE);

      Power.Sound_active=DISABLE;      
			Sound_key_pressed=DISABLE;
}



void timer9_Config(void)
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1; // ������������� ����� �������, ����� - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;   // ���������� - ����� �������
  TIM_OCConfig.TIM_Pulse = 1;   // ������ ����������� �������
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High; // ���������� => ����� - ��� ������� (+3.3V)

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000000) - 1; // �������� (1 ��� = 1���)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 500;  // ����� ���������� ����� (������������)
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up; // ������ �� ���� �� TIM_Period
  
  
  TIM_DeInit(TIM9); // ��-�������������� ������ �9
  TIM_TimeBaseInit(TIM9, &TIM_BaseConfig);
  TIM_OC1Init(TIM9, &TIM_OCConfig);  // �������������� ������ ����� ������� �9 (� HD ��� PB13)

  // ��� � ����� - �������������� ����������� �������, ���� ������ - ��������.
  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);

  TIM9->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
  TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable); // ��������� �������
  TIM_Cmd(TIM9, ENABLE);
}

void timer10_Config(void)
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	
  TIM_BaseConfig.TIM_Prescaler = (uint32_t) (SystemCoreClock / (Settings.Sound_freq*4000)) - 1;
  TIM_BaseConfig.TIM_Period = 3;
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCConfig.TIM_Pulse = 2;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_DeInit(TIM10); // ��-�������������� ������ �9
  TIM_TimeBaseInit(TIM10, &TIM_BaseConfig);
  TIM_OC1Init(TIM10, &TIM_OCConfig);  // �������������� ������ ����� ������� �9 (� HD ��� PB13)


  // ��� � ����� - �������������� ����������� �������, ���� ������ - ��������.
  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);

  TIM10->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Disable); // ��������� �������
  TIM_Cmd(TIM10, ENABLE);
}
