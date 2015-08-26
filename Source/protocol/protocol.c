/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Protocol which is consist of BLE Stack, BLE UART Profile Layer(L0), Transport Control 
			Layer(L1) and Application Layer(L2).
 *
 * @note	This file is related with the context of chip and BLE stack implement while the Transport Control Layer and 
			Application Layer is independent of the context. So if the context is changed, please take good care of this
			file which may be changed carefully and specifically.
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */
#include <protocol.h>

static ble_uart_t*		m_p_uart;

/**@brief Function for sending packet to the BLE Profile Layer.
 *
 * @note This function, which is related with the context connects BLE Profile Layer
 *		 and Transport Control Layer.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 */
static uint32_t ble_send_handler(uint8_t* p_data, uint16_t length)
{
	uint32_t err_code = ble_uart_send(m_p_uart, p_data, &length);
	// The NRF_XXX and BLE_XXX may be different in different context.
	if (NRF_SUCCESS == err_code
		|| NRF_ERROR_BUSY == err_code
		|| BLE_ERROR_NO_TX_BUFFERS == err_code) {
		al_send_success();
		return AL_SUCCESS;
	}
	return AL_ERROR;
}

/**@brief Function for initializing Protocol module.
 *
 * @param[in]   p_init	Pointer to the TCL initiate structure.
 *
 */
void purifier_protocol_init(protocol_init_t* p_protocol_init)
{
	m_p_uart = p_protocol_init->p_uart;
	al_init_t init_al;
	init_al.ble_send_hander = ble_send_handler;								//add the function for sending packet 
	init_al.control_handler = al_process_control_packet;      //add the function for process control application packet 
	init_al.settting_handler = al_process_setting_packet;			//add the function for process setting application packet
	init_al.real_time_monitor_handler = al_process_rt_monitor_packet;		//add the function for monitor the real-time data.
	init_al.status_handler = al_process_status_packet;				//add the function for notify the hardware status to Android.
	p_protocol_init->p_al_init = &init_al;
	al_init(p_protocol_init->p_al_init);
}

