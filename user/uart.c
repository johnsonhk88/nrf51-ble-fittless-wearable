/******************************************************
File: UART Debug port driver




********************************************************/
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
//#include "boards/ak2.h"
#include "system.h"
#include "oled_096.h"

#if UART_Func
 
volatile uint8_t cr=0;
volatile uint8_t rx_flag=0;
uint8_t Rx_Buff[64];

extern uint8_t Firmware_Ver[];
extern RTC_Reg RTC_Con;
extern ble_nus_t  m_nus; 

extern volatile uint8_t OLED_Contrast;

#ifndef ENABLE_LOOPBACK_TEST

/** Sends ' Exit!' string to UART.
Execution is blocked until UART peripheral detects all characters have been sent.
 */


#else

/** Set @ref ERROR_PIN to one and enters an infinite loop. This function is called if any of the
 *  nRF6350 functions fail.
 */
static void show_error(void)
{
  nrf_gpio_pin_write(ERROR_PIN, 1);
  while(true)
  {
  }
}

/** Transmits one char at a time as check if the loopback received data is same as transmitted
 *  Just used for testing with loopback setup (i.e, @ref TX_PIN_NUMBER connected to @ref RX_PIN_NUMBER)
 *  return true if test passed, else return false
 */
static void uart_loopback_test()
{
  uint8_t tx_data[] = ("\n\r  LOOPBACK_TEST");
  uint8_t rx_data[MAX_TEST_DATA_BYTES] = {0};

  // Start sending one byte and see if you get the same
  for(uint8_t i = 0; i < MAX_TEST_DATA_BYTES; i++)
  {
    bool status;
    simple_uart_put(tx_data[i]);
    if(!simple_uart_get_with_timeout(2, &rx_data[i]))
	{
	  show_error();
	}
  }
	
  for(uint8_t i = 0; i < MAX_TEST_DATA_BYTES; i++)
  {
	  if ((rx_data[i] != tx_data[i]))
    {
      show_error();
    }
  }
  return; // Test passed
}

#endif   // end of loopback test

///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
/************************************************************** 
* 函数名  : fputc()
* 描述    : 重定义putc函数，这样可以使用printf函数从串口1打印输出
* 输入    : None
* 输出    : None
* 返回    : None
* 作者    : 青风
* 创建日期: 2014.1.1
* 版本    : V1.00
*************************************************************/
int fputc(int ch, FILE *f)
{
		/* Place your implementation of fputc here */
		/* 发送一个字节的数据 */
		simple_uart_put((uint8_t)ch);
		/* 等待发送完成 */
		return ch;
}

/************************************************************** 
* 函数名  : fputc()
* 描述    : 重定义putc函数，这样可以使用printf函数从串口1获取数据
* 输入    : None
* 输出    : None
* 返回    : None
* 作者    : 青风
* 创建日期: 2014.1.1
* 版本    : V1.00
*************************************************************/
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		/* 返回接收到的数据 */
		return (int)simple_uart_get();
}

#endif


#if UART_Func_Int

/**@brief UART Interrupt handler.
*
* @details UART interrupt handler to process TX Ready when TXD is available, RX Ready when a byte
* is received, or in case of error when receiving a byte.
*/
void UART0_IRQHandler(void)
{
	//uint8_t cr=0;
// Handle reception
if (NRF_UART0->EVENTS_RXDRDY != 0)
{
// Clear UART RX event flag
NRF_UART0->EVENTS_RXDRDY = 0;
//cr = (uint8_t)NRF_UART0->RXD;
if(rx_flag<64)
{	
 Rx_Buff[rx_flag++]=(uint8_t)NRF_UART0->RXD;
}
else
{
	UART_Decode();
	 Rx_Buff[rx_flag++]=(uint8_t)NRF_UART0->RXD;
}
  }
}


#endif // end of UART interrupt



//////////////////////////////////////////////////////////////////////////////////////////

void UART_init(void)
{	
simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
#if 	UART_Func_Int
// Enable UART interrupt
NRF_UART0->INTENCLR = 0xffffffffUL;
NRF_UART0->INTENSET = (UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos);
	// |(UART_INTENSET_TXDRDY_Set << UART_INTENSET_TXDRDY_Pos) |
 //(UART_INTENSET_ERROR_Set << UART_INTENSET_ERROR_Pos);
NVIC_ClearPendingIRQ(UART0_IRQn);
NVIC_SetPriority(UART0_IRQn, 0);
NVIC_EnableIRQ(UART0_IRQn);   // enable interrupt
#endif	

#ifndef ENABLE_LOOPBACK_TEST	
// uart_start(); // send start debug message
 printf("\n\rStart: ");
 printf("\n\rFriwmare Version: ");
//simple_uart_putstring((const uint8_t *)"\n\rFriwmare Version: ");
// simple_uart_putstring((const uint8_t *)  Firmware_Ver);
 printf("%s",Firmware_Ver);

#else
	// ERROR_PIN configure as output
  nrf_gpio_cfg_output(ERROR_PIN);
#endif


}


