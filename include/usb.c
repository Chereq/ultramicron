#include "usb.h"
#include "main.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb.h"
#include "clock.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;

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
      /*Check to see if we have data yet */
      if (Receive_length  != 0)
      {
        if (packet_sent == 1)
          CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
        Receive_length = 0;
      }
    }
}

void USB_send_gamma3_data()
{
//---------------------------------------------������� �� ����� 3------------------------------------
  unsigned char IndexHi;                //  ������� ���� �������
  unsigned char IndexLo;                // �������  ���� ������� 
  unsigned char voltIndexHi;                //  ������� ���� �������
  unsigned char voltIndexLo;                // �������  ���� ������� 
  unsigned char crc;                // ����������� �����
	unsigned int  Volt;

	Volt=ADCData.Batt_voltage;
	crc=((fon_level+Volt) & 0xff);			    //���������� ����������� �����

	IndexLo =  fon_level & 0xff;                         // ������� ������ �� ������� ����
	IndexHi = (fon_level >> 8) & 0xff;                   // ������� ������ �� ������� ����  
	voltIndexLo =  Volt & 0xff;                         // ������� ������ �� ������� ����
	voltIndexHi = (Volt >> 8) & 0xff;                   // ������� ������ �� ������� ����  
	
	Receive_Buffer[0]=0xE9;                                      // �������� ����� �����3
	Receive_Buffer[1]=IndexHi;                                   // �������� �� ����� 
	Receive_Buffer[2]=IndexLo;                                   // �������� �� ����� 
	Receive_Buffer[3]=0xE5;                                      // �������� ����� ����������
	Receive_Buffer[4]=voltIndexHi;                               // �������� �� ����� 
	Receive_Buffer[5]=voltIndexLo;                               // �������� �� ����� 
	Receive_Buffer[6]=crc;                                       // �������� ����������� �����
	Receive_Buffer[7]=0xA4;                                      // �������� ����� ��������� ��������
	
	Receive_length=8;
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

	
	impulseLo =  Detector_massive[Detector_massive_pointer] & 0xff;                         // ������� ������ �� ������� ����
	impulseHi = (Detector_massive[Detector_massive_pointer] >> 8) & 0xff;                   // ������� ������ �� ������� ����  

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
	Receive_Buffer	[5]=fonLo;                                     // �������� �� ����� 
	Receive_Buffer[6]=voltIndexHi;                               // �������� �� ����� 
	Receive_Buffer[7]=voltIndexLo;                               // �������� �� ����� 
                              	for(i=1;i<8;i++)crc+=Receive_Buffer[i]; //������ ����������� �����
	Receive_Buffer[8]=crc & 0xff;                                // �������� ����������� �����
	Receive_Buffer[9]=0xD2;                                      // �������� ����� ��������� ��������
	
	Receive_length=10;
}

void USB_off()
{
//---------------------------------------------���������� USB------------------------------------
	Power.USB_active=DISABLE;
	PowerOff();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
	set_msi_2mhz();
}


