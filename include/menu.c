#include <stdio.h>
#include <string.h>
#include "main.h"
#include "menu.h"
#include "keys.h"
#include "ext2760.h"


MenuItem Menu_list[max_struct_index] = {
  
  //������   �����          ���� �������� 0   ���� 1          ���� ������ ��� 1  ������ ����� ���� ��������                             �������  ��������  ������   ������� �� ����������     �� ���������� 
  {  0x00, "�������",	      "����",							"",		          	"%u���",	     &Settings.Alarm_level,                                  0,       10000,    60,      &plus_alarm,              &minus_alarm},
  {  0x00, "���",	    		  "����",							"",			        	"%u���",	     &Settings.Sleep_time,                                   10,      230,      30,      &plus_ten,              &minus_ten},
  {  0x00, "����",	        "����",							"���",	        	"",	           &Settings.Sound,                                        0,       1,        0,       &plus_on,                 &minus_off},
	{  0x00, "����� USB",     "����",    		  		"���", 		       	"",            &Settings.USB,                                          0x00,    0x00,     0x00,    &usb_activate,            &usb_deactivate},
  {  0x00, "����� ����",    "*",   							"*",		        	"*",	         0x00,                                                   0x00,    0x00,     0x00,    &plus_doze_reset,         0x00},
  {  0x00, "������������",  "*",   							"*",		        	"*",	         0x00,                                                   0x00,    0x00,     0x00,    &plus_reboot,             0x00},
  {  0x01, "��������",	    "",		  						"",			        	"%u",  	       &Settings.contrast,                                     0,       15,       0,       &plus_one,                &minus_one},
  {  0x01, "������",	      "����",							"",			        	"%u",	         &Settings.Display_reverse,                              0,       3,        0,       &plus_one,                &minus_one},
  {  0x01, "����",	        "",		  						"",			        	"%u���",	     &Settings.Second_count,                                 200,     450,      200,     &plus_ten,                &minus_ten},
	{  0x01, "LSI",		        "�����",						"",			        	"%u ��",	     &Settings.LSI_freq,                            	       26000,   56000,    38000,   &plus_500,                &minus_500}
//  {  0x00, "���������",		  "����",							"",			        	"%u���",	     &Settings.Led_Sleep_time,                               0,       300,      30,      &plus_sleep,              &minus_sleep},
//  {  0x01, "����",	        "",		  						"",			        	"%u���",	     &Settings.Sound_freq,                                   1,       10,       8,       &plus_one,                &minus_one}
//	{  0x01, "����������",	  "",		  						"",			        	"%uV",	       &Settings.Geiger_voltage,                               300,     450,      380,     &plus_ten,                &minus_ten},
//	{  0x01, "��������",	    "",		  						"",			        	"%u���",	     &Settings.Pump_Energy,                                  150,     450,      250,     &plus_50,                 &minus_50},

/*  {  0x01, "�����������",	  "���",  						"����",		       	"",	           &Settings.Power_comp,                                   0,       1,        0,       &plus_one,                &minus_one}
  {  0x01, "���",	    		  "%u",		  					"%u",			       	"%u",			     &RTC_Time.Hour,                        						     0,       23,       0,       &plus_one,                &minus_one},
  {  0x01, "������",	 		  "%u",								"%u",			       	"%u",		  	   &RTC_Time.Minute,                     								   0,       59,       0,       &plus_one,                &minus_one},
  {  0x01, "�������",	      "%u",								"%u",			       	"%u",		    	 &RTC_Time.Second,                              				 0,       59,       0,       &plus_one,                &minus_one},
  {  0x01, "����",	   		  "%u",		  					"%u",			       	"%u",			     &RTC_Time.Day,                          							   1,       31,       0,       &plus_one,                &minus_one},
  {  0x01, "�����",	 			  "%u",								"%u",			       	"%u",		  	   &RTC_Time.Month,                          							 1,       12,       0,       &plus_one,                &minus_one},
  {  0x01, "���",	   		  	"%u",								"%u",			       	"%u",		    	 &RTC_Time.Year,                            			       2014,    2050,     0,       &plus_one,                &minus_one}
  {  0x01, "����. ��/x",	  "",									"",			        	".%01u%%",	   &Settings.Geiger_angle_of_counter_characteristics,      1,       9,        5,       &plus_one,                &minus_one},
  {  0x01, "����. ��",	    "",									"",			        	"%u%%",	       &Settings.HV_ADC_Corr,                                  1,       99,       42,      &plus_one,                &minus_one},
  {  0x01, "�� ���/����",	  "",									"",			        	"%u���",	     &Settings.pump_pulse_by_impulse,                        1,       40,       5,       &plus_one,                &minus_one},
  {  0x00, "����������",    "*",		  					"*",		        	"*",	         0x00,                                                   0x00,    0x00,     0x00,    &plus_poweroff,           0x00},
  {  0x01, "�� �����.",	    "",									"",			        	"%u",	         &Settings.pump_skvagennost,                             1,       50,       8,       &plus_one,                &minus_one}
*/
};
//////////////////////////////////////////////////////////////////////////////////////////////////



