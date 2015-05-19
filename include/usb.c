#include "usb.h"
#include "main.h"
#include "delay.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb.h"
#include "keys.h"
#include "clock.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;

#pragma O0
void USB_on()
{
//---------------------------------------------��������� USB------------------------------------
	set_pll_for_usb();
	Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  SYSCFG->PMC |= (uint32_t) SYSCFG_PMC_USB_PU; // Connect internal pull-up on USB DP line
	Power.USB_active=ENABLE;
}


void USB_work()
{
//---------------------------------------------�������� ������------------------------------------
	if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();

#ifndef version_401 // ������ ����� ��������� 4.01+
			if(Settings.USB == 1) // MadOrc
#endif				
			{
			
				/*Check to see if we have data yet */
				if (Receive_length  == 1)
				{

					USB_not_active=0; // ����� ������� ������������ USB 
					
					if(Receive_Buffer[0] == 0xD4) // �������� �������� ������ � USB Gaiger
					{
						USB_send_madorc_data();
						CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
					}
					if(Receive_Buffer[0] == 0x31) // �������� ������� ������������� ����
					{
						USB_send_maxfon_data();
						CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
					}
					if(Receive_Buffer[0] == 0x32) // �������� ������� ����
					{
						USB_send_doze_data();
						CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
					}
					if(Receive_Buffer[0] == 0x33) // �������� ��������
					{
						USB_maxfon_massive_pointer=0;
						USB_doze_massive_pointer=0;
						USB_send_settings_data();
						CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
					}
					if(Receive_Buffer[0] == 0x39) // ���������� ��������
					{
						USB_maxfon_massive_pointer=0;
						USB_doze_massive_pointer=0;
					}

					Receive_length = 0;
				
				delay_ms(1);
			}
		}
// -----------------------------------------------------------------------------------------------------------------------
	}
#ifdef version_401
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) // ���� 5� �� USB �� ��������, �� ��������� ���
#else
	if ((USB_not_active>60) && (Settings.USB == 1)) // ���� 4 ������ USB �� �������, �� ��������� ���
#endif
	{
		delay_ms(100);
#ifndef version_401 
		Settings.USB=0;
#endif
		usb_deactivate(0x00);
	}

}


void USB_send_madorc_data()
{
//---------------------------------------------������� ��� MadOrc------------------------------------
  uint8_t i;
	uint8_t fonMegaHi=0;                //  ������� ���� �������
  uint8_t fonHi=0;                //  ������� ���� �������
  uint8_t fonLo=0;                // �������  ���� ������� 
  uint8_t impulseHi=0;                //  ������� ���� �������
  uint8_t impulseLo=0;                // �������  ���� ������� 
  uint8_t voltIndexHi=0;                //  ������� ���� �������
  uint8_t voltIndexLo=0;                // �������  ���� ������� 
  uint32_t crc=0;                // ����������� �����

	
	impulseLo =  madorc_impulse & 0xff;                         // ������� ������ �� ������� ����
	impulseHi = (madorc_impulse >> 8) & 0xff;                   // ������� ������ �� ������� ����  
  madorc_impulse=0;
	
  voltIndexLo =  ADCData.Batt_voltage & 0xff;                         // ������� ������ �� ������� ����
	voltIndexHi = (ADCData.Batt_voltage >> 8) & 0xff;                   // ������� ������ �� ������� ����  

	
	fonLo =  fon_level & 0xff;                                   // ������� ������ �� ������� ����
	fonHi = (fon_level >> 8) & 0xff;                             // ������� ������ �� ������� ����  
	fonMegaHi = (fon_level >> 16) & 0xff;                        // ������� ������ �� ������� ����  

	Receive_Buffer[0]=0xD1;                                      // �������� ����� Marorc
	Receive_Buffer[1]=impulseHi;                                 // �������� �� ����� 
	Receive_Buffer[2]=impulseLo;                                 // �������� �� ����� 
	Receive_Buffer[3]=fonMegaHi;                                 // �������� �� ����� 
	Receive_Buffer[4]=fonHi;                                     // �������� �� ����� 
	Receive_Buffer[5]=fonLo;                                     // �������� �� ����� 
	Receive_Buffer[6]=voltIndexHi;                               // �������� �� ����� 
	Receive_Buffer[7]=voltIndexLo;                               // �������� �� ����� 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //������ ����������� �����
	Receive_Buffer[8]=crc & 0xff;                                // �������� ����������� �����
	Receive_Buffer[9]=0xD2;                                      // �������� ����� ��������� ��������
	
	Receive_length=10;
}

