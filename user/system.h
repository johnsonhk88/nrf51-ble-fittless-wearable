/******************************************************************



******************************************************************/
#ifndef         __SYSTEM_H
#define         __SYSTEM_H




#define      LED_Func			       1
#define      UART_Func           1 
#define      UART_Func_Int       1  // interrupt
#define      Key0_Func           0
#define      Key1_Func           0
#define      RTC_Func            1  // RTC function
#define      OLED096_Func        1
#define      BLE_Proxim          0
#define      BLE_UART            1
#define      UART_TO_BLE         0  // UART Rx data sent to BLE
#define      ADC_BAT_LV_Func     0   // 
#define      Heart_Rate_Func     1   // ADC heart rate 



#if  UART_Func
#include "simple_uart.h"
#endif
#if BLE_Proxim
#include "ble_proximity.h"
#endif
#if BLE_UART
#include "ble_apps_uart.h"
#include "ble_nus.h"
#endif

#include "rtc.h"
#include "nrf.h"
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

#include "ak2.h"





//#define ENABLE_LOOPBACK_TEST           /*!< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback */

#define ERROR_PIN                (LED0)   /*!< gpio pin number to show error if loopback is enabled */
#define MAX_TEST_DATA_BYTES      (15U) /*!< max number of test bytes to be used for tx and rx */


////////Timing define//////////////////////////////////////
#define LED_Blink_Timing		     800           //
#define ADC_Heart_Sample         2            // 2ms
#define Key_press_Timeout        25           // 25ms 
#define Key_press_Hold_Timeout   3000         // key holding 
#define Timing_1sec              1000         // 


///ADC voltage /thershold level////////////////////////////
#define ADC_Heart_VCC           3000               // Heart Sensor Voltage 3.00v
#define ADC_Heart_Norm_Out      ADC_Heart_VCC>>1   // ADC_Vcc/2
#define ADC_Heart_Norm_Vin      ADC_Heart_Norm_Out/3   // Analog /3 = ADC_Heart_norm_out/3
#define ADC_Vref                1200               // 
#define ADC_Heart_Threshold     (ADC_Heart_Norm_Vin*1024)/ADC_Vref   // norm threshold 500/1200*1024= 424
#define ADC_Heart_NormIBI            600
#define ADC_Heart_StartTime       250                     // start capture time 
#define ADC_Heart_Timeout         2500                   //  Heart timeout (without pulse)



void Key0_init(void);
void Key1_init(void);

void UART_init(void);
void UART_Decode(void);
void uart_quit(void);

void uart_start(void);

void LED_init(void);
void LED_Status_Out(void);

void Convert_8bit_to_BCD_ASCII (unsigned char *buffer, unsigned char Data);
void Convert_16bit_to_BCD_ASCII (unsigned char *buffer, unsigned short Data);
void Convert_32bit_to_Hex_ASCII (unsigned char *buffer, unsigned int Data);
void ADC_Heart_Rate_Init(uint8_t  ADC_res, uint8_t ADC_input_select, uint8_t ADC_ISR_Enable);
void ADC_IRQHandler(void) ;
uint16_t adc_Heart_read(void) ;
void adc_start(void);
void Timer2_init(void);
void Timer2_handling(void);
void adc_Heart_Signal_Process(void);
#endif
