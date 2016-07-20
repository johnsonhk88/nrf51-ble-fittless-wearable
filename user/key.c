/*****************************************************
Key file



****************************************************/

#include <stdint.h>

#include "nrf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "system.h"
#include "boards.h"



/** GPIOTE interrupt handler.
 * Triggered on pin 0 change
 */
#if 0 
void GPIOTE_IRQHandler(void)
{
  // Event causing the interrupt must be cleared
	 simple_uart_putstring((const uint8_t *)" \n\rTrigger GPIOTE!\n\r");
#if Key0_Func	
  if ((NRF_GPIOTE->EVENTS_IN[0] == 1) && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN0_Msk))
  {
    NRF_GPIOTE->EVENTS_IN[0] = 0;
		 nrf_gpio_pin_toggle(LED4);
  }
#endif

#if Key1_Func	
	 if ((NRF_GPIOTE->EVENTS_IN[1] == 1) && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN0_Msk))
  {
    NRF_GPIOTE->EVENTS_IN[1] = 0;
		 nrf_gpio_pin_toggle(LED3);
  }
#endif 
}
#endif

#if Key0_Func
void Key0_init(void)
{
	/*Configure Button for Input*/
	nrf_gpio_cfg_output(LED4);
  nrf_gpio_cfg_input(BUTTON0, NRF_GPIO_PIN_PULLUP);
	
	 NRF_GPIOTE->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) //High to low trigger
              | (16 << GPIOTE_CONFIG_PSEL_Pos)  // select pin Button0
              | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);   // in event mode

  /*Configure GPIOTE interrupt */
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;
	NVIC_EnableIRQ(GPIOTE_IRQn);

}	
#endif

#if Key1_Func
void Key1_init(void)
{

/*Configure Button for Input*/
	nrf_gpio_cfg_output(LED3);
  nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
	
	 NRF_GPIOTE->CONFIG[1] =  (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) //High to low trigger
              | (BUTTON1 << GPIOTE_CONFIG_PSEL_Pos)  // select pin Button0
              | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);   // in event mode

  /*Configure GPIOTE interrupt */
  NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos;
//	NVIC_EnableIRQ(GPIOTE_IRQn);

}	

#endif























