/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 * 
 */

/** @file
 *
 * @brief Identification Service module.
 *
 */

#ifndef BLE_ID_H__
#define BLE_ID_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

// UUID_BASE	FF CC xx xx - 99 9F - 64 2B - 27 A0 - 49 39 5C 1F 58 E3
#define ID_UUID_BASE {0xE3, 0x58, 0x1F, 0x5C, 0x39, 0x49, 0xA0, 0x27, 0x2B, 0x64, 0x9F, 0x99, 0x00, 0x00, 0xCC, 0xFF}
#define ID_UUID_SERVICE 0x0001
#define ID_UUID_ID_CHAR 0x0002
#define ID_UUID_TMP_CHAR 0x0003

#define ID_CHAR_USER_DESC	(uint8_t*)"Identification Characteristic"

// Forward declaration of the ble_id_t type. 
typedef struct ble_id_s ble_id_t;

/**@brief Identification Service initiate structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
	uint8_t*	pID;
} ble_id_init_t;

/**@brief Identification Service structure. This contains various status information for the service. */
typedef struct ble_id_s
{
    uint8_t						uuid_type;
		uint16_t					service_handle;		/**< Handle of Identification Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t	id_handles;			/**< Handles related to the Identification characteristic. */
    ble_gatts_char_handles_t	tmp_handles;
		uint16_t					conn_handle;		/**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
} ble_id_t;

/**@brief Function for initializing the Identification Service.
 *
 * @param[out]  p_id       Identification Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_id_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_id_init(ble_id_t * p_id, const ble_id_init_t * p_id_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Identification Service.
 *
 * @param[in]   p_id      Identification Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_id_on_ble_evt(ble_id_t * p_id, ble_evt_t * p_ble_evt);

/**@brief Function for filling the ID Sevice init structure.
 *
 * @param[in]   p_id_init	Information needed to initialize the service.
 * @param[in]   pID       	Product ID Number.
 */
void InitID(ble_id_init_t* pInit, uint32_t* pID);

#endif // BLE_ID_H__

/** @} */
