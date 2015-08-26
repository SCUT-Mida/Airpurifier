/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 *
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "app_scheduler.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "ble_error_log.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "ble_debug_assert_handler.h"
#include "pstorage.h"

// Headers of Sevices
#include <ble_config.h>
#include <ble_id.h>
#include <ble_uart.h>
// Common Header for Car Air Purfier
#include <car_air_purifier.h>
// Headers of Buffer Queue
#include <rx_buffer_queue.h> 



// Header of Car Air Purifier Communication Protocol
#include <protocol.h>

#define ADVERTISING_LED_PIN_NO          APP_ADVERTISING_LED                           /**< Is on when device is advertising. */
#define CONNECTED_LED_PIN_NO            APP_CONNECT_LED                             /**< Is on when device has connected. */
#define ASSERT_LED_PIN_NO               APP_ASSERT_LED                            		/**< Is on when application has asserted. */

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_id_t							m_id;
static uint32_t							m_product_id = CAR_AIR_PURIFIER_SERIAL_NUMBER;																				
static ble_uart_t						m_uart;
static app_timer_id_t					m_adv_timer_id = 0;
static app_timer_id_t					m_sample_timer_id = 1;
static rx_buffer_elem_t     al_recv_buffer_queue[RX_BUFFER_QUEUE_LENGTH];             /*The buffer queue for AL receive the 'write' data.<Created by @Mida 2015-6-2>***/
static SensorData 					m_sensor;
static uint8_t 							SampleTickTack = 0;

static ble_gap_adv_params_t				adv_params;

#define ADV_TIMER_TICKS					APP_TIMER_TICKS(APP_ADV_TIMEOUT_IN_SECONDS * 500, APP_TIMER_PRESCALER)
#define SAMPLE_TIMER_TICKS			APP_TIMER_TICKS(SENSOR_SAMPLE_TIMER_INTERVAL * 500, APP_TIMER_PRESCALER)

// YOUR_JOB: Modify these according to requirements (e.g. if other event types are to pass through
//           the scheduler).
#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */

// Persistent storage system event handler
void pstorage_sys_event_handler (uint32_t p_evt);

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
//    nrf_gpio_pin_set(ASSERT_LED_PIN_NO);

    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    ble_debug_assert_handler(error_code, line_num, p_file_name);

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


/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
/*
// YOUR_JOB: Uncomment this function and make it handle error situations sent back to your
//           application by the services it uses.
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
} */

/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    sd_ble_gap_adv_start(&adv_params);
}

/**@brief Function for handling time-out of timer.
 *
 * @details Initializes the timer module.
 */
static void adv_timeout_handler(void * p_context)
{
	sd_ble_gap_adv_stop();
	advertising_start();
}

