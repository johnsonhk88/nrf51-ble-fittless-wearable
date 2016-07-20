/***************
BLE APPS UART header file

**************************/


#ifndef     __BLE_APPS_UART
#define     __BLE_APPS_UART



#define ADVERTISING_LED_PIN_NO          LED0                                       //advertising led
#define CONNECTED_LED_PIN_NO            LED1                                       //connected LED

#define DEVICE_NAME                     "BLE_Health"                                //bluetooth name

#define APP_ADV_INTERVAL                64                                          //advertising time interval
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         //advertising timeout

#define APP_TIMER_PRESCALER             0                                           //RTC1 divide 
#define APP_TIMER_MAX_TIMERS            2                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               16                                          /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               60                                          /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           //m
#define CONN_SUP_TIMEOUT                400                                         //disconnect timeout
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (20 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define RTC_INTERVAL                      APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)        /**< Battery level measurement interval (ticks). This value corresponds to 120 seconds.  1 second*/
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_TIMEOUT               30                                          /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define START_STRING                    "Start Connect BLE..\n\r"                                /**< The string that will be sent over the UART when the application starts. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define BT_TR_TIME                      60
#define UART_SEND_AGAIN                 1




void BLE_UART_Init(void);
void power_manage(void);
void rtc_timeout_handler(void * p_context);
void BLE_Rx_Decode(uint8_t * rx_buff, uint16_t rx_length);


#endif


