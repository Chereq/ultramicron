/*
 * Usb_Geiger.c
 *
 * Created: 13.08.2013 18:29:28
 *  Author: orc
 */ 

#define F_CPU 12000000UL

//#define SBM19 // ���� ���������� - ������ - ���-19
//#define IMP_REPEATER // ���� ���������� - �������� ����������� �� PC0

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   /* ����� ��� usbdrv.h */
#include "usbdrv.h"
#include "therm_ds18b20.c"
#include "therm_ds18b20.h"


#define LED_R_OFF PORTD &= ~_BV(4);
#define LED_R_ON PORTD |= _BV(4);

#ifdef IMP_REPEATER
#define LED_IMP_OFF PORTC &= ~_BV(0);
#define LED_IMP_ON PORTC |= _BV(0);
#endif

struct dataexchange_t
{
  uchar  h_byte_imp;
  uchar  l_byte_imp;
  uchar  pingback_u;
  uchar  byte2_fon;
  uchar  byte1_fon;
  uchar  byte0_fon;
  uchar  count_interval_u;
  uchar  count_validate_u;
  uchar  temp;
  uchar  tempsign;
} pdata;




PROGMEM char usbHidReportDescriptor[22] = {    /* ���������� ������� USB */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, sizeof(struct dataexchange_t),              //   REPORT_COUNT (8)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};
/* ��������� �� ������ ������ ���� feature-������, �� �� ���������� �������������� 
 *  report-ID (������� ������ ���� � ������ ����� �������). ���� ������ ������� �� 128
 *  opaque ���� ������.
 */

/* ��� ���������� ������ ������ ������� �������� */
static uchar    currentAddress;
static uchar    bytesRemaining;

struct dataexchange_t pdata; // � ��� ���� ������



volatile uchar clkr=0;
unsigned char blink=0, second, k;
volatile unsigned long impulses=0, rad, fon;
volatile unsigned int sbm[74], sbmf[11];
uchar count_interval, count_validate, pingback, temp_l, measure, measureres;

uchar Temperature = 255, szero = 1; //-255
uchar nSensors;

	int temp = 999;

uint8_t  j;

ISR(INT1_vect) // �������� � ������� 0
{
blink=1;
sbm[0]++;

#ifdef IMP_REPEATER
LED_IMP_ON;
_delay_us(5);
LED_IMP_OFF;
_delay_us(185); // ������� ����� ���20 �� ����� �����, ������ ������������ �� ����� ��
#else
_delay_us(190); // ������� ����� ���20 �� ����� �����, ������ ������������ �� ����� ��
#endif

}


ISR(TIMER0_OVF_vect)
{
	uchar kc;
if (second++>5) {second=0; measure=1;
}
if (second==4) {measureres=1;}

if (clkr++ == 46) // 1 �� � 12 ���, ������ - �����
 {
    for(k=73;k>0;k--)
    {
	sbm[k]=sbm[k-1];
	}
    sbm[0]=0;

if (count_validate>0) count_validate--;


    for(k=10;k>0;k--)
    {
	sbmf[k]=sbmf[k-1];
	}
    sbmf[0]=0;
	
if ((count_validate==0)&(sbmf[2] > sbmf[8])) if ((sbmf[2] - sbmf[8])>20) count_validate=count_interval; //���� ������� ���� >20 - ��������
if ((count_validate==0)&(sbmf[2] < sbmf[8])) if ((sbmf[8] - sbmf[2])>20) count_validate=count_interval; //���� ������� ���� >20 - ��������


rad=0;
// ���ٸ� ����
   // ������ � ������� ��������� ��� 1 ������� 
	for(kc=1;kc<73;kc++) // � sbm[0] ������a� ����, �� �����.
{
    rad=rad+sbm[kc];

    switch(kc) // �������
    {
    case 2:
        if(rad>555) // ��� ������ 10 000 ���/� �������� ������� 2 ���
        {
            rad*=18;
            kc=73;
			if (count_interval!=2) count_validate=2;
            count_interval=2;
        }
        break;

    case 4:
        if(rad>666) // ��� ������ 6 000 ���/� �������� ������� 4 ���
        {
            rad*=9;
            kc=73;
			if (count_interval!=4) count_validate=4;
            count_interval=4;
        }
        break;

    case 9:
        if(rad>375) // ��� ������ 1 500 ���/� �������� ������� 9 ���
        {
            rad*=4;
            kc=73;
			if (count_interval!=9) count_validate=9;
            count_interval=9;
			
        }
        break;

    case 12:
        if(rad>266) // ��� ������ 800 ���/� �������� ������� 12 ���
        {
            rad*=3;
            kc=73;
			if (count_interval!=12) count_validate=12;
            count_interval=12;
			
        }
        break;
    
    case 18:
        if(rad>150) // ��� ������ 300 ���/� �������� ������� 18 ���
        {
            rad*=2;
            kc=73;
            if (count_interval!=18) count_validate=18;
			count_interval=18;
			
        }
        break;

    case 36:
        if(rad>50) // ��� ������ 50 ���/� �������� ������� 36 ���
        {
            // rad *1
            kc=73;
			if (count_interval!=36) count_validate=36;
            count_interval=36;
			
        }
        break;
    case 72:
			rad/=2;
			if (count_interval!=72) count_validate=72;
            count_interval=72; // ����������. ����� ������.
        break;

    }
}
// ������ �������
#ifdef SBM19
rad=rad/4;
#endif


fon=rad;
sbmf[0]=rad;


//	 blink=1;
	 clkr=0;
 }

}
	



uchar   usbFunctionRead(uchar *data, uchar len)
{
    uint8_t i;

impulses=sbm[1];

	pdata.pingback_u=pdata.l_byte_imp+1;

	pdata.h_byte_imp=impulses >> 8;
	pdata.l_byte_imp=impulses;
	
	//fon=453;
	
	pdata.byte2_fon = fon >> 16;
	pdata.byte1_fon = fon >> 8;
	pdata.byte0_fon = fon;
	pdata.count_interval_u=count_interval;
	pdata.count_validate_u=count_validate;
	pdata.temp = Temperature;
	pdata.tempsign = szero;
	 
	impulses=0;
 

    if(len > bytesRemaining)
        len = bytesRemaining;
    uchar *buffer=(uchar*)&pdata;
    for(i=0;i < len;i++)
	  data[i]=buffer[i+currentAddress];
    currentAddress += len;
    bytesRemaining -= len;
    return len;

}

/* usbFunctionWrite() ���������� ����� ���� ���������� ������ ������ � ����������
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{
    uint8_t i;

 
    if(bytesRemaining == 0)
        return 1;               /* ����� �������� */
    if(len > bytesRemaining)
        len = bytesRemaining;
    uchar *buffer=(uchar*)&pdata;
    for(i=0;i < len;i++)
	  buffer[i+currentAddress]=data[i];
    currentAddress += len;
    bytesRemaining -= len;
    if(bytesRemaining == 0) /* ��� ������ �������� */
    {
 } 
    return bytesRemaining == 0; /* 0 ��������, ��� ���� ��� ������ */
}


