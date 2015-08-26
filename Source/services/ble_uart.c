/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 *
 */

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include <ble_config.h>
#include <ble_uart.h>

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_uart      UART Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
    p_uart->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_uart		UART Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_uart->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_uart       Battery Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	uint16_t length = p_evt_write->len;
	if ((p_evt_write->handle == p_uart->recv_handles.value_handle) && 
		(p_uart->uart_recv_handler != NULL))
	{
		p_uart->uart_recv_handler(p_uart, p_evt_write->data, length);
	}
}

/**@brief Function for handling the BLE event.
 *
 * @param[in]   p_uart      UART Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_uart_on_ble_evt(ble_uart_t * p_uart, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_uart, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_uart, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_uart, p_ble_evt);
            break;
            
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding the Send characteristic.
 *
 * @param[in]   p_uart			UART Service structure.
 * @param[in]   p_uart_init		Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t uart_send_char_add(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_t    attr_char_value;

	memset(&cccd_md, 0, sizeof(cccd_md));

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
	cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    static uint8_t *pUserDesc = UART_SEND_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pUserDesc);
	char_md.char_user_desc_size = strlen((const char*)pUserDesc);
    char_md.p_char_user_desc  = pUserDesc;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_uart->uuid_type;
	ble_uuid.uuid = UART_UUID_SEND_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));
	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = BLE_UART_CHAR_BUFFER_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_UART_CHAR_BUFFER_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_uart->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_uart->send_handles);
}

/**@brief Function for adding the Recv characteristic.
 *
 * @param[in]   p_uart			UART Service structure.
 * @param[in]   p_uart_init		Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t uart_recv_char_add(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_t    attr_char_value;
    
    memset(&char_md, 0, sizeof(char_md));
    static uint8_t *pUserDesc = UART_RECV_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pUserDesc);
	char_md.char_user_desc_size = strlen((const char*)pUserDesc);
    char_md.p_char_user_desc  = pUserDesc;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_uart->uuid_type;
	ble_uuid.uuid = UART_UUID_RECV_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));
	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = BLE_UART_CHAR_BUFFER_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_UART_CHAR_BUFFER_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_uart->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_uart->recv_handles);
}


/**@brief Function for adding theUART Service.
 *
 * @param[in]   p_uart			UART Service structure.
 * @param[in]   p_uart_init		Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_uart_init(ble_uart_t * p_uart, const ble_uart_init_t * p_uart_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
	p_uart->uart_recv_handler = p_uart_init->uart_recv_handler;
    p_uart->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    ble_uuid128_t base_uuid = UART_UUID_BASE;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_uart->uuid_type);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	ble_uuid.type = p_uart->uuid_type;
	ble_uuid.uuid = UART_UUID_SERVICE;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_uart->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    // Add identification characteristics
	err_code = uart_send_char_add(p_uart, p_uart_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	err_code = uart_recv_char_add(p_uart, p_uart_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	return NRF_SUCCESS;
}

/**@brief Function for sending the packet by BLE notify through the send characteristic of UART Service.
 *
 * @param[in]   	p_uart      UART Service structure.
 * @param[in]   	p_data  	Pointer to the data buffer.
 * @param[in&out]   p_length  	Length of the data to send and the real length of data has been sent will be return.
 */
uint32_t ble_uart_send(ble_uart_t *p_uart, uint8_t* p_data, uint16_t* p_length)
{
	ble_gatts_hvx_params_t params;
	memset(&params, 0, sizeof(params));
	params.type = BLE_GATT_HVX_NOTIFICATION;
	params.handle = p_uart->send_handles.value_handle;
	params.p_data = p_data;
	params.p_len = p_length;
	return sd_ble_gatts_hvx(p_uart->conn_handle, &params);
}

