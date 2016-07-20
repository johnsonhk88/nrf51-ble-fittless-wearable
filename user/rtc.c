/*****************************************************
File: RTC function




*****************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "system.h"
#include "rtc.h"
#include "oled_096.h"

#if RTC_Func

volatile RTC_Reg RTC_Con;
unsigned char temp_buffer[8];	

app_timer_id_t                           m_rtc_update_timer_id;    

#if  Heart_Rate_Func

extern uint8_t  HRM_Run;   // 

extern uint16_t adc_result ;  // 
extern uint8_t  HRM_Update;
extern uint32_t ADC_Sample_Per_Sec;
#endif




#if BLE_Proxim | BLE_UART

void rtc_timeout_handler(void * p_context)
{
	 UNUSED_PARAMETER(p_context);
   RTC_Update();
	 RTC_Con.Soft_RTC_Update=1;



}	

void rtc_config(void)
{
 uint32_t err_code;
 RTC_Init();	
 err_code = app_timer_start(m_rtc_update_timer_id, RTC_INTERVAL, NULL);
 APP_ERROR_CHECK(err_code);


}	



#else

/** Starts the internal LFCLK XTAL oscillator
 */
void lfclk_config(void)
{
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
  {
  }
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

/** Configures the RTC with TICK for 100Hz and COMPARE0 to 10 sec
 */
void rtc_config(void)
{
  NVIC_EnableIRQ(RTC0_IRQn);                                 // Enable Interrupt for the RTC in the core
  NRF_RTC0->PRESCALER = COUNTER_PRESCALER;                   // Set prescaler to a TICK of RTC_FREQUENCY
 // NRF_RTC0->CC[0] = COMPARE_COUNTERTIME * RTC_FREQUENCY;     // Compare0 after approx COMPARE_COUNTERTIME seconds
    NRF_RTC0->CC[0] = RTC_Update_Value;
  // Enable TICK event and TICK interrupt:
#if 0 
	NRF_RTC0->EVTENSET = RTC_EVTENSET_TICK_Msk;
  NRF_RTC0->INTENSET = RTC_INTENSET_TICK_Msk;
#endif
	
  // Enable COMPARE0 event and COMPARE0 interrupt:
  NRF_RTC0->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
  NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;
}

/** Configures PIN8 and PIN9 as outputs
 */
void gpio_config(void)
{
  nrf_gpio_cfg_output(GPIO_TOGGLE_TICK_EVENT);
  nrf_gpio_cfg_output(GPIO_TOGGLE_COMPARE_EVENT);

  nrf_gpio_pin_write(GPIO_TOGGLE_TICK_EVENT, 0);
  nrf_gpio_pin_write(GPIO_TOGGLE_COMPARE_EVENT, 0);
}

/** RTC0 interrupt handler.
 * Triggered on TICK and COMPARE0 match.
 */
void RTC0_IRQHandler()
{
	/*
  if ((NRF_RTC0->EVENTS_TICK != 0) && ((NRF_RTC0->INTENSET & RTC_INTENSET_TICK_Msk) != 0))
  {
    NRF_RTC0->EVENTS_TICK = 0;
   // nrf_gpio_pin_toggle(GPIO_TOGGLE_TICK_EVENT);
		nrf_gpio_pin_toggle(LED_STOP);
  }
  */	
  if ((NRF_RTC0->EVENTS_COMPARE[0] != 0) && ((NRF_RTC0->INTENSET & RTC_INTENSET_COMPARE0_Msk) != 0))
  {
    NRF_RTC0->EVENTS_COMPARE[0] = 0;
    //nrf_gpio_pin_write(GPIO_TOGGLE_COMPARE_EVENT, 1);
		nrf_gpio_pin_toggle(LED_START);
		 //NRF_RTC0->CC[0] = (NRF_RTC0->CC[0] + (COMPARE_COUNTERTIME * RTC_FREQUENCY))%(0xffffffff); // Update 
      NRF_RTC0->CC[0] = (NRF_RTC0->CC[0] + (RTC_Update_Value))%(0xffffffff); // Update 
		        
		//  if(RTC_Con.Soft_RTC_En)
	//	{
      //if(++RTC_Con.Second_Cnt>=One_Sec)
		//	{
				//RTC_Con.Second_Cnt=0;
			    RTC_Update();
			  RTC_Con.Soft_RTC_Update=1;
				//RTC_Con.Soft_RTC_Update
		//	}	
			
			
		//}			
		
	}
}



#endif

//Calender //////////////////////////////////////////

void RTC_Init(void)
{
	RTC_Con.Second_Reg=59;
	RTC_Con.Min_Reg=59;
	RTC_Con.Hour_Reg=20;
	RTC_Con.Day_Reg=10;
	RTC_Con.Month_Reg=5;
	RTC_Con.Year_Reg=2016;
	RTC_Con.Second_Cnt=0;
	RTC_Con.Soft_RTC_Update=0;
	RTC_Con.Soft_RTC_En=1;

}	

 void RTC_Run(void)
 {
	 
	  if(RTC_Con.Soft_RTC_Update)
		 {
      RTC_Con.Soft_RTC_Update=0;
   /*		
			 UARTWrite("\n\rTime :", strlen("\n\rTime ")); 
			Convert_16bit_to_BCD_ASCII(temp_buffer, RTC_Con.Year_Reg);
     	UARTWrite(temp_buffer+1, 4); 		 
      UARTWrite("/", strlen("/")); 
			Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Month_Reg);
     	UARTWrite(temp_buffer+1, 2); 		 
      UARTWrite("/", strlen("/")); 
			Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Day_Reg);
     	UARTWrite(temp_buffer+1, 2); 		 
      UARTWrite(" ", strlen(" "));  
			Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Hour_Reg);
     	UARTWrite(temp_buffer+1, 2); 		 
      UARTWrite(":", strlen(":")); 
			Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Min_Reg);
     	UARTWrite(temp_buffer+1, 2); 		 
      UARTWrite(":", strlen(":"));  
			Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Second_Reg);
     	UARTWrite(temp_buffer+1, 2); 		 
     // UARTWrite("", strlen(":"));  
		*/	 
		printf("\n\rDate :%d/%d/%d ", RTC_Con.Year_Reg, RTC_Con.Month_Reg,RTC_Con.Day_Reg);
		printf("%d:%d:%d", RTC_Con.Hour_Reg, RTC_Con.Min_Reg, RTC_Con.Second_Reg);
		printf("\n\rADC Heart Sample rate %d per second\n\r", ADC_Sample_Per_Sec);
     ADC_Sample_Per_Sec=0; // reset ADC sample rate		
		 #if OLED096_Func
		 memset(temp_buffer, 0, 8);
		 Convert_16bit_to_BCD_ASCII(temp_buffer, RTC_Con.Year_Reg);
		 OLED_ShowString(0,32,temp_buffer+1);
		 OLED_ShowString(32,32,(uint8_t*)"/");
		 memset(temp_buffer, 0, 8);
		 Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Month_Reg);
		 OLED_ShowString(40,32,temp_buffer+1);
		 OLED_ShowString(56,32,(uint8_t*)"/");
		 memset(temp_buffer, 0, 8);
		 Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Day_Reg);
		 OLED_ShowString(64,32,temp_buffer+1);
		 //OLED_ShowString(80,32,(uint8_t*)"/");
		 memset(temp_buffer, 0, 8);
		 Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Hour_Reg);
		 OLED_ShowString(0,48,temp_buffer+1);
		 OLED_ShowString(16,48,(uint8_t*)":");
		 memset(temp_buffer, 0, 8);
		 Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Min_Reg);
		 OLED_ShowString(24,48,temp_buffer+1);
		 OLED_ShowString(40,48,(uint8_t*)":");
		 memset(temp_buffer, 0, 8);
		  Convert_8bit_to_BCD_ASCII(temp_buffer, RTC_Con.Second_Reg);
		 OLED_ShowString(48,48,temp_buffer+1);
		// OLED_ShowString(32,48,(uint8_t*)":");
		// memset(temp_buffer, 0, 8);
		 
		 
		 OLED_Refresh_Gram();
		 #endif
		
     }			 
	 
	 
 } 