////////////////////////////////////////////////////////////////
void UART_Decode(void)
{	
  int i=0, j=0;
	
 #if 	 BLE_UART & UART_TO_BLE
	uint32_t err_code=0;
	if((rx_flag>0) && (rx_flag<20))
	{	
		 
	 err_code = ble_nus_send_string(&m_nus, Rx_Buff, rx_flag );      //上传数据
   if (err_code != NRF_ERROR_INVALID_STATE)
   {
   APP_ERROR_CHECK(err_code);
   }
 }
	 
	 #endif		
	
	
	
#ifndef  ENABLE_LOOPBACK_TEST
		/*Get the input character*/
#if (UART_Func_Int)		
		
		if(rx_flag)
		 {
			 nrf_delay_ms(500); 
			for(i=0; i<rx_flag; i++)
       {		
#if 0				 
      simple_uart_put(Rx_Buff[i]);

    if(Rx_Buff[i]== 'q' || Rx_Buff[i] == 'Q')
     {
			 printf(" \n\rExit!\n\r");
			  nrf_gpio_pin_toggle(LED4);
			 break;
     } 	
#endif		 


  	 switch(Rx_Buff[i])
				 {

				 
		case  's' :
#if RTC_Func				
			         if(Rx_Buff[i+1]=='c')
		           {
						     switch(Rx_Buff[i+2])
                 {
                 case 's':   // set second
									          if((Rx_Buff[i+3]>='0')&&(Rx_Buff[i+3]<='9'))
														{
                              if((Rx_Buff[i+4]>='0')&&(Rx_Buff[i+4]<='9'))
															{
																if(Rx_Buff[i+5]=='e')
																{
																RTC_Con.Second_Reg=((Rx_Buff[i+3]-0x30)*10) + Rx_Buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;
														
								 case 'm':   // set mintue
									          if((Rx_Buff[i+3]>='0')&&(Rx_Buff[i+3]<='9'))
														{
                              if((Rx_Buff[i+4]>='0')&&(Rx_Buff[i+4]<='9'))
															{
																if(Rx_Buff[i+5]=='e')
																{
																 RTC_Con.Min_Reg=((Rx_Buff[i+3]-0x30)*10) + Rx_Buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		

                    case 'h':   // set mintue
									          if((Rx_Buff[i+3]>='0')&&(Rx_Buff[i+3]<='9'))
														{
                              if((Rx_Buff[i+4]>='0')&&(Rx_Buff[i+4]<='9'))
															{
																if(Rx_Buff[i+5]=='e')
																{
																 RTC_Con.Hour_Reg=((Rx_Buff[i+3]-0x30)*10) + Rx_Buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		


                   case 'd':   // set day
									          if((Rx_Buff[i+3]>='0')&&(Rx_Buff[i+3]<='9'))
														{
                              if((Rx_Buff[i+4]>='0')&&(Rx_Buff[i+4]<='9'))
															{
																if(Rx_Buff[i+5]=='e')
																{
																 RTC_Con.Day_Reg=((Rx_Buff[i+3]-0x30)*10) + Rx_Buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		


                   
                case 'M':   // set Month
									          if((Rx_Buff[i+3]>='0')&&(Rx_Buff[i+3]<='9'))
														{
                              if((Rx_Buff[i+4]>='0')&&(Rx_Buff[i+4]<='9'))
															{
																if(Rx_Buff[i+5]=='e')
																{
																 RTC_Con.Month_Reg=((Rx_Buff[i+3]-0x30)*10) + Rx_Buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;				

                case 'Y':   // set year
									          for(j=0; j<4; j++)
								            {
														if((Rx_Buff[i+3+j]<'0')&&(Rx_Buff[i+3+j]>'9'))
														 {
															break;
														 }
														 														 
															
														}	
									          if(Rx_Buff[i+7]=='e')
																{
																 RTC_Con.Year_Reg=((Rx_Buff[i+3]-0x30)*1000) + ((Rx_Buff[i+4]-0x30)*100)+ 
																	  ((Rx_Buff[i+5]-0x30)*10)+((Rx_Buff[i+6]-0x30));
																}	
                         
																												
                             break;																

														


								 }									 
								 
								 
							 } 
#endif  // end of RTC setting 

 #if OLED096_Func             
 if(Rx_Buff[i+1]=='o')
		           {
						     switch(Rx_Buff[i+2])
                 {
                 case 'i':   // increase OLED contrast
									            if(Rx_Buff[i+3]!='e')
															{
																break;
															}	
									           if(OLED_Contrast<0xDF)
														 {	 
															OLED_Contrast+=0x20; 
									           OLED_WR_Byte(0x81,OLED_CMD); //对比度设置	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 
														 } 	
														 else
														 {
														 OLED_Contrast =0xff; 
									           OLED_WR_Byte(0x81,OLED_CMD); //对比度设置	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD);  
															 
														 } 
															 
															 
                             break;
														
								 case 'd':   // decrease OLED contrast
									             if(Rx_Buff[i+3]!='e')
															{
																break;
															}	
									         if(OLED_Contrast>=0x20)
														 {	 
															OLED_Contrast-=0x20; 
									           OLED_WR_Byte(0x81,OLED_CMD); //对比度设置	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 
														 } 
														 else
														 {
														 OLED_Contrast =0x00; 
									           OLED_WR_Byte(0x81,OLED_CMD); //对比度设置	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 

														 }															 
                             break;		

														


								 }									 
								 
							 } 
#endif			// 				 
			          break; 

		 
	       }
			 } 
      rx_flag=0;
		 }			 
#else		
    cr = simple_uart_get(); 
    simple_uart_put(cr);

    if(cr == 'q' || cr == 'Q')
    {
      uart_quit();
      while(1){}
    }		

#endif //  end of non-interrupt function		
		
#else		
		  uart_loopback_test();

#endif   // end of loop back function








}


///////////////////////////////////////////////////////
void Convert_8bit_to_BCD_ASCII (unsigned char *buffer, unsigned char Data)
{
 unsigned char i=0;
  unsigned char temp=0;
 unsigned char result[3];
   temp= Data;
       result[0] = (temp/100 )+0x30;
	   result[1] =  ((temp%100)/10)+0x30;
	   result[2] = 	((temp%100)%10)+0x30;
	    
	   for(i=0; i<3; i++)
       {
		    *buffer   =result[i];
			buffer++; 					
		}
 }


void Convert_16bit_to_BCD_ASCII (unsigned char *buffer, unsigned short Data)
{
 unsigned char i=0 ;
 unsigned short temp=0;
 unsigned char result[5];
   temp= Data;
    
	   result[0] = (temp/10000 )+0x30;		  // 10throusd
	   result[1] =  ((temp%10000)/1000)+0x30;	  //1 thruosed
	   result[2] = 	((temp%1000)/100)+0x30;	  	  // 100
	   result[3] = 	((temp%100)/10)+0x30;		 // 10
	   result[4] =   (temp%10)+0x30;			 // 1

       	    
	   for(i=0; i<5; i++)
       {
		    *buffer   =result[i];
			buffer++; 					
		}



 }




 
 void Convert_32bit_to_Hex_ASCII (unsigned char *buffer, unsigned int Data)
 {
   unsigned char i=0;
   unsigned int temp =0;
   unsigned char  result[8];
   temp= Data;
// convert 32 MSB (28-32bit)
      	   if(((temp & 0xf0000000)>>28)<0x0a) 
		     {
		     result[0] =(unsigned char)(((temp &  0xf0000000)>>28)+0x30);

			}
		   else
		     {

			  result[0] = (unsigned char)(((temp &  0xf0000000)>>28)+0x37);
			  }

// convert 32 MSB (24-27bit)			 
		     if(((temp & 0x0f000000)>>24)<0x0a) 
		     {
		     result[1] = (unsigned char)(((temp & 0x0f000000)>>24)+0x30);

			}
		   else
		     {

			  result[1] = (unsigned char)(((temp & 0x0f000000)>>24)+0x37);
			  }
// convert 32 MSB (20-23bit)	      
		    if(((temp & 0x00f00000)>>20)<0x0a) 
		     {
		     result[2] = ((temp & 0x00f00000)>>20)+0x30;

			}
		   else
		     {

			  result[2] = ((temp & 0x00f00000)>>20)+0x37;
			  }
// convert 32 MSB (16-19bit)	      
		    if(((temp & 0x000f0000)>>16)<0x0a) 
		     {
		     result[3] = ((temp & 0x000f0000)>>16)+0x30;

			}
		   else
		     {

			  result[3] = ((temp & 0x000f0000)>>16)+0x37;
			  }
// convert 32 MSB (12-15bit)	      
		    if(((temp & 0x0000f000)>>12)<0x0a) 
		     {
		     result[4] = ((temp & 0x0000f000)>>12)+0x30;

			}
		   else
		     {

			  result[4] = ((temp & 0x0000f000)>>12)+0x37;
			  }

// convert 32 MSB (8-13bit)	      
		    if(((temp & 0x00000f00)>>8)<0x0a) 
		     {
		     result[5] = ((temp & 0x00000f00)>>8)+0x30;

			}
		   else
		     {

			  result[5] = ((temp & 0x00000f00)>>8)+0x37;
			  }
// convert 32 MSB (4-7bit)	      
		    if(((temp & 0x000000f0)>>4)<0x0a) 
		     {
		     result[6] = ((temp & 0x000000f0)>>4)+0x30;

			}
		   else
		     {

			  result[6] = ((temp & 0x000000f0)>>4)+0x37;
			  }
// convert 32 MSB (0-3bit)	      
		    if((temp & 0x0000000f)<0x0a) 
		     {
		     result[7] = (temp & 0x0000000f)+0x30;

			}
		   else
		     {

			  result[7] = (temp & 0x0000000f)+0x37;
			 
			  }
// write 
	     for(i=0; i<8; i++)
       {
		    *buffer   =result[i];
			buffer++; 					
		}




  }















#endif




