/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 * 
 */

/** @file
 *
 * @brief Instant Air Quality Service module.
 *
 */

#ifndef BLE_IAQ_H__
#define BLE_IAQ_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

// UUID_BASE	7C 79 xx xx - 34 D8 - 7C 7B - 8F 33 - 54 88 C7 84 5D 2A
#define IAQ_UUID_BASE {0x2A, 0x5D, 0x84, 0xC7, 0x88, 0x54, 0x33, 0x8F, 0x7B, 0x7C, 0xD8, 0x34, 0x00, 0x00, 0x79, 0x7C}
#define IAQ_UUID_SERVICE 			0x0001
#define IAQ_UUID_PM25_CHAR 			0x0002
#define IAQ_UUID_TVOC_CHAR 			0x0003
#define IAQ_UUID_TEMP_CHAR 			0x0004
#define IAQ_UUID_HUMI_CHAR 			0x0005
#define IAQ_UUID_RESERVED1_CHAR		0x0006
#define IAQ_UUID_RESERVED2_CHAR		0x0007

#define IAQ_PM25_CHAR_USER_DESC			(uint8_t*)"Instant PM2.5 Characteristic"
#define IAQ_TVOC_CHAR_USER_DESC			(uint8_t*)"Instant TVOC Characteristic"
#define IAQ_TEMP_CHAR_USER_DESC			(uint8_t*)"Instant Temperature Characteristic"
#define IAQ_HUMI_CHAR_USER_DESC			(uint8_t*)"Instant Humidity Characteristic"
#define IAQ_RESERVED1_CHAR_USER_DESC	(uint8_t*)"Instant Reserved1 Characteristic"
#define IAQ_RESERVED2_CHAR_USER_DESC	(uint8_t*)"Instant Reserved2 Characteristic"

// Forward declaration of the ble_instant_air_quality_t type. 
typedef struct ble_instant_air_quality_s ble_iaq_t;

/**@brief Instant Air Quality Service initialize structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
	uint8_t	dummy; // The content of this initialization structure is empty which may be filled in the future.
} ble_iaq_init_t;

/**@brief Instant Air Quality Service structure. This contains various status information for the service. */
typedef struct ble_instant_air_quality_s
{
    uint8_t						uuid_type;
	uint16_t					service_handle;		/**< Handle of Instant Air Quality Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t	pm25_handles;		/**< Handles related to the Instant PM2.5 characteristic. */
    ble_gatts_char_handles_t	tvoc_handles;		/**< Handles related to the Instant TVOC characteristic. */
    ble_gatts_char_handles_t	temp_handles;		/**< Handles related to the Instant Temperature characteristic. */
    ble_gatts_char_handles_t	humi_handles;		/**< Handles related to the Instant Humidity characteristic. */
    ble_gatts_char_handles_t	reserved1_handles;	/**< Handles related to the Instant Reserved1 characteristic. */
    ble_gatts_char_handles_t	reserved2_handles;	/**< Handles related to the Instant Reserved2 characteristic. */
    uint16_t					conn_handle;		/**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
} ble_iaq_t;

/**@brief Function for initializing the Instant Air Quality Service.
 *
 * @param[out]  p_iaq       Instant Air Quality Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_iaq_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_iaq_init(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Instant Air Quality Service.
 *
 *
 * @param[in]   p_iaq      Instant Air Quality Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_iaq_on_ble_evt(ble_iaq_t * p_iaq, ble_evt_t * p_ble_evt);


uint32_t ble_iaq_on_pm25_change(ble_iaq_t *p_iaq, uint8_t button_state);


uint32_t ble_iaq_on_tvoc_change(ble_iaq_t *p_iaq, uint8_t button_state);


uint32_t ble_iaq_on_temp_change(ble_iaq_t *p_iaq, uint8_t button_state);


uint32_t ble_iaq_on_humi_change(ble_iaq_t *p_iaq, uint8_t button_state);


uint32_t ble_iaq_on_reserved1_change(ble_iaq_t *p_iaq, uint8_t button_state);


uint32_t ble_iaq_on_reserved2_change(ble_iaq_t *p_iaq, uint8_t button_state);

#endif // BLE_ID_H__

/** @} */