/**@brief Function for starting advertisement timer.
*/
static void adv_timers_start(void)
{
    uint32_t err_code = app_timer_start(m_adv_timer_id, ADV_TIMER_TICKS, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for stopping advertisement timer.
*/
static void adv_timers_stop(void)
{
    uint32_t err_code = app_timer_stop(m_adv_timer_id);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling sample-start of timer.
 *<Modify by @Mida 2015-8-4>
 * @note The interval value is 3s.
 * @details Sampling the all sensor.And Pass to the AL.
 */
static void sample_start_handler(void * p_context)    //Every (3*5=)15s sample one time ;every 3s change one lcd page.
{
	if(0 == SampleTickTack)
	{
		GetSensorData(&m_sensor);
		pass_to_al_sensor_data(m_sensor);
	}
	LcdDisplaySensorData(m_sensor,SampleTickTack);
	if(++SampleTickTack >= LCD_SHOW_PAGE )
			SampleTickTack = 0;
}

/**@brief Function for starting tcl timer.
*/
static void sample_timers_start(void)
{
    uint32_t err_code = app_timer_start(m_sample_timer_id, SAMPLE_TIMER_TICKS, NULL);
    APP_ERROR_CHECK(err_code);
		sample_start_handler(NULL);
}

/**@brief Function for stopping tcl timer.
*/
static void sample_timers_stop(void)
{
    uint32_t err_code = app_timer_stop(m_sample_timer_id);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one. */
    uint32_t err_code = app_timer_create(&m_adv_timer_id, APP_TIMER_MODE_REPEATED, adv_timeout_handler);
    APP_ERROR_CHECK(err_code);
		
		err_code = app_timer_create(&m_sample_timer_id, APP_TIMER_MODE_REPEATED, sample_start_handler);
    APP_ERROR_CHECK(err_code);
	
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
    APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
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

    // YOUR_JOB: Use UUIDs for service(s) used in your application.
    ble_uuid_t adv_uuids[] = {{ID_UUID_SERVICE,		m_id.uuid_type},};
    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
	
    //advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    //advdata.uuids_complete.p_uuids  = adv_uuids;
	memset(&scanrsp, 0, sizeof(scanrsp));
	scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
	
	memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
}

static void add_id_service()
{
  uint32_t err_code;
	ble_id_init_t id_init; // The content of this structure is dummy that should be ignored.
	InitID(&id_init, &m_product_id);
	err_code = ble_id_init(&m_id, &id_init);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the received packet through the recv characteristic of UART Service.
 *
 * @param[in]   p_uart      UART Service structure.
 * @param[in]   p_data  	Pointer to the data buffer.
 * @param[in]   length  	Length of the data received.
 */
static void ble_uart_recv_handler(ble_uart_t* p_uart, uint8_t* p_data, uint16_t length)
{
	add_rx_buffer_to_queue(p_data,length);               //add to the rx buffer queue
}

static void add_uart_service()
{
  uint32_t err_code;
	ble_uart_init_t uart_init; 
	uart_init.uart_recv_handler = ble_uart_recv_handler;
	err_code = ble_uart_init(&m_uart, &uart_init);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
	// Add ID Sevice to BLE Stack
	add_id_service();
	add_uart_service();
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


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
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


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

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

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
			adv_timers_stop();
			sample_timers_start();               //<Add by @Mida 2015-6-9>
			
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
            //nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            /* YOUR_JOB: Uncomment this part if you are using the app_button module to handle button
                         events (assuming that the button events are only needed in connected
                         state). If this is uncommented out here,
                            1. Make sure that app_button_disable() is called when handling
                               BLE_GAP_EVT_DISCONNECTED below.
                            2. Make sure the app_button module is initialized.
			*/
            //err_code = app_button_enable();
            //APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            /* YOUR_JOB: Uncomment this part if you are using the app_button module to handle button
                         events. This should be done to save power when not connected
                         to a peer.
            */
            //err_code = app_button_disable();
            //APP_ERROR_CHECK(err_code);
			
			sample_timers_stop();
      adv_timers_start();			
			advertising_start();						//<Add by @Mida 2015-6-9>
			
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
         
                // Go to system-off mode (this function will not return; wakeup will cause a reset)                
                err_code = sd_power_system_off();
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
	// Dispatch the event to the Sevice Event Handler.
    ble_id_on_ble_evt(&m_id, p_ble_evt); // ID Sevice Event Handler.
    ble_uart_on_ble_evt(&m_uart, p_ble_evt); // Uart Sevice Event Handler.
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    
    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for initializing the GPIOTE handler module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Transport Protocol module.
 */
static void protocol_init(void)
{
	protocol_init_t  init;	
	init.p_uart = &m_uart;
	purifier_protocol_init(&init);
}
 
/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize	
    timers_init();
    gpiote_init();
    ble_stack_init();
    scheduler_init();    
    gap_params_init();
			
	// Attention that the order of the following two init functions must be fixed.
	// {
    services_init();
    advertising_init();
	// }
    conn_params_init();
    sec_params_init();
			
		init_rx_buffer_queue_evt(al_recv_buffer_queue);
		InitAirPurifier();
	
	// Init the Transport Protocol Module.
		protocol_init();
	
    // Start execution
    adv_timers_start();
    advertising_start();
	
    // Enter main loop
		LcdDisplayInit();
    for (;;)
    {
        app_sched_execute();
				rx_buffer_queue_evt_schedule();
				SmartAdapt(m_sensor);			//<2015-8-7>For testing LEDs
        //power_manage();
    }
}

/**
 * @}
 */
