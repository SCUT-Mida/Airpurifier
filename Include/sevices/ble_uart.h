/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 * 
 */

/** @file
 *
 * @brief UART Service module.
 *
 */

#ifndef BLE_UART_H__
#define BLE_UART_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"	

// UUID_BASE	CF 20 xx xx - DC 23 - ED 56 - B9 FB - FD 52 FC 28 53 61
#define UART_UUID_BASE {0x61, 0x53, 0x28, 0xFC, 0x52, 0xFD, 0xFB, 0xB9, 0x56, 0xED, 0x23, 0xDC, 0x00, 0x00, 0x20, 0xCF}
#define UART_UUID_SERVICE 				0x0001
#define UART_UUID_SEND_CHAR 			0x0002
#define UART_UUID_RECV_CHAR 			0x0003

#define UART_SEND_CHAR_USER_DESC		(uint8_t*)"Characteristic for UART Send"
#define UART_RECV_CHAR_USER_DESC		(uint8_t*)"Characteristic for UART Receive"

// Forward declaration of the ble_uart_t type. 
typedef struct ble_uart_s ble_uart_t;

/**@brief Function for handling the received packet through the recv characteristic of UART Service.
 *
 * @param[in]   p_uart      UART Service structure.
 * @param[in]   p_data  	Pointer to the data buffer.
 * @param[in]   length  	Length of the data received.
 */
typedef void (*ble_uart_recv_handler_t) (ble_uart_t* p_uart, uint8_t* p_data, uint16_t length);

/**@brief UART Service initialize structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
	ble_uart_recv_handler_t   uart_recv_handler;	/**< Event handler to be called for handling events in the UART Service.*/ 
} ble_uart_init_t;

/**@brief UART Service structure. This contains various status information for the service. */
typedef struct ble_uart_s
{
    uint8_t						uuid_type;
		uint16_t					service_handle;		/**< Handle of UART Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t	send_handles;		/**< Handles related to the Instant PM2.5 characteristic. */
    ble_gatts_char_handles_t	recv_handles;		/**< Handles related to the Instant TVOC characteristic. */
    uint16_t					conn_handle;		/**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    ble_uart_recv_handler_t		uart_recv_handler;     /**< Event handler to be called for handling events in the Service. */
} ble_uart_t;

/**@brief Function for initializing the UART Service.
 *
 * @param[out]  p_uart       UART Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_uart_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_uart_init(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the UART Service.
 *
 *
 * @param[in]   p_uart      UART Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_uart_on_ble_evt(ble_uart_t * p_uart, ble_evt_t * p_ble_evt);

/**@brief Function for sending the packet by BLE notify through the send characteristic of UART Service.
 *
 * @param[in]   	p_uart      UART Service structure.
 * @param[in]   	p_data  	Pointer to the data buffer.
 * @param[in&out]   p_length  	Length of the data to send and the real length of data has been sent will be return.
 */
uint32_t ble_uart_send(ble_uart_t *p_uart, uint8_t* p_data, uint16_t* p_length);

#endif // BLE_ID_H__

/** @} */
