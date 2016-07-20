/********************************************
file: RTC header file



**********************************************/


#ifndef        __RTC_H
#define        __RTC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_gpio.h"



#define GPIO_TOGGLE_TICK_EVENT    (8)                       /*!< Pin number to toggle when there is a tick event in RTC */
#define GPIO_TOGGLE_COMPARE_EVENT (9)                       /*!< Pin number to toggle when there is compare event in RTC */
#define LFCLK_FREQUENCY           (32768UL)                 /*!< LFCLK frequency in Hertz, constant */
#define RTC_FREQUENCY             (10UL)                     /*!< required RTC working clock RTC_FREQUENCY Hertz. Changable */
#define COMPARE_COUNTERTIME       (1UL)                     /*!< Get Compare event COMPARE_TIME seconds after the counter starts from 0 */

#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1)  /* f = LFCLK/(prescaler + 1) */

#define RTC_Update_Value           (RTC_FREQUENCY*COMPARE_COUNTERTIME)



#define    One_Min        60     // 60 second
#define    One_Hour       60     // 60 Minture
#define    One_Day        24     // 24 hour    
#define    One_Year       12    //  12 month per year



typedef struct  {
	               unsigned char  Soft_RTC_En : 1;
	               unsigned char  Soft_RTC_Update : 1;
	               unsigned char  Second_Reg;
	               unsigned char  Min_Reg;       
	               unsigned char  Hour_Reg;
	               unsigned char  Day_Reg;
	               unsigned char  Month_Reg;
	               unsigned short Year_Reg;
	               unsigned int   Second_Cnt;     // number of timer interrupt counter
	

              }RTC_Reg;






void RTC_Init(void);
void RTC_setSecond(unsigned char sec);
void RTC_setMin(unsigned char min);
void RTC_setHour(unsigned char hour);
void RTC_setMonth(unsigned char month);
void RTC_setDay(unsigned char Day);
void RTC_setYear(unsigned int Year);
void RTC_Update(void);
void RTC_Run(void);							
unsigned char monthDay(unsigned char Month);							







void lfclk_config(void);
void rtc_config(void);








#endif

