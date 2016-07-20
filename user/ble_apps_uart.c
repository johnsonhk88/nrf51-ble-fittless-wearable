/**************************************
BLE APPS UART Source file


****************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "system.h"
#include "oled_096.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "simple_uart.h"
#include "ble_error_log.h"
#include "ble_debug_assert_handler.h"
#include "nrf_delay.h"

///////////////

#if BLE_UART

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
volatile uint8_t                          BT_TR_time_count=0;                         //????



volatile uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
uint8_t states[4];
volatile uint8_t index_i = 0;
volatile uint8_t change_state=0;
uint8_t BLE_Rx_Flag=0;

extern app_timer_id_t                           m_rtc_update_timer_id;                            // Timer to Real time clock)  
extern uint8_t Firmware_Ver[];
extern RTC_Reg RTC_Con;
extern volatile uint8_t OLED_Contrast;


/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
  nrf_gpio_cfg_output(ADVERTISING_LED_PIN_NO);
  nrf_gpio_cfg_output(CONNECTED_LED_PIN_NO);
	
	//nrf_gpio_cfg_output(RELAY);
	//nrf_gpio_cfg_output(LED_RED);
	//nrf_gpio_cfg_output(LED_GREEN );
	nrf_gpio_cfg_output(LED4);
	nrf_gpio_cfg_output(LED2);
	
//	nrf_gpio_pin_clear(LED_RED);
//	nrf_gpio_pin_set(LED_GREEN);
//	nrf_gpio_pin_set(LED_BLUE);
//	nrf_gpio_pin_set(RELAY);
	  nrf_gpio_pin_clear(LED4);
	  nrf_gpio_pin_clear(LED2);
}


/**@brief   Function for Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
	  uint32_t err_code;
  // Initialize timer module
  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
	 err_code = app_timer_create(&m_rtc_update_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                rtc_timeout_handler);
    APP_ERROR_CHECK(err_code); 
	
}


/**@brief   Function for the GAP initialization.
 *
 * @details This function will setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief   Function for the Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    
    ble_uuid_t adv_uuids[] = {{BLE_UUID_NUS_SERVICE, m_nus.uuid_type}};

    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;
    
    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
}


/**@brief    Function for handling the data from the Nordic UART Service.
 *
 * @details  This function will process the data received from the Nordic UART BLE Service and send
 *           it to the UART module.
 */
/**@snippet [Handling the data received over BLE] */

//´ÓBLEÐ­ÒéÕ»»ñÈ¡APPÏÂ·¢µÄÊý¾Ý

void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
	  printf("BLE Rx Length: %d",length);
	
	  BLE_Rx_Decode(p_data, length);
/*	
	  if(((p_data[0]=='O')||(p_data[0]=='o'))&&((p_data[1]=='N')||(p_data[1]=='n')))  //????????“ON”??"on"
		{
		 //nrf_gpio_pin_clear(LED_GREEN );    //???????
		// nrf_gpio_pin_set(LED_RED );        //???????
		 //nrf_gpio_pin_clear(RELAY);         //?????
			nrf_gpio_pin_set(LED4);
		 states[0]=p_data[0];               //¸üÐÂÒªÉÏ´«µÄ×´Ì¬Êý¾Ý
		 states[1]=p_data[1];
		 states[2]=0;
		 change_state=1;                  //??????,????????
		}
		
		 if(((p_data[0]=='O')||(p_data[0]=='o'))&&((p_data[1]=='F')||(p_data[1]=='f'))&&((p_data[2]=='F')||(p_data[2]=='f')))
		{
		 //????????“OFF”??"off"
		// nrf_gpio_pin_clear(LED_RED);     //???????
		 //nrf_gpio_pin_set(LED_GREEN );    //???????
		 //nrf_gpio_pin_set(RELAY);         //?????
		 nrf_gpio_pin_clear(LED4);	
		 states[0]=p_data[0];             //??????????
		 states[1]=p_data[1];
		 states[2]=p_data[2];
		 change_state=1;                  //??????,????????
		}
*/		
	}
/**@snippet [Handling the data received over BLE] */



/**@brief Function for initializing services that will be used by the application.
 */
	
//service init 
static void services_init(void)
{
    uint32_t         err_code;
    ble_nus_init_t   nus_init;
    
    memset(&nus_init, 0, sizeof(nus_init));      //??nus_init?????

    nus_init.data_handler = nus_data_handler;    //??service?handler
    
    err_code = ble_nus_init(&m_nus, &nus_init);  //???nus service
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief       Function for handling an event from the Connection Parameters Module.
 *
 * @details     This function will be called for all events in the Connection Parameters Module
 *              which are passed to the application.
 *
 * @note        All this function does is to disconnect. This could have been done by simply setting
 *              the disconnect_on_fail config parameter, but instead we use the event handler
 *              mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief       Function for handling errors from the Connection Parameters module.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
//Á¬½Ó²ÎÊýµÄ³õÊ¼»¯
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));     //?????

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
//¿ªÊ¼¹ã²¥
static void advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
    
    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));           //?????
    
    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;            //????
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;  //?????

    err_code = sd_ble_gap_adv_start(&adv_params);         //????????
    APP_ERROR_CHECK(err_code);                            //????ERROR??

    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);             //????????
}


