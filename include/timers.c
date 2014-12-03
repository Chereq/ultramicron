#include "stm32l1xx_tim.h"
#include "main.h"

void sound_activate(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

#ifdef version_330 // ������ ����� � ��������������
  TIM_PrescalerConfig(TIM10,(uint32_t) (SystemCoreClock / 524250) - 1,TIM_PSCReloadMode_Immediate); // ������� ������� ~524.2 ���
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Enable); // ��������� ������ ���������
	TIM_SetAutoreload(TIM10, 65 );

#else // ������ ����� ��� �������������
  TIM_PrescalerConfig(TIM10,(uint32_t) (SystemCoreClock / 32000)  - 1,TIM_PSCReloadMode_Immediate); // ������� ������� 16 ���
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Enable); // ��������� ������ ���������
	TIM_SetAutoreload(TIM10, 4 );
#endif
	TIM10->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
	TIM2->EGR  |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
	TIM_Cmd(TIM10, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	Alarm.Tick_beep_count=0;
	Power.Sound_active=ENABLE;
}

void sound_deactivate(void)
{	

	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Disable); // ��������� ������ ���������
	TIM_Cmd(TIM10, DISABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, DISABLE);

  Power.Sound_active=DISABLE;      
	Sound_key_pressed=DISABLE;
}



void timer9_Config(void) // ��������� �������
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1; // ������������� ����� �������, ����� - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;   // ���������� - ����� �������
  TIM_OCConfig.TIM_Pulse = 1;   // ������ ����������� �������
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High; // ���������� => ����� - ��� ������� (+3.3V)

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000000) - 1; // �������� (1 ��� = 0.5���)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 280;  // ����� ���������� ����� (������������) 140��� (���� 500)
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

void timer10_Config(void) // ��������� �����
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
TIM_OCInitTypeDef TIM_OCConfig;
	
	TIM_TimeBaseStructInit(&TIM_BaseConfig);
	TIM_OCStructInit(&TIM_OCConfig);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;

#ifdef version_330 // ������ ����� � ��������������
  TIM_BaseConfig.TIM_Prescaler = (uint32_t) (SystemCoreClock / 524250) - 1; // ������� ������� ~524.2 ���
  TIM_BaseConfig.TIM_Period = 65;  // ~8 ���
  TIM_OCConfig.TIM_Pulse = 2; // ������������ ~3% (��� ����� 16.8��)
#else // ������ ����� ��� �������������
  TIM_BaseConfig.TIM_Prescaler = (uint32_t) (SystemCoreClock / 32000) - 1; // ������� ������� 32 ���
  TIM_BaseConfig.TIM_Period = 4;  // ~8 ���
  TIM_OCConfig.TIM_Pulse = 2; // ������������ ~50% 
#endif
  // ��� � ����� - �������������� ����������� �������, ���� ������ - ��������.

	TIM_DeInit(TIM10); // ��-�������������� ������ �10
  TIM_TimeBaseInit(TIM10, &TIM_BaseConfig);
  TIM_OC1Init(TIM10, &TIM_OCConfig);  // �������������� ������ ����� �������

  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);

  TIM10->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
	TIM_CCxCmd(TIM10, TIM_Channel_1, TIM_CCx_Disable); // ��������� ������ �����
  TIM_Cmd(TIM10, ENABLE);
}
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
void tim2_Config()
{
TIM_TimeBaseInitTypeDef TIM_BaseConfig;
NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 800) - 1; // �������� (1 ��� = 10��)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 1;  // ����� ���������� �����
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_TimeBaseInit(TIM2, &TIM_BaseConfig);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM2->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
  TIM_Cmd(TIM2, ENABLE);
	
}
