/***********************************************************
nRF51822 AKII development board hardware 




***********************************************************/




#ifndef AKII_H
#define AKII_H

//MPU-6050
#define Motion_SDA     0
#define Motion_SCL     1
#define Motion_INT		 2

// Heart Rate Sensor ADC input
#define Heart_ADC      3
#define ADC_Heart_Input   ADC_CONFIG_PSEL_AnalogInput4
#define ADC_Heart_RES     ADC_CONFIG_RES_10bit

// OLED SPI 
#define SPI0_MOSI				12
#define SPI0_MISO       13
#define SPI0_SCK        14
#define SPI0_SS         15

#define OLED_RST       24   // 
#define OLED_RS        25  // D/C control


#define LED_START      18
#define LED0           18
#define LED_STOP       19
#define LED1           19
#define LED_NEXT			 20
#define LED2					 20
#define LED3           21
#define LED4           22
#define LED_PORT       NRF_GPIO_PORT_SELECT_PORT2
#define LED_OFFSET     2

#define Sys_LED        LED3  // system running LED
#define HeartRead_LED  LED4 

#define BUTTON_START   16
#define BUTTON0        16
#define BUTTON_STOP    17
#define BUTTON1        17

/*
#define RX_PIN_NUMBER  11
#define TX_PIN_NUMBER  9
#define CTS_PIN_NUMBER 10
#define RTS_PIN_NUMBER 8
#define HWFC           false
*/


#if 0
#define RX_PIN_NUMBER  9    // UART RX pin number.
#define TX_PIN_NUMBER  11    // UART TX pin number.
#define CTS_PIN_NUMBER 8    // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 10    // Not used if HWFC is set to false
#define HWFC           false // UART hardware flow control

#else

#define RX_PIN_NUMBER  11
#define TX_PIN_NUMBER  9
#define CTS_PIN_NUMBER 10
#define RTS_PIN_NUMBER 8
#define HWFC           false

#endif








#define BLINKY_STATE_MASK   0x01





#endif