/**@brief       Function for the Application's S110 SoftDevice event handler.
 *
 * @param[in]   p_ble_evt   S110 SoftDevice event.
 */
//Ã¿´ÎÁ¬½ÓÊÂ¼þ²úÉúµÄ´¦Àíº¯Êý
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:                             //?????
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);             //???????
            nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);         //??????
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
				    	#if OLED096_Func 
						OLED_ShowString(0,16, (uint8_t*)"BT:CON");
	           OLED_Refresh_Gram();
						#endif
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            advertising_start();
				    #if OLED096_Func 
						OLED_ShowString(0,16, (uint8_t*)"BT:DIS");
	           OLED_Refresh_Gram();
						#endif

            break;
            
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;
            
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;
            
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
             nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);

                // Configure buttons with sense level low as wakeup source.
             //nrf_gpio_cfg_sense_input(WAKEUP_BUTTON_PIN,
             //                             BUTTON_PULL,
             //                             NRF_GPIO_PIN_SENSE_LOW);
                
                // Go to system-off mode (this function will not return; wakeup will cause a reset)
             //     err_code = sd_power_system_off();    
             //   APP_ERROR_CHECK(err_code);
              #if OLED096_Func 
					    OLED_ShowString(0,16, (uint8_t*)"BT:Tout");
	            OLED_Refresh_Gram();
					    #endif		
							
						 advertising_start();
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief       Function for dispatching a S110 SoftDevice event to all modules with a S110
 *              SoftDevice event handler.
 *
 * @details     This function is called from the S110 SoftDevice event interrupt handler after a
 *              S110 SoftDevice event has been received.
 *
 * @param[in]   p_ble_evt   S110 SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
}


/**@brief   Function for the S110 SoftDevice initialization.
 *
 * @details This function initializes the S110 SoftDevice and the BLE event interrupt.
 */
//BLE stack initial
static void ble_stack_init(void)
{
    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);
    
    // Subscribe for BLE events.
    uint32_t err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}




/**@brief  Function for placing the application in low power state while waiting for events.
 */
void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


/////////BLE UART APPS initial///////////////////////////////////////////

void BLE_UART_Init(void)
{
   leds_init();              //Ö¸Ê¾µÆ¼ÌµçÆ÷³õÊ¼»¯  
	  timers_init();            //Èí¼þ¶¨Ê±Æ÷³õÊ¼»¯
    //uart_init();              //´®¿Ú³õÊ¼»¯
	 // nrf_delay_us(100000);
	
     //À¶ÑÀ³õÊ¼»¯
    ble_stack_init();    // Install BLE event Handler function list
    gap_params_init();    // set gap parameter
    services_init();       //set advertising mode, set UUID service
    advertising_init();    // add UUID service , and service function handler
    conn_params_init();   // set the connection parameter
    sec_params_init();    // set the security connect parameter
   
    advertising_start();      //¿ªÊ¼¹ã²¥

}



//////BLE RX Decode///////////////////////////////////////

