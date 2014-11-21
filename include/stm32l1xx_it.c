#include "stm32l1xx_it.h"
#include "main.h"
#include "timers.h"
#include "dac.h"
#include "comp.h"
#include "hw_config.h"
#include "stm32_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb.h"


uint16_t tmpCC1[2] = {0, 0};
extern __IO uint32_t CaptureNumber, PeriodValue;
uint32_t IC1ReadValue1 = 0, IC1ReadValue2 =0;

void NMI_Handler(void)
{
}

  void HardFault_Handler(void)
{
while (1)
  {
}
}
    
/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
void SVC_Handler(void)
{
}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{
}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{
}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
void SysTick_Handler(void)
{
  extern uint32_t msTicks;
  msTicks++;		// ��������� �������� �������
}
/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/

/**
* @brief  This function handles PPP interrupt request.
* @param  None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/

/**
* @}
*/ 

void check_wakeup_keys()
{
	if ((!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) && !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)) || Power.Display_active)
	{
		tim10_sound_activate();
    Power.sleep_time=Settings.Sleep_time;
		Power.led_sleep_time=Settings.Led_Sleep_time;	
	}
	
}

// =======================================================
// ���������� �� ������� ������ 0
void EXTI3_IRQHandler(void)
{
  if (fullstop==1){EXTI_ClearITPendingBit(EXTI_Line3);return;}
  
  
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
    if(Alarm.Alarm_active && !Alarm.User_cancel)
    {
      Alarm.User_cancel=ENABLE;
    }
    else
    {
      if(Power.Display_active)
      {
				key|=0x1; // ������ ��nu
      }
    }
		Sound_key_pressed=ENABLE;
		check_wakeup_keys();
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}

// =======================================================
// ���������� �� ������� ������ 1
void EXTI4_IRQHandler(void)
{
  if (fullstop==1){EXTI_ClearITPendingBit(EXTI_Line4);return;}
  
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
    
    
    if(Power.Display_active)
    {      
      key|=0x4;   // ������ -
    }    
		Sound_key_pressed=ENABLE;
		check_wakeup_keys();
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}

// =======================================================
// ���������� �� �������� �� �������� 1 � ������ 2
void EXTI9_5_IRQHandler(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	extern __IO uint8_t Receive_Buffer[64];
  extern __IO  uint32_t Receive_length ;
  extern __IO  uint32_t length ;

  
	if (fullstop==1){EXTI_ClearITPendingBit(EXTI_Line8);EXTI_ClearITPendingBit(EXTI_Line6);return;}
  
  
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
		EXTI_ClearITPendingBit(EXTI_Line8);
		
		Detector_massive[Detector_massive_pointer]++;  // ��������� ��������� ������� � ��������  
		Doze_massive[0]++;	           					// ���������� ��������� ������� ����

		if(Power.Pump_active==DISABLE)
    {
			dac_on(); // �������� ���
			comp_on();              // �������� ����������
			EXTI_StructInit(&EXTI_InitStructure);
			EXTI_InitStructure.EXTI_Line = EXTI_Line22;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
			
			Power.Pump_active=ENABLE;
			TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
		  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
			TIM9->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
			TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Enable); // ��������� �������	
			EXTI_ClearITPendingBit(EXTI_Line22);
	
			
		}
    if(Settings.Sound && !(Alarm.Alarm_active && !Alarm.User_cancel))
    {
      if(Power.Display_active)
      {
        tim10_sound_activate();
      }
    }
  }
  
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    
    EXTI_ClearITPendingBit(EXTI_Line6);
    if(Power.Display_active)
    {
        key|=0x2; // ������ +
    }
		Sound_key_pressed=ENABLE;
		check_wakeup_keys();
  }
  
}


// =======================================================
// ���������� ��� ����� ��������� ������ �������� �������
void TIM9_IRQHandler(void)
{
	if (fullstop==1){TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);TIM_ClearITPendingBit(TIM9, TIM_IT_Update);return;}
  if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
     if(TIM9->CCER & (TIM_CCx_Enable << TIM_Channel_1))
			 {
				 current_pulse_count++;
				 pump_counter_avg_impulse_by_1sec[0]++;
			 }
  }
  if (TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET)
  {
      TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
  }
  
}

