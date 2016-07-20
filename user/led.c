/********************************************
LED driver


*************************************************/

#include <stdint.h>

#include "nrf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "system.h"
#include "boards.h"



#if LED_Func

void LED_init(void)
{	
   // Configure LED-pins as outputs
 // nrf_gpio_range_cfg_output(LED_START, LED_NEXT); //LED_STOP);
	nrf_gpio_range_cfg_output(LED_START, LED3); //LED_STOP);
}



/////////////////////////////////////////////////////////////////
void LED_Status_Out(void)
{	
  static  uint8_t output_state = 0;
//  nrf_gpio_port_write(LED_PORT, 1 << (output_state + LED_OFFSET));
  //  output_state = (output_state + 1) & BLINKY_STATE_MASK;
	//	  Port 2 (GPIO pin 16-23)
		 nrf_gpio_port_write(LED_PORT, (1 << (output_state + 2)));
    output_state = (output_state + 1)&0x03; //&0x07;
    nrf_delay_ms(100);

}


#endif





