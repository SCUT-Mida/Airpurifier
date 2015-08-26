/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 *
 */

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include <ble_config.h>
#include <ble_id.h>


/**@brief Function for initiating the ID Sevice init structure.
 *
 * @param[in]   p_id_init	Information needed to initialize the service.
 * @param[in]   pID       	Product ID Number.
 */
void InitID(ble_id_init_t* pInit, uint32_t* pID)
{
	/*
	for (int i = 0; i < BLE_ID_SIZE; ++i)
		pInit->pID[i] = (uint8_t)(*pID >> (8 * i));
	*/
	pInit->pID = (uint8_t*)pID;
}

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_id       Identification Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_id_t * p_id, ble_evt_t * p_ble_evt)
{
    p_id->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_id       Identification Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_id_t * p_id, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_id->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the BLE event.
 *
 * @param[in]   p_id       Identification Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_id_on_ble_evt(ble_id_t * p_id, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_id, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_id, p_ble_evt);
            break;
            
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding the identification characteristic.
 *
 * @param[in]   p_id        Identification Service structure.
 * @param[in]   p_id_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t id_char_add(ble_id_t * p_id, const ble_id_init_t * p_id_init)
{
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_t    attr_char_value;
    
    memset(&char_md, 0, sizeof(char_md));
    static uint8_t *pUserDesc = ID_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pUserDesc);
	char_md.char_user_desc_size = strlen((const char*)pUserDesc);
    char_md.p_char_user_desc  = pUserDesc;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_id->uuid_type;
	ble_uuid.uuid = ID_UUID_ID_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));
	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_USER;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = BLE_ID_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_ID_SIZE;
    attr_char_value.p_value      = p_id_init->pID;
    
    return sd_ble_gatts_characteristic_add(p_id->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_id->id_handles);
}

/**@brief Function for adding the Identification Service.
 *
 * @param[in]   p_id        Identification Service structure.
 * @param[in]   p_id_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_id_init(ble_id_t * p_id, const ble_id_init_t * p_id_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_id->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    ble_uuid128_t base_uuid = ID_UUID_BASE;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_id->uuid_type);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	ble_uuid.type = p_id->uuid_type;
	ble_uuid.uuid = ID_UUID_SERVICE;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_id->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    // Add identification characteristics
	err_code = id_char_add(p_id, p_id_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	return NRF_SUCCESS;
}
