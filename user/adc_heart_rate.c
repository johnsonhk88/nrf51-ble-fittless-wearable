/******************************************************
ADC  heart beat sensor

normal output of heart beat sensor = Vcc/2 (3v/2=1.5v

Analog input = Vsig/3 , so normal ADC input= 1.5/3 =0.5v



***********************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "system.h"
#include "oled_096.h"


#if  Heart_Rate_Func

uint8_t  HRM_Run=0;   // 

uint16_t adc_result = 0;  // 
uint8_t  HRM_Update=0;
uint32_t ADC_Sample_Per_Sec=0;

volatile int rate[16];                    // used to hold last ten IBI values
volatile int rate_max=0;
volatile int rate_min=0;              
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile int P =ADC_Heart_Threshold;                      // used to find peak in pulse wave
volatile int T = ADC_Heart_Threshold;                     // used to find trough in pulse wave
volatile int thresh = (ADC_Heart_Threshold+15);                // used to find instant moment of heart beat
volatile int Pulse_amp = 100;                   // used to hold amplitude of pulse waveform
volatile uint8_t firstBeat = 1;        // used to seed rate array so we startup with reasonable BPM
volatile uint8_t secondBeat = 0;       // used to seed rate array so we startup with reasonable BPM
volatile uint32_t Pulse_Timing=0;
uint16_t runningTotal = 0;                   // clear the runningTotal variable 


// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile uint16_t Signal;                // holds the incoming ADC raw data
volatile int IBI = ADC_Heart_NormIBI;             // holds the time between beats, the Inter-Beat Interval in ms
volatile uint8_t Pulse = 0;     // true when pulse wave is high, false when it's low
volatile uint8_t QS = 0;        // becomes true when Arduoino finds a beat.


// inital ADC Heart Rate I
void ADC_Heart_Rate_Init(uint8_t  ADC_res, uint8_t ADC_input_select, uint8_t ADC_ISR_Enable)
	
{
  // uint32_t err_code;

    // Configure ADC
	  NRF_ADC->CONFIG     = (ADC_res                                    << ADC_CONFIG_RES_Pos)     |    // 
                          (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)  |    // Analog input/3 prescaling input
                          (ADC_CONFIG_REFSEL_VBG                      << ADC_CONFIG_REFSEL_Pos)  |   // Use internal 1.2V bandgap voltage
                          (ADC_input_select                           << ADC_CONFIG_PSEL_Pos)    |
                          (ADC_CONFIG_EXTREFSEL_None                  << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
	
	
	
	NRF_ADC->ENABLE = 1;  
    // Check interrupt enable  
  if(ADC_ISR_Enable)
        {
            NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
            NVIC_SetPriority(ADC_IRQn, 1);
            NVIC_EnableIRQ(ADC_IRQn);
        }

}	

void ADC_IRQHandler(void)  //中断函数
{
    
     if (NRF_ADC->EVENTS_END != 0) 
			{
        NRF_ADC->EVENTS_END     = 0;               // clear evnt
        adc_result              = NRF_ADC->RESULT;  //读取adc的值
				//printf("ADC Heart Raw Data:%d\r\n",adc_result);
				HRM_Update=1;
				ADC_Sample_Per_Sec++;
        NRF_ADC->TASKS_STOP     = 1;
    }
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Disabled;
}
void adc_start(void)  //开始中断
{
	if((HRM_Update==0) &&(HRM_Run==1))
	{	
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;

    NRF_ADC->EVENTS_END  = 0;    
    NRF_ADC->TASKS_START = 1;
	}	
}

/// 
uint16_t adc_Heart_read(void)  //读取数据
{
  uint16_t adc_data;
  NRF_ADC->TASKS_START = 1;
  while(NRF_ADC->EVENTS_END == 0);
  NRF_ADC->EVENTS_END = 0;
  adc_data = NRF_ADC->RESULT;
  return adc_data;
}



void adc_Heart_Signal_Process(void)
{
	int i=0;
 Signal=  adc_result;  // copy Adc result
 sampleCounter++;      // sample counter+1 1ms	
 Pulse_Timing = sampleCounter - lastBeatTime;  // monitor the time since the last beat to avoid noise
	
 //  find trough of the pulse wave
  if((Signal < thresh) && (Pulse_Timing > (IBI/5)*3))
	{      
		// avoid dichrotic noise by waiting 3/5 of last IBI
     if (Signal < T)
		 {                        // T is the trough
          T = Signal;         // update new lowest point in pulse wave 
      }
  } 
	// update new Peak 
	if(Signal > thresh && Signal > P)
		{
		// thresh condition helps avoid noise
        P = Signal;                             // P is the peak
    }                                        // keep track of highest point in pulse wave
	
 //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
if (Pulse_Timing > ADC_Heart_StartTime)
	{                                   
	 // avoid high frequency noise
  if ( (Signal > thresh) && (Pulse == 0) && (Pulse_Timing > (IBI/5)*3) )
		{        
    Pulse = 1;                        // set the Pulse flag when we think there is a pulse
   // digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
	//	nrf_gpio_pin_set( HeartRead_LED);	
    IBI = sampleCounter - lastBeatTime;         // Update IBI measure time between beats in mS
			lastBeatTime = sampleCounter;               // current time = lastBesttime of time for next pulse	
		
			    if(secondBeat==1)
					 {
 					 // if this is the second beat, if secondBeat == TRUE
           secondBeat = 0;         // clear secondBeat flag
            for(int i=0; i<=15; i++)
						 {
							// seed the running total to get a realisitic BPM at startup
                rate[i] = IBI;                      
              }
            }
			
			
	    if(firstBeat==1)
				{
  				// if it's the first time we found a beat, if firstBeat == TRUE
          firstBeat = 0;                 // clear firstBeat flag
					secondBeat = 1;  
           return;                            // IBI value is unreliable so discard it
         }   
     
          runningTotal = 0;
    // keep a running total of the last 10 IBI values
				 
	#if 0			 
      
    // calculate BPM old alogrithm
    for(int i=0; i<=14; i++)
			{               
				// shift data in the rate array
          rate[i] = rate[i+1];              // and drop the oldest IBI value 
          runningTotal += rate[i];          // add up the 9 oldest IBI values
        }
       
    rate[15] = IBI;                          // add the latest IBI to the rate array
    runningTotal += rate[15];                // add the latest IBI to runningTotal
    runningTotal /= 16;                     // average the last 10 IBI values 
	
#else
	  // calculate BPM new alogrithm find rate Max and min
			
			// shift data in the rate array	
			 for(i=0; i<=14; i++)
			{               	             
				
          rate[i] = rate[i+1];              // and drop the oldest IBI value 
			}	
			rate[15]=IBI;         // update new IBI value in array
			 rate_max =rate[0];  // preset max				
			 rate_min =rate[0];	 // preset min 
			
			// find max/min rate value
			 for(i=0; i<=15; i++)
			{    
				if(rate[i]<rate_min)
				{
					rate_min=rate[i];
				}
        if(rate[i]>rate_max)
				{
          rate_max=rate[i];
				}	
         runningTotal += rate[i]; // sum all of IBI data				
			}	
			runningTotal = runningTotal-rate_max-rate_min;   // remove max/min vlue
			runningTotal /= 14;                     // average the last 14 IBI values 
			
			
			
#endif				
				
    BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
    QS = 1;                              // set Quantified Self flag 
    // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
}	 
	   //set new threshold
		 if (Signal < thresh && Pulse == 1)
			 {
				 // when the values are going down, the beat is over 
     //   digitalWrite(blinkPin,LOW);            // turn off pin 13 LED 
		//	nrf_gpio_pin_clear(HeartRead_LED);	 
       Pulse = 0;                         // reset the Pulse flag so we can do it again 
       Pulse_amp = P - T;                 // find pulse amplitude of the pulse wave 
       thresh = (Pulse_amp>>1) + T;                    // set thresh at 50% of the amplitude 
       P = thresh;                            // reset these for next time (peak) 
       T = thresh;                            // reset through
    } 
		// ADC Heart Timeout 	 
   if (Pulse_Timing > ADC_Heart_Timeout)
		 {                           // if 2.5 seconds go by without a beat 
     thresh = ADC_Heart_Threshold+15;                          // set thresh default 
     P = ADC_Heart_Threshold;                               // set P default 
     T = ADC_Heart_Threshold;                               // set T default 
     lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date         
     firstBeat = 1;                      // set these to avoid noise 
     secondBeat = 0;                    // when we get the heartbeat back 
     } 

		

}	











#endif 