void RTC_Update(void)
{
	unsigned char no_of_day=0;
	no_of_day=monthDay(RTC_Con.Month_Reg); 
	if(++RTC_Con.Second_Reg>=One_Min)
	{
		RTC_Con.Second_Reg=0;
		if(++RTC_Con.Min_Reg>=One_Hour)
		{
		  RTC_Con.Min_Reg=0;
			if(++RTC_Con.Hour_Reg>=One_Day)
			{
       RTC_Con.Hour_Reg=0;
			if(++RTC_Con.Day_Reg>=no_of_day)
			{
       RTC_Con.Day_Reg=1; // reset 
			if(++RTC_Con.Month_Reg>=One_Year)
			  {
         RTC_Con.Month_Reg=1;
				 RTC_Con.Year_Reg++;
				
			  }				
				
			}				
				
			}				
		}	
		
	}	
	
}	


unsigned char monthDay(unsigned char Month)
{
	   switch(Month)
	{
		case 1:
		case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:     return 31;

    case 4:
		case 6:
    case 9:
    case 11:     return 30;
 
    case 2:  if(((RTC_Con.Year_Reg %4 ==0) &&(RTC_Con.Year_Reg %100!=0)) || (RTC_Con.Year_Reg %400== 0))
		           {
                  return 29; // leap Year
             			
							 }
		          else
							{
								return 28;
							}	
		
		
		
		
		
	}
	
	return 30;
	
}	




void RTC_setSecond(unsigned char sec)
{
	RTC_Con.Second_Reg=sec;
	
}	


void RTC_setMin(unsigned char min)
{
	
	RTC_Con.Min_Reg=min;
}	

void RTC_setHour(unsigned char hour)
{
	
	RTC_Con.Hour_Reg=hour;
}	


void RTC_setMonth(unsigned char month)
{
	
	RTC_Con.Month_Reg=month;
}	

void RTC_setDay(unsigned char Day)
{
	
	RTC_Con.Day_Reg=Day;
}	

void RTC_setYear(unsigned int Year)
{
	
	RTC_Con.Year_Reg=Year;
}	














#endif