// ========================================================
// ????
void TIM10_IRQHandler(void)
{
  
	if (fullstop==1){TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);TIM_ClearITPendingBit(TIM10, TIM_IT_Update);return;}
  if (TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    if(Alarm.Alarm_active)
    {
      if(!Alarm.User_cancel)
      {        
        Alarm.Alarm_beep_count++;
        if(Alarm.Alarm_beep_count==500)  TIM_SetCompare1(TIM10, 1 );
        if(Alarm.Alarm_beep_count==1000){TIM_SetCompare1(TIM10, 2 );Alarm.Alarm_beep_count=0;}
      }
      else
      {
				if(Power.Sound_active==ENABLE)
				{	
					if(Sound_key_pressed)
					{
						if(Alarm.Tick_beep_count>1500)
							{
								Alarm.Tick_beep_count=0;
								tim10_sound_deactivate();
							} else Alarm.Tick_beep_count++;
							
					} else if(Alarm.Tick_beep_count>50)
							{
								Alarm.Tick_beep_count=0;
								tim10_sound_deactivate();
							} else Alarm.Tick_beep_count++;
				}
			}     
    }else
    {
		 if(Power.Sound_active==ENABLE)
		 {	
			if(Sound_key_pressed)
			{
				 if(Alarm.Tick_beep_count>1500)
							{
								Alarm.Tick_beep_count=0;
								tim10_sound_deactivate();
							} else Alarm.Tick_beep_count++;
							
			} else if(Alarm.Tick_beep_count>50)
							{
								Alarm.Tick_beep_count=0;
								tim10_sound_deactivate();
							} else Alarm.Tick_beep_count++;
		 }
		}
  }

  if (TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
  {
      TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
  }
}


////////////////////////////////////////
// ��������� ���
////////////////////////////////////////

void recalculate_fon()
{
	int i,pointer;
	int massive_len=Settings.Second_count>>2; // 50
	int recalc_len=massive_len/auto_speedup_factor; // 5.55
	float tmp;
	
	fon_level=0;				  
					
	for(i=0;i<recalc_len;i++)
	{
		if(Detector_massive_pointer>=i)
		{
			pointer=Detector_massive_pointer-i;
		}else
		{
			pointer=massive_len-(i-Detector_massive_pointer);
		}
		fon_level+=Detector_massive[pointer];
	}
	tmp=fon_level;
	tmp=tmp/recalc_len;
	tmp=tmp*(massive_len/auto_speedup_factor);
	tmp=tmp*auto_speedup_factor;
	fon_level=tmp;
}

void RTC_Alarm_IRQHandler(void) {
		int i;
		if (fullstop==1){RTC_ClearITPendingBit(RTC_IT_ALRA);EXTI_ClearITPendingBit(EXTI_Line17);return;}
		
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) {

        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
			
		// ������� ������� ��� ���������� ���������� ��� (������ 4 ������)
		if(DataUpdate.Batt_update_time_counter>300) 
    {
      DataUpdate.Need_batt_voltage_update=ENABLE;
      DataUpdate.Batt_update_time_counter=0;
    } else DataUpdate.Batt_update_time_counter++;
    
		// ������� ������� ��� ���������� �������� ��������� �������
    if(DataUpdate.pump_counter_update_time>59) 
    {
			if(Power.USB_active)USB_send_gamma3_data(); // ������ �������� ������ ����� USB ���� �� �������
      pump_counter_avg_impulse_by_1sec[1]=pump_counter_avg_impulse_by_1sec[0];
      pump_counter_avg_impulse_by_1sec[0]=0;
      DataUpdate.pump_counter_update_time=0;
    } else DataUpdate.pump_counter_update_time++;

		// ����� ������� ����
		if(DataUpdate.doze_sec_count>=600)
		{
			Doze_count=0;
			Doze_hour_count=0;
			Max_fon=0;
			for(i=doze_length;i>0;i--)
			{
				Doze_massive[i]=Doze_massive[i-1];
				max_fon_massive[i]=max_fon_massive[i-1];
				if(max_fon_massive[i]>Max_fon)Max_fon=max_fon_massive[i];
				Doze_count+=Doze_massive[i];
				if(i<7)Doze_hour_count+=Doze_massive[i];
			}
			Doze_massive[0]=0;
			max_fon_massive[0]=0;
			DataUpdate.doze_sec_count=0;
		} else DataUpdate.doze_sec_count++;
		////////////////////////////////////////////////////

		////////////////////////////////////////////////////    
		if(second_divide>=0x04)
		{
			second_divide=0x00;

			if(Detector_massive[Detector_massive_pointer]>9)
			{	
				if(Detector_massive[Detector_massive_pointer]>199) // ������� �� 9 ��� ���� ����� 10 000
				{ 
					if(auto_speedup_factor!=10)auto_speedup_factor=9;
				} else
				{
					if(Detector_massive[Detector_massive_pointer]>99) // ������� �� 5 ��� ���� ����� 5 000
					{ 
						if(auto_speedup_factor!=5)auto_speedup_factor=5;
					} else
					{
						if(Detector_massive[Detector_massive_pointer]>19) // ������� �� 3 ��� ���� ����� 1 000
						{ 
							if(auto_speedup_factor!=3)auto_speedup_factor=3;
						} else
						{ // ������� �� 2 ��� ���� ����� 500
							if(auto_speedup_factor!=2)auto_speedup_factor=2;
						}
					}
				}
				
				if(auto_speedup_factor!=1)recalculate_fon(); // �������� ����, ���� ������������� ���������
				
			} else
			{ // ���� ��������� �� ���������
				if(auto_speedup_factor!=1){auto_speedup_factor=1;recalculate_fon();}
				else
				{	fon_level+=Detector_massive[Detector_massive_pointer];}

			}

			if(Power.USB_active)USB_send_madorc_data();
			Detector_massive_pointer++;
			if(Detector_massive_pointer>=(Settings.Second_count>>2))	
			{
				if(auto_speedup_factor==1)fon_level-=Detector_massive[0];
				Detector_massive[0]=0;
				Detector_massive_pointer=0;
			}else
			{
				if(auto_speedup_factor==1)fon_level-=Detector_massive[Detector_massive_pointer];
				Detector_massive[Detector_massive_pointer]=0;
			}
			if(fon_level>max_fon_massive[0])max_fon_massive[0]=fon_level; // ���������� ������� ������������� ����
			
			DataUpdate.Need_fon_update=ENABLE;
		} else second_divide++;
		
		////////////////////////////////////////////////////
		
//		DataUpdate.Need_display_update=ENABLE;
    if(Power.sleep_time>0)Power.sleep_time--;
		if(Power.led_sleep_time>0)Power.led_sleep_time--;
 
    }
}