void USB_send_maxfon_data()
{
//---------------------------------------------������� ��� MadOrc------------------------------------
  uint8_t i;
	uint8_t address_hi=0;
  uint8_t address_lo=0;
	uint8_t fon_1_4=0;
  uint8_t fon_2_4=0;
  uint8_t fon_3_4=0;
  uint8_t fon_4_4=0;
  uint32_t crc=0;                // ����������� �����

  address_lo =  USB_maxfon_massive_pointer       & 0xff;	
  address_hi = (USB_maxfon_massive_pointer >> 8) & 0xff;	
	
	fon_1_4 =  max_fon_massive[USB_maxfon_massive_pointer]        & 0xff;       
	fon_2_4 = (max_fon_massive[USB_maxfon_massive_pointer] >> 8)  & 0xff; 
	fon_3_4 = (max_fon_massive[USB_maxfon_massive_pointer] >> 16) & 0xff;
	fon_4_4 = (max_fon_massive[USB_maxfon_massive_pointer] >> 24) & 0xff;
	USB_maxfon_massive_pointer++;
	
	if(USB_maxfon_massive_pointer<=doze_length_week)
	{
		Receive_Buffer[0]=0xF1;                                       // �������� �����
		Receive_Buffer[1]=address_hi;                                 // �������� �� ����� 
		Receive_Buffer[2]=address_lo;                                 // �������� �� ����� 
		Receive_Buffer[3]=fon_4_4;                                    // �������� �� ����� 
		Receive_Buffer[4]=fon_3_4;                                    // �������� �� ����� 
		Receive_Buffer[5]=fon_2_4;                                    // �������� �� ����� 
		Receive_Buffer[6]=fon_1_4;                                    // �������� �� ����� 
		Receive_Buffer[7]=0xFF;                                       // �������� �� ����� 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //������ ����������� �����
		Receive_Buffer[8]=crc & 0xff;                                 // �������� ����������� �����
		Receive_Buffer[9]=0xF2;                                       // �������� ����� ��������� ��������
	
		Receive_length=10;
	}else USB_maxfon_massive_pointer=0;
}


void USB_send_doze_data()
{
//---------------------------------------------������� ��� MadOrc------------------------------------
  uint8_t i;
	uint8_t address_hi=0;
  uint8_t address_lo=0;
	uint8_t doze_1_4=0;
  uint8_t doze_2_4=0;
  uint8_t doze_3_4=0;
  uint8_t doze_4_4=0;
  uint32_t crc=0;                // ����������� �����

  address_lo =  USB_doze_massive_pointer       & 0xff;	
  address_hi = (USB_doze_massive_pointer >> 8) & 0xff;	
	
	doze_1_4 =  Doze_massive[USB_doze_massive_pointer]        & 0xff;       
	doze_2_4 = (Doze_massive[USB_doze_massive_pointer] >> 8)  & 0xff; 
	doze_3_4 = (Doze_massive[USB_doze_massive_pointer] >> 16) & 0xff;
	doze_4_4 = (Doze_massive[USB_doze_massive_pointer] >> 24) & 0xff;
	USB_doze_massive_pointer++;
	
	if(USB_doze_massive_pointer<=doze_length_week)
	{
		Receive_Buffer[0]=0xF3;                                       // �������� �����
		Receive_Buffer[1]=address_hi;                                 // �������� �� ����� 
		Receive_Buffer[2]=address_lo;                                 // �������� �� ����� 
		Receive_Buffer[3]=doze_4_4;                                    // �������� �� ����� 
		Receive_Buffer[4]=doze_3_4;                                    // �������� �� ����� 
		Receive_Buffer[5]=doze_2_4;                                    // �������� �� ����� 
		Receive_Buffer[6]=doze_1_4;                                    // �������� �� ����� 
		Receive_Buffer[7]=0xFF;                                       // �������� �� ����� 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //������ ����������� �����
		Receive_Buffer[8]=crc & 0xff;                                 // �������� ����������� �����
		Receive_Buffer[9]=0xF4;                                       // �������� ����� ��������� ��������
	
		Receive_length=10;
	}else USB_doze_massive_pointer=0;
}


void USB_send_settings_data()
{
//---------------------------------------------������� ��� MadOrc------------------------------------
  uint8_t i;
	uint8_t count_time_hi=0;
  uint8_t count_time_lo=0;
  uint32_t crc=0;                // ����������� �����

  count_time_lo =  Settings.Second_count       & 0xff;	
  count_time_hi = (Settings.Second_count >> 8) & 0xff;	
		Receive_Buffer[0]=0xF5;                                       // �������� �����
		Receive_Buffer[1]=count_time_hi;                                 // �������� �� ����� 
		Receive_Buffer[2]=count_time_lo;                                 // �������� �� ����� 
		Receive_Buffer[3]=0xFF;                                    // �������� �� ����� 
		Receive_Buffer[4]=0xFF;                                    // �������� �� ����� 
		Receive_Buffer[5]=0xFF;                                    // �������� �� ����� 
		Receive_Buffer[6]=0xFF;                                    // �������� �� ����� 
		Receive_Buffer[7]=0xFF;                                       // �������� �� ����� 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //������ ����������� �����
		Receive_Buffer[8]=crc & 0xff;                                 // �������� ����������� �����
		Receive_Buffer[9]=0xF6;                                       // �������� ����� ��������� ��������
	
		Receive_length=10;
}



void USB_off()
{
//---------------------------------------------���������� USB------------------------------------
	Power.USB_active=DISABLE;
	PowerOff();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
	set_msi(DISABLE);

	// �������� � ������� ������� ������������ (��������)
	sleep_mode(ENABLE);
	while(Power.Pump_active || Power.Sound_active);
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);    // ��������� � ���
	sleep_mode(DISABLE);
	Power.sleep_time=Settings.Sleep_time;
	Power.led_sleep_time=Settings.Sleep_time-3;

}