void main_screen()
{
  
  uint16_t battery_procent=0;
   
  //������ �������� ��������� 3.2� = 0% 4.2� = 100%
  battery_procent=ADCData.Batt_voltage;
  battery_procent-=3200;
  battery_procent/=10;
  if(ADCData.Batt_voltage<3200){LcdBatt(82, 19, 82+10, 19+19, 0);}//������ ���������ADCData.Batt_voltage
  else LcdBatt(84, 19, 84+10, 19+19, battery_procent);//������ ���������ADCData.Batt_voltage

	if (main_menu_stat>5)main_menu_stat=1;
	if (main_menu_stat<1)main_menu_stat=5;
	switch (main_menu_stat)
	{
		case 0x01:
			sprintf (lcd_buf, "�������� ����"); // ����� � ����� �������� ��������
			LcdString(1,4); // // ������� ������� ������� ���������� ������

			sprintf (lcd_buf, "%9u ���", Max_fon); // ����� � ����� �������� ��������
			LcdString(1,5); // // ������� ������� ������� ���������� ������
      break;
	// -----------------------------------------
		case 0x02:
			sprintf (lcd_buf, "���� �� 10���"); // ����� � ����� �������� ��������
			LcdString(1,4); // // ������� ������� ������� ���������� ������
	
			if(Doze_massive[1]>0)
			{
				//��� �� ��� massive/(3600/����� �����)
				sprintf (lcd_buf, "%9u ���", (Doze_massive[1]*(Settings.Second_count>>2))/900); // ����� � ����� �������� ��������
			} else {
				sprintf (lcd_buf, " *������* ���"); // ����� � ����� �������� ��������
			}
			LcdString(1,5); // // ������� ������� ������� ���������� ������
      break;
		// -----------------------------------------
		case 0x03:
			sprintf (lcd_buf, "���� �� ���"); // ����� � ����� �������� ��������
			LcdString(1,4); // // ������� ������� ������� ���������� ������
		
			if(Doze_massive[6]>0)
			{
				sprintf (lcd_buf, "%9u ���", (Doze_hour_count*(Settings.Second_count>>2))/900); // ����� � ����� �������� ��������
			} else {
				sprintf (lcd_buf, " *������* ���"); // ����� � ����� �������� ��������
			}
			LcdString(1,5); // // ������� ������� ������� ���������� ������
      break;
  // -----------------------------------------
		case 0x04:
			sprintf (lcd_buf, "���� �� �����"); // ����� � ����� �������� ��������
			LcdString(1,4); // // ������� ������� ������� ���������� ������

			if(Doze_massive[doze_length_day]>0)
			{
				sprintf (lcd_buf, "%9u ���", (Doze_day_count*(Settings.Second_count>>2))/900); // ����� � ����� �������� ��������
			} else {
				sprintf (lcd_buf, " *������* ���"); // ����� � ����� �������� ��������
			}

			LcdString(1,5); // // ������� ������� ������� ���������� ������
      break;
  // -----------------------------------------
		case 0x05:
			sprintf (lcd_buf, "���� �� ������"); // ����� � ����� �������� ��������
			LcdString(1,4); // // ������� ������� ������� ���������� ������

			if(Doze_massive[doze_length_week]>0)
			{
				sprintf (lcd_buf, "%9u ���", (Doze_week_count*(Settings.Second_count>>2))/900); // ����� � ����� �������� ��������
			} else {
				sprintf (lcd_buf, " *������* ���"); // ����� � ����� �������� ��������
			}

			LcdString(1,5); // // ������� ������� ������� ���������� ������
      break;
  // -----------------------------------------

    default: 
			break;
	}

	
    Draw_fon_digit(1, 1, 0);
    Draw_fon_graph(2, 94, 67-25, 67);

	if(auto_speedup_factor>1)
	{
		sprintf (lcd_buf, "x%1u", auto_speedup_factor); // ����� � ����� �������� ��������
		LcdString(12,3); // // ������� ������� ������� ���������� ������
	}
    LcdUpdate(); // ���������� ������ �� ��������������� �����-������ �� �������
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////
void menu_screen()
{
  char para_string[20];
  char tmp_string[20];
  uint16_t menu_page,i,j; 
  //���������� ����
  
  sprintf (lcd_buf, "      ����      ");
  LcdStringInv(1,1);
  
  if(menu_select==0)
  {
    menu_page=0;
  }else{

#ifdef hidden_menu
    menu_page=(menu_select-1)/(max_string_count-start_offset);// ����������� �������� ���� 
#else
		menu_page=0;
#endif
  }
	
  for(i=0;i<(max_string_count-start_offset);i++)
  {
    uint8_t fill_len=0; 
    uint8_t para_len=0; 
    uint8_t text_len=0; 
    uint8_t menu_struct_index=0; 
    
    
    menu_struct_index=(menu_page*(max_string_count-start_offset))+i; // ���������� ����� � ���������
    if (menu_struct_index>=max_struct_index)break; // ���� ���� ���������
    
    
    // ����������� ������� ������, ����� ������� ����� �������� ��������� ������� �� ������ ������ � ���������� (�������� �� ������� ����)
    
    // ��������� �������� ��������� "10���"
    switch(*Menu_list[menu_struct_index].Parameter_value)
    {
    case 0:  // ���� �������� ��������� ����� ����, ���� ��� �� ���������������� �� ���� ������
      if(Menu_list[menu_struct_index].Param_is_0[0] != '\0')
      { sprintf (para_string,  Menu_list[menu_struct_index].Param_is_0); break;}
      
    case 1:  // ���� �������� ��������� ����� �������, ���� ��� �� ���������������� �� ���� ������
      if(Menu_list[menu_struct_index].Param_is_1[0] != '\0')
      { sprintf (para_string,  Menu_list[menu_struct_index].Param_is_1);  break;}
      
    default:  // �� ���� ��������� ������� ������� �� �������
      sprintf (para_string,  Menu_list[menu_struct_index].Param_is_N, *Menu_list[menu_struct_index].Parameter_value); 
      break;
    }
    
    para_len=strlen(para_string);                  // ������ ���������
    text_len=strlen(Menu_list[menu_struct_index].Text);            // ����� ������
    fill_len=max_string_len - para_len - text_len; // ������� ��������� ������ ��������
    
    sprintf (tmp_string, Menu_list[menu_struct_index].Text);             // ����� ����� �������             "���"
    for(j=0;j<fill_len;j++) strcat(tmp_string, " "); // �������� ��������� ����������   "���      "
    
    // ����� �� �����
    // ���� ������ �� ������, �� ������������. �� ���� �� ����� � ����� ����, �� ������������ ������ ��������
    sprintf (lcd_buf,  tmp_string); // ������� � ������ �� ����� "���      "
    if(menu_select==menu_struct_index+1 && enter_menu_item==DISABLE) // ����������� ���������
    {LcdStringInv(1,i+start_offset+1);} 
    else
    {LcdString(1,i+start_offset+1);}
    
    sprintf (lcd_buf,  para_string); // ������� � ������ �� �������� "10 ���"
    if(menu_select==menu_struct_index+1)               // ����������� ���������
    {LcdStringInv(1+text_len+fill_len,i+start_offset+1);} 
    else
    {LcdString(1+text_len+fill_len,i+start_offset+1);}
    
  }
  
  LcdUpdate();
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////
void stat_screen()
{
  switch(stat_screen_number)
  {
  case 0: 
		sprintf (lcd_buf, "   ����������   ");
		LcdStringInv(1,1);
  
		sprintf(lcd_buf, "����������"); // ������� �� �������
		LcdString(1,2); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "���  |+3V  |��"); // ������� �� �������
		LcdString(1,3); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "%1i.%02i",ADCData.Batt_voltage/1000,(ADCData.Batt_voltage%1000)/10); // ������� �� �������
		LcdString(1,4); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "|%1i.%02i",ADCData.Power_voltage/1000,(ADCData.Power_voltage%1000)/10); // ������� �� �������
		LcdString(6,4); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "|%3i",Settings.Geiger_voltage); // ������� �� �������
		LcdString(12,4); // // ������� ������� ������� ���������� ������ �� ������ 8

		sprintf(lcd_buf, "�������"); // ������� �� �������
		LcdString(1,6); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "���/� | ��������"); // ������� �� �������
		LcdString(1,7); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		if(pump_counter_avg_impulse_by_1sec[1]==0){sprintf(lcd_buf, "������");} // ������� �� �������
		else                                       sprintf(lcd_buf, "%5i ",pump_counter_avg_impulse_by_1sec[1]); // ������� �� �������
		LcdString(1,8); // // ������� ������� ������� ���������� ������ �� ������ 8
  
		sprintf(lcd_buf, "%3i���",(TIM9->CCR1*ADCData.Batt_voltage)/176); // ������� �� �������
		LcdString(10,8); // // ������� ������� ������� ���������� ������ �� ������ 8
		break;