////////////////////////////////////////

void RTC_WKUP_IRQHandler (void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line20);
	if (fullstop==1){RTC_ClearITPendingBit(RTC_IT_WUT);return;}
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
		RTC_ClearITPendingBit(RTC_IT_WUT);
		if(!Power.Pump_active)
		{
			dac_on(); // �������� ���
			comp_on();              // �������� ����������
			EXTI_StructInit(&EXTI_InitStructure);
			EXTI_InitStructure.EXTI_Line = EXTI_Line22;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);

			Power.Pump_active=ENABLE;
			TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
		  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
			TIM9->EGR |= 0x0001;  // ������������� ��� UG ��� ��������������� ������ ��������
			TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Enable); // ��������� �������
			EXTI_ClearITPendingBit(EXTI_Line22);
		}
  } 
}
/////////////////////////////////////////////////////////////////////////////////////////
void COMP_IRQHandler(void)
{
 EXTI_InitTypeDef EXTI_InitStructure;
 int i;	
	extern uint16_t current_pulse_count;
	if (fullstop==1){   EXTI_ClearITPendingBit(EXTI_Line22);return;}
	
  if(EXTI_GetITStatus(EXTI_Line22) != RESET)
  {
   EXTI_ClearITPendingBit(EXTI_Line22);
   // ������������ ������ ������ ���������, ��������� �������
	 {
			TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable); // ��������� �������
		  TIM_ITConfig(TIM9, TIM_IT_Update, DISABLE);
			TIM_ITConfig(TIM9, TIM_IT_CC1, DISABLE);
			Power.Pump_active=DISABLE;
			comp_off();              // ��������� ����������
			EXTI_StructInit(&EXTI_InitStructure);
			EXTI_InitStructure.EXTI_Line = EXTI_Line22;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_LineCmd = DISABLE;
			EXTI_Init(&EXTI_InitStructure);
			EXTI_ClearITPendingBit(EXTI_Line22);

			dac_off(); // ��������� ���
			i=RTC->WUTR;
			if(current_pulse_count>20) // ���� ���������� ������� ��������, ����� �������� �������
			{
				i=0x01E; // = 15 ��
			} else
			{
				if(current_pulse_count>2) // ���� ���������� �� ���������� �������� �������
				{
					if(i>0x352) // ���� ������ 400�� + 15��
					{
						i-=0x333  ;  // - 400 ��
					} else 
					{
						if(i>0x01E)
						{
							i-=0x01E;  // - 15 ��
						} else 
						{
							i=0x01E;  // - 15 ��
						}
					}
				} else 
				{
					if(i>0x198)
					{
						if(i<0x500B)i+=0x333; // + 400 �� (�� ���� ������)
					} else
					{
						i+=0x01E;  // + 15 ��
					}
				}
			}    
			while(RTC_WakeUpCmd(DISABLE)!=SUCCESS);
  	  RTC_SetWakeUpCounter(i); 			// ���������� ������� ����������
			current_pulse_count=0;
			while(RTC_WakeUpCmd(ENABLE)!=SUCCESS);
		}
  }
}

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_IRQHandler(void)
{
  USB_Istr();
}

/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USB_FS_WKUP_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line18);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
