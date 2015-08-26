/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 *
 */

#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include <car_air_purifier.h>
#include <ble_instant_air_quality.h>

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_iaq       Instant Air Quality Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_iaq_t * p_iaq, ble_evt_t * p_ble_evt)
{
    p_iaq->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_iaq       Instant Air Quality Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_iaq_t * p_iaq, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_iaq->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the BLE event.
 *
 * @param[in]   p_iaq       Instant Air Quality Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_iaq_on_ble_evt(ble_iaq_t * p_iaq, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_iaq, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_iaq, p_ble_evt);
            break;
            
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding the PM2.5 characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_pm25_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_PM25_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_PM25_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->pm25_handles);
}

/**@brief Function for adding the TVOC characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_tvoc_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_TVOC_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_TVOC_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->tvoc_handles);
}

/**@brief Function for adding the Temperature characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_temp_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_TEMP_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_TEMP_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->temp_handles);
}

/**@brief Function for adding the Humidity characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_humi_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_HUMI_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_HUMI_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->humi_handles);
}

/**@brief Function for adding the reserved1 characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_reserved1_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_RESERVED1_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_RESERVED1_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->reserved1_handles);
}

/**@brief Function for adding the reserved2 characteristic.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t instant_reserved2_char_add(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
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
    static uint8_t *pButtonDes = IAQ_RESERVED2_CHAR_USER_DESC;
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
	char_md.char_user_desc_max_size = strlen((const char*)pButtonDes);
	char_md.char_user_desc_size = strlen((const char*)pButtonDes);
    char_md.p_char_user_desc  = pButtonDes;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_RESERVED2_CHAR;
    
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
    attr_char_value.init_len     = BLE_SENSOR_SIZE;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = BLE_SENSOR_SIZE;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_iaq->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_iaq->reserved2_handles);
}

/**@brief Function for adding the Instant Air Quality Service Service.
 *
 * @param[in]   p_iaq        Instant Air Quality Service Service structure.
 * @param[in]   p_iaq_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_iaq_init(ble_iaq_t * p_iaq, const ble_iaq_init_t * p_iaq_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_iaq->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    ble_uuid128_t base_uuid = IAQ_UUID_BASE;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_iaq->uuid_type);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	ble_uuid.type = p_iaq->uuid_type;
	ble_uuid.uuid = IAQ_UUID_SERVICE;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_iaq->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    // Add identification characteristics
	err_code = instant_pm25_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	err_code = instant_tvoc_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	err_code = instant_temp_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	err_code = instant_humi_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	//err_code = instant_reserved1_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	err_code = instant_reserved2_char_add(p_iaq, p_iaq_init);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	return NRF_SUCCESS;
}