#ifdef debug
	case 2:

		sprintf (lcd_buf, "�����  %5i", Wakeup.total_wakeup); 	 	LcdString(1,1);
		sprintf (lcd_buf, "RTC    %5i", Wakeup.rtc_wakeup);   		LcdString(1,2);
		sprintf (lcd_buf, "tim9   %5i", Wakeup.tim9_wakeup); 	 	  LcdString(1,3);
		sprintf (lcd_buf, "���.   %5i", Wakeup.pump_wakeup); 			LcdString(1,4);
		sprintf (lcd_buf, "COMP2  %5i", Wakeup.comp_wakeup); 			LcdString(1,5);
//		sprintf (lcd_buf, "tim10  %5i", Wakeup.tim10_wakeup); 		LcdString(1,6);
		sprintf (lcd_buf, "����� %6i",  debug_wutr/2); 	        	LcdString(1,7);
		sprintf (lcd_buf, "������ %5i", Wakeup.sensor_wakeup);  		LcdString(1,8);
		break;
#endif
	
	case 1:

		sprintf (lcd_buf, "  � ���������   ");
		LcdStringInv(1,1);
  
		sprintf (lcd_buf, "  ������-������ ");
		LcdString(1,3);
  	sprintf (lcd_buf, "   (c)Shodan    ");
		LcdString(1,4);
		sprintf (lcd_buf, " �.����  2014�.");
		LcdString(1,5);
		sprintf (lcd_buf, " �����������.net");
		LcdString(1,6);
	 sprintf (lcd_buf, " FW Build:");
	 LcdString(1,7);
	 sprintf (lcd_buf, "     %s",__DATE__);
	 LcdString(1,8);
		break;
	
	default:
		stat_screen_number=0;
		break;
  }
	LcdUpdate();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
