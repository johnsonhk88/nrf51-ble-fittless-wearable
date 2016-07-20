/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
* @brief Example project on GPIO usage to drive LEDs
* @defgroup gpio_example_pca10001 GPIO example
* @{
* @ingroup nrf_examples_pca10001
*
* @brief Example of GPIO output usage.
*
* This example shows how to configure GPIO pins as outputs which can also be used to drive LEDs.
* Each LED is set on one at a time and each state lasts 100 milliseconds.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "system.h"
#include "oled_096.h"

#include "nordic_common.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_tps.h"
#include "ble_ias.h"
#include "ble_lls.h"
#include "ble_bas.h"
#include "ble_conn_params.h"
//#include "ble_eval_board_pins.h"
#include "ble_sensorsim.h"
//#include "ble_stack_handler.h"
#include "app_timer.h"
#include "ble_bondmngr.h"
#include "ble_ias_c.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "app_util.h"
#include "ble_radio_notification.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
//#include "ble_proximity.h"

/***********************************************************************
version History
Version               Date                     Describe  
ak2_v1_1_0            16-May-2016             1. porting  APPs UART sevice 
                                               1.1  (base on nRF51 SDK_5_2_0 and softDevice_s110_6_0_0_API)
                                               2. fix UART not message , when port new sdk , simple_uart.c 
																							    need to change data rate from 9600 to 115200bps
																							3. add 1second timer2 for check send data to BLE UART	for report on/off state
                                              4. add Tx BMP value to APPS UART
                                              5. add 	RTC Update handler in Apps UART timer			
                                              6. add Bluetooth Rx command decoding with different function
																							7.add UART Rx data send to BLE 
																							
																							

ak2_v1_0_9            10-May-2016             1. add ADC heart rate sensor  
                                                1.1 disable ADC battery level 
																							2. add Timer2 to generate 1ms for check Timeout ADC
                                                2.1 system timer 
																								2.2 modify ADC checking Heart rate sensor 
                                                2.3 add System running LED3 indicator	
                                              3. add Heart rate calculate alogrithm , change rate filter 16 data	
                                              4. add BPM rate in OLED																							
                                              5. add ADC filter to filter max and min rate 
																							
ak2_v1_0_8            24-July-2014						1. Integate BT Proixmity apps functon 
                                                 (base on nRF51 SDK_4_2_0 and softDevice_s110_5_2_0_API)
																								 1.1 Support Reference Manual v1.1
																								 1.2 support Product Specification v1.3
																								 1.3 S110-SDS (v1.1) specification				 
																							2. Modify the RTC function, add the timer inside
																							   the Library  "app_timer.c" 
                                                2.1 change  APP_TIMER_MAX_TIMERS     5 to  6
                                                2.2 change  APP_TIMER_OP_QUEUE_SIZE  6 to 7
																								2.3 add app_time_create() for RTC time handler (repeat)
																								2.4 add app_timer_tick(); for calucate timer interval
																								2.5 add app_timer_start(); for RTC time start
																							3. remove the power system off ,when the GAp timeout 
                                                  err_code = sd_power_system_off();			
                                              4. add BLuetooth connect status display																									
																								
																								


ak2_v1_0_7            23-July-2014						1. Change OLED Display 
                                              2. Add RTC clock display information
																							3. Add Firmware version show on display
																							4. add set RTC clock
																			         command :
																			            scsxxe =set clock second 2 digit 
																			            scmxxe =set clock mintue 2 digit
																			            schxxe = set clock hour 2 digit
																			            scdxxe = set clock day 2 digit
																			            scMxxe = set clock month 2 digit
																			            scYxxxxe = set clock year 4 digit
																						 5. add set OLED contrast
																						 command :
                                                   soie =increase the OLED contrast
                                                   sode =decrease the OLED contrast																									 

ak2_v1_0_6            15-Apr-2014             1. complete full porting from PCA1001 BT Proixmity apps(SDK_4_2_0)
                                                (only add files, without call function)  

ak2_v1_0_5						27-Feb-2014             1. add the PCA1001 BT Proixmity apps(SDK_4_2_0) : portial not completed
                                              2. create source code module , for further integration

ak2_v1_0_4            21-Feb-2014							1. add 0.96 OLED driver
                                              2. change UART baudrate to 115200bps
																							


ak2_v1_0_3            05-Feb-2014              1.add nRF51822 AKII board support (header file) 
                                               2.modify the LED blink function, can blink 5 LEDs
                                               3. add UART Rx interrupt support  
																							 4. add pin change interrupt in button (GPIOE--event) Key0, Key1
																							 5. add firmware version display
																							 6. add printf( using retarget< add putchar() to UART)
																							 

ak2_v1_0_2						01-Feb-2014							 1. using nRF51-SDK-4.2 and s110_nrf51822_5.1.0
																							 2. UART sample program from SDK->board->PCA10001-> UART 
																							 3. when an exit character 'q' or 'Q' is typed this program will 
																							 end into an infinite loop after transmitting 
                                               "EXIT!" on the new line of the terminal.
                                               This example is not just for COM ports but can be used for 
																							 any UART connection, COM port redirection is for visual verification.
                                               4. Setting the define ENABLE_LOOPBACK_TEST will assume that the 
																							 TX_PIN_NUMBER is connected to RX_PIN_NUMBER and this example is 
																							 used to test the loopback. In this case no com port can be used as the data flows
                                               from TX to RX and ERROR_PIN is set high for any loss of data.
                                               5. Configure your terminal application for 38400 bauds, 8 data bits and 1 stop bit
																							 6. modify LED function
																							 

ak2_v1_0_1            29-Jan-2014              1. inital version for nrf51822 AKII board
                                               2. using nRF51-SDK-4.2 and s110_nrf51822_5.1.0
																							 3. Blinking LED sample program from SDK->board->PCA10001-> blinkly_example
																							 
																							 
																							 









*************************************************************************/
uint8_t Firmware_Ver[]=   "V1.1.0";
uint8_t BLE_Chip_Ver[]=  "v1";

#if Heart_Rate_Func
extern uint8_t  HRM_Run;

extern uint16_t adc_result ;  // 
extern uint8_t  HRM_Update;
extern volatile int BPM;                   // used to hold the pulse rate
extern volatile uint16_t Signal;                // holds the incoming ADC raw data
extern volatile int IBI ;             // holds the time between beats, the Inter-Beat Interval in ms
extern volatile uint8_t Pulse ;     // true when pulse wave is high, false when it's low
extern volatile uint8_t QS ;        // becomes true when Arduoino finds a beat.
extern volatile int P ;                      // used to find peak in pulse wave
extern volatile int T ;                     // used to find trough in pulse wave
extern volatile int thresh ;                // used to find instant moment of heart beat
extern volatile int Pulse_amp ;                   // used to hold amplitude of pulse waveform

#endif

#if UART_Func

extern volatile uint8_t cr;
extern volatile uint8_t rx_flag;


#endif
extern RTC_Reg RTC_Con;
extern unsigned char temp_buffer[8];	

#if BLE_UART

extern  ble_nus_t  m_nus;  
extern  uint8_t states[4];
extern volatile uint8_t BT_TR_time_count;  
extern volatile uint8_t change_state;


#endif



/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


/**@brief Function for error handling, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}



/***********************************************************************
 * main() function
 * @return 0. int return type required by ANSI/ISO standard.
*************************************************************************** */
int main(void)
{
	uint32_t err_code;
//uint8_t cr=0;	
	

	
	
#if BLE_Proxim
	
	  BLE_Primx_Apps_Int();
	
#elif  BLE_UART
	
	BLE_UART_Init();
#else	
	
#if LED_Func		
	LED_init();
#endif	
	
#endif
#if  Key0_Func
   Key0_init();
#endif	
#if  Key1_Func
	 Key1_init();
#endif	

#if UART_Func
    UART_init();
	
#endif	

  
	
	
	#if OLED096_Func
  OLED_Init();			//初始化OLED      
 	OLED_ShowString(0,0, (uint8_t *)"Smart Band Demo");  
 	//OLED_ShowString(0,16, (uint8_t *)"BLE nRF51822");  
	//OLED_ShowString(0,16, (uint8_t *)"Ver:");
	 //OLED_ShowString(72,16,  Firmware_Ver);
	OLED_Refresh_Gram();	  // update the 

  #endif
	printf("\n\rSmart Watch Testing!");
	#if RTC_Func
	#if BLE_Proxim
	rtc_config();
	#elif BLE_UART
	rtc_config();
	
	#else
  RTC_Init();	
  lfclk_config();
  rtc_config();
  NRF_RTC0->TASKS_START = 1;
	RTC_Con.Soft_RTC_Update=1;
	RTC_Run();
	#endif
#endif
#if OLED096_Func 
OLED_ShowString(0,16, (uint8_t*)"BT:DIS");
OLED_Refresh_Gram();						
#endif
	
#if 	Heart_Rate_Func
	ADC_Heart_Rate_Init(ADC_Heart_RES, ADC_Heart_Input, 1);
	HRM_Run=1;
#endif	
 Timer2_init();

	while(true)
  {

/*LED test function*/
#if     0  //LED_Func		
   LED_Status_Out();
#endif		

#if UART_Func
   UART_Decode();  // Decode the 
		//simple_uart_put((uint8_t)"Test");
		// printf("\n\rFriwmare Version: ");
    // printf("%s\n\r",Firmware_Ver);
		//printf("Tesing!\n\r"); 
#endif		
		
#if 	Heart_Rate_Func
#if 0	
  if((HRM_Run==1)&&(HRM_Update==1))
	{	
	 //	adc_start();
		printf("ADC Heart Raw Data:%d\r\n",adc_result);
		//HRM_Update=0;
  }	
#endif	
	if(QS==1)
	{
	 printf("BPM is %d BPM\n\r",BPM);	
	 printf("IBI is %d ms\n\r", IBI);	
	 printf("Peak ADC value is %d \n\r", P);
	 printf("Throug ADC value is %d \n\r", T);
	 printf("Heart ADC Threshold is %d \n\r ",thresh);
	 printf("Heart ADC Pulse amplitude is %d \n\r", Pulse_amp);	
	 // display in OLED
	 memset(temp_buffer, 0, 8); // clear temp buffer
	 Convert_8bit_to_BCD_ASCII(temp_buffer, BPM);
	 OLED_ShowString(72,16,temp_buffer);
	 OLED_ShowString(104,16,(uint8_t *)"bpm");	
	 OLED_Refresh_Gram();
	 #if 	 BLE_UART
		temp_buffer[3]=' ';
		temp_buffer[4]='b';
		temp_buffer[5]='p';
		temp_buffer[6]='m';
	 err_code = ble_nus_send_string(&m_nus, temp_buffer, 8 );      //上传数据
   if (err_code != NRF_ERROR_INVALID_STATE)
   {
   APP_ERROR_CHECK(err_code);
   }
   
	 
	 #endif	
		
   QS=0;
	}		
#endif		
		
#if RTC_Func

RTC_Run();
	
#endif

#if BLE_Proxim
		// power_manage();
#elif  BLE_UART
	
	power_manage();
	
	
#endif		
	
	
		//nrf_delay_ms(50);
  }
}

/**
 *@}
 **/
