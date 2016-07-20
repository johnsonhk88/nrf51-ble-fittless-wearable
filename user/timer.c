/******************************************
Timer2 source code



********************************************/

#include "nrf51.h"
#include "nrf_gpio.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>
#include "system.h"


/////////////////////////////////////////////////////

uint32_t LED_Sys_Cnt=0;
uint32_t Timeout_1sec_Cnt=0;


#if Heart_Rate_Func
extern uint8_t  HRM_Run;
extern uint16_t adc_result ;  // 
extern uint8_t  HRM_Update;
#endif


#if BLE_UART

extern  ble_nus_t  m_nus;  
extern  uint8_t states[4];
extern volatile uint8_t BT_TR_time_count;  
extern volatile uint8_t change_state;


#endif


void Timer2_init(void)
{
	// Set timer2 in timer mode
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
	NRF_TIMER2->PRESCALER = 9;  // Timer frequency = HFCLK/2^prescaler = 16M/2^9 = 31250hz (32us)
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
  NRF_TIMER2->CC[0]       = (31U);  //=32us*31=0.000992s
  
	// set Compare[0]  interrupt enable
  NRF_TIMER2->INTENSET= (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);	
	// clear compare0 event 
	NRF_TIMER2->SHORTS      = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
  
	// Set Timer2 interrupt 
		NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_SetPriority(TIMER2_IRQn,3);
		NVIC_EnableIRQ(TIMER2_IRQn);  
	  NRF_TIMER2->TASKS_START = 1; //开启定时器
}	


void TIMER2_IRQHandler(void)
{
   // 
 	if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0)) 
   { 
 		NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event	 
	//	nrf_gpio_pin_set(GPIO_TOGGLE_PIN);           //Set LED 
		NRF_TIMER2->TASKS_CLEAR = 1;  
		Timer2_handling();
		
  } 


}	


void Timer2_handling(void)
{
	uint32_t err_code;
	// System Running LED
	LED_Sys_Cnt++;
	if(LED_Sys_Cnt>LED_Blink_Timing)
  {
	 LED_Sys_Cnt=0;	
	 nrf_gpio_pin_toggle(Sys_LED);
	}	

	#if Heart_Rate_Func
   if(HRM_Update==1)
	 {
		adc_Heart_Signal_Process();
    HRM_Update=0;
	 }		 
	if((HRM_Run==1))
	{
   adc_start();
	}	
   	
	#endif
	
	//system 1second timing
	Timeout_1sec_Cnt++;
	if(Timeout_1sec_Cnt>Timing_1sec)
	{
    Timeout_1sec_Cnt=0;
	 
 		// other 1 second function
		
		#if  BLE_UART
		// 
		BT_TR_time_count++;                                        //记时
 if((BT_TR_time_count==BT_TR_TIME)||(change_state==1))      //定时向APP上传状态值
 {
   err_code = ble_nus_send_string(&m_nus, states, 8 );      //上传数据
   if (err_code != NRF_ERROR_INVALID_STATE)
   {
   APP_ERROR_CHECK(err_code);
   }
   BT_TR_time_count=0;
	 change_state=0;
 } 
		
		
		#endif
		
		
		
	}		

}	