/* ------------------------------------------------------------------------- */


usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID ���������� */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  
            bytesRemaining = sizeof(struct dataexchange_t);
            currentAddress = 0;
            return USB_NO_MSG;  
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            bytesRemaining = sizeof(struct dataexchange_t);
            currentAddress = 0;
            return USB_NO_MSG; 
        }
    }else{
        /* ��������� ������� �� ������ ���������� */
    }
    return 0;
}
/* ------------------------------------------------------------------------- */

int main(void)
{

//    DDRC |= _BV(DDC0) | _BV(DDC1) | _BV(DDC2) | _BV(DDC5);
cli();
// ��������� �������
DDRD  |=  (1 << DDD4);
PORTD |=  (0 << PD4);

// ������ DS18B20 � ���������
DDRB |= (0 << PB3);
PORTB |= (1 << PB3);

// ������ ��� � ���������
DDRD |= (0 << PD3);
PORTD |= (1 << PD3);

      TCCR0|=(1<<CS00)|(1<<CS02); // ����������� CLK/1024
	   
	  MCUCR |= (0 << ISC11)|(0 << ISC10); //|(0 << ISC21)|(0 << ISC20);
	  GICR |= (1 << INT1); //|(1 << INT2);


    usbInit();
    usbDeviceDisconnect();  // ������������� ����������� �� �����, ��� ������ ����� ������ ��� ����������� �����������!

    _delay_ms(250); // ����� > 250 ms

    usbDeviceConnect();     // ������������

      TIMSK|=(1<<TOIE0); //  ��������� ���������� �� ������������ �2.



	  LED_R_OFF;


    sei();               // ��������� ����������

    while(1){
		if (blink)
		{
		blink=0;
		LED_R_ON;
        _delay_ms(6);
		LED_R_OFF;
		}



								
				if (measureres) 
				{
				measureres=0;

				}

         usbPoll();       // ��� ������� ���� ��������� �������� � ������� �����, ������������ �������� ����� �������� - 50 ms

/* 1wire � ������������ ���� �� ����, �� � ��� � ���
				if (measure) 
				{
					cli();
				therm_read_temperature2(&szero, &Temperature);
				sei();
				measure=0;
				}
*/				

    }

    return 0;
}
/* ------------------------------------------------------------------------- */