void BLE_Rx_Decode(uint8_t * rx_buff, uint16_t rx_length)
{	
  int i=0, j=0;

		/*Get the input character*/		
		if(rx_length)
		 {
			// nrf_delay_ms(500); 
			for(i=0; i<rx_length; i++)
       {		
	
  	 switch(rx_buff[i])
				 {			 
		case  's' :
#if RTC_Func				
			         if(rx_buff[i+1]=='c')
		           {
						     switch(rx_buff[i+2])
                 {
                 case 's':   // set second
									          if((rx_buff[i+3]>='0')&&(rx_buff[i+3]<='9'))
														{
                              if((rx_buff[i+4]>='0')&&(rx_buff[i+4]<='9'))
															{
																if(rx_buff[i+5]=='e')
																{
																RTC_Con.Second_Reg=((rx_buff[i+3]-0x30)*10) + rx_buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;
														
								 case 'm':   // set mintue
									          if((rx_buff[i+3]>='0')&&(rx_buff[i+3]<='9'))
														{
                              if((rx_buff[i+4]>='0')&&(rx_buff[i+4]<='9'))
															{
																if(rx_buff[i+5]=='e')
																{
																 RTC_Con.Min_Reg=((rx_buff[i+3]-0x30)*10) + rx_buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		

                    case 'h':   // set mintue
									          if((rx_buff[i+3]>='0')&&(rx_buff[i+3]<='9'))
														{
                              if((rx_buff[i+4]>='0')&&(rx_buff[i+4]<='9'))
															{
																if(rx_buff[i+5]=='e')
																{
																 RTC_Con.Hour_Reg=((rx_buff[i+3]-0x30)*10) + rx_buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		


                   case 'd':   // set day
									          if((rx_buff[i+3]>='0')&&(rx_buff[i+3]<='9'))
														{
                              if((rx_buff[i+4]>='0')&&(rx_buff[i+4]<='9'))
															{
																if(rx_buff[i+5]=='e')
																{
																 RTC_Con.Day_Reg=((rx_buff[i+3]-0x30)*10) + rx_buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;		


                   
                case 'M':   // set Month
									          if((rx_buff[i+3]>='0')&&(rx_buff[i+3]<='9'))
														{
                              if((rx_buff[i+4]>='0')&&(rx_buff[i+4]<='9'))
															{
																if(rx_buff[i+5]=='e')
																{
																 RTC_Con.Month_Reg=((rx_buff[i+3]-0x30)*10) + rx_buff[i+4]-0x30 ;
																}	
															}	
														}															
                             break;				

                case 'Y':   // set year
									          for(j=0; j<4; j++)
								            {
														if((rx_buff[i+3+j]<'0')&&(rx_buff[i+3+j]>'9'))
														 {
															break;
														 }
														 														 
															
														}	
									          if(rx_buff[i+7]=='e')
																{
																 RTC_Con.Year_Reg=((rx_buff[i+3]-0x30)*1000) + ((rx_buff[i+4]-0x30)*100)+ 
																	  ((rx_buff[i+5]-0x30)*10)+((rx_buff[i+6]-0x30));
																}	
                         
																												
                             break;																


								 }									 		 
								 
							 } 
#endif  // end of RTC setting 

 #if OLED096_Func             
 if(rx_buff[i+1]=='o')
		           {
						     switch(rx_buff[i+2])
                 {
                 case 'i':   // increase OLED contrast
									            if(rx_buff[i+3]!='e')
															{
																break;
															}	
									           if(OLED_Contrast<0xDF)
														 {	 
															OLED_Contrast+=0x20; 
									           OLED_WR_Byte(0x81,OLED_CMD); //¶Ô±È¶ÈÉèÖÃ	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 
														 } 	
														 else
														 {
														 OLED_Contrast =0xff; 
									           OLED_WR_Byte(0x81,OLED_CMD); //¶Ô±È¶ÈÉèÖÃ	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD);  
															 
														 } 
															 
															 
                             break;
														
								 case 'd':   // decrease OLED contrast
									             if(rx_buff[i+3]!='e')
															{
																break;
															}	
									         if(OLED_Contrast>=0x20)
														 {	 
															OLED_Contrast-=0x20; 
									           OLED_WR_Byte(0x81,OLED_CMD); //¶Ô±È¶ÈÉèÖÃ	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 
														 } 
														 else
														 {
														 OLED_Contrast =0x00; 
									           OLED_WR_Byte(0x81,OLED_CMD); //¶Ô±È¶ÈÉèÖÃ	
														 OLED_WR_Byte(OLED_Contrast,OLED_CMD); 

														 }															 
                             break;		

								 }									 
								 
							 } 
#endif			// 				 
			          break; 
								 
		case   'o':		  if(rx_buff[i+1]=='n')
		               {	
											nrf_gpio_pin_set(LED4);
		                 states[0]=rx_buff[0];               //¸üÐÂÒªÉÏ´«µÄ×´Ì¬Êý¾Ý
		                 states[1]=rx_buff[1];
		                 states[2]=0;
		                 change_state=1;                  //
									 }
									 else if(rx_buff[i+1]=='f')
									 {
                     if(rx_buff[i+2]=='f')
										 {
											  nrf_gpio_pin_clear(LED4);	
		                 states[0]=rx_buff[0];               //¸üÐÂÒªÉÏ´«µÄ×´Ì¬Êý¾Ý
		                 states[1]=rx_buff[1];
		                 states[2]=rx_buff[2];
		                 change_state=1;                  
										 } 
                     
									 }			
                  else
								  {
                    
							   	}									
									           
                 break;

    case   'O':     if(rx_buff[i+1]=='N')
		               {	
											nrf_gpio_pin_set(LED4);
		                 states[0]=rx_buff[0];               //¸üÐÂÒªÉÏ´«µÄ×´Ì¬Êý¾Ý
		                 states[1]=rx_buff[1];
		                 states[2]=0;
		                 change_state=1;                  //
									 }
									 else if(rx_buff[i+1]=='F')
									 {
                     if(rx_buff[i+2]=='F')
										 {
											  nrf_gpio_pin_clear(LED4);	
		                 states[0]=rx_buff[0];               //¸üÐÂÒªÉÏ´«µÄ×´Ì¬Êý¾Ý
		                 states[1]=rx_buff[1];
		                 states[2]=rx_buff[2];
		                 change_state=1;                  
										 } 
                     
									 }			
                  else
								  {
                    
							   	}							
			
                 break;			
								 
						
	       }
			 } 
      rx_length=0;
		 }			 


}






#endif


