/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Transport Control Layer Protocol which abbreviated TCL when the L0 means the BLE Profile layer 
 *			to access the BLE Stack and the L2 means the Application Layer which is the most upper layer.
 *
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */
#include <stdbool.h>
#include <string.h>
#include <transport.h>
#include <application.h>

// The following variable holds the handler of BLE Module for sending data.
static ble_send_handler_t			ble_send_handler;

// The following variable holds the handler of Application Module for receiving packet 
static al_recv_handler_t					al_recv_handler;
static al_send_failed_handler_t		al_send_failed_handler;
static al_send_success_handler_t	al_send_success_handler;

// The following environment is set and saved for one transmission which are derived from the Application Layer.
// {
static uint8_t*						m_p_send_data; // Pointer to the data will be sent.
static uint16_t						m_send_data_length_left; // The byte number left of the data from L2.

static uint8_t						m_send_sub_packet_number; // The number of the sub-packet.
static uint8_t						m_send_sequence_id; // The id of the sub-packet.
static tcl_send_status_t			m_send_status;
static tcl_timer_t					m_send_sub_packet_timer;

static tcl_packet_t					m_send_packet; // Data packet to sent.
static tcl_ack_packet_t				m_send_ack_packet; // ACK packet to sent.
// }

// The following environment is saved for one transmission which are from the BLE Profile Layer.
// {
static uint8_t						m_recv_payload[TCL_PAYLOAD_MTU];
static uint16_t						m_recv_data_length_count;
static tcl_timer_t				m_recv_packet_timer;
// }

/**@brief Function for starting the timer.
 *
 * @param[in]   p_timer		Pointer to the Timer.
 */
static void tcl_timer_start(tcl_timer_t* p_timer)
{
	p_timer->status = TCL_TIMER_START;
}

/**@brief Function for stopping the timer.
 *
 * @param[in]   p_timer		Pointer to the Timer.
 */
static void tcl_timer_stop(tcl_timer_t* p_timer)
{
	p_timer->count = 0;
	p_timer->status = TCL_TIMER_STOP;
}

/**@brief Function for calculate the checksum of the packet.
 *
 * @param[in]   p_packet		Pointer to the TCL packet.
 * @param[in]   length			Pointer to the TCL packet.
 *
 * @return Checksum of the packet.
 */
static uint16_t checksum(uint16_t* p_data, uint16_t length)
{
	uint16_t ret = 0;
	/*********<Modify by Mida>***************/
	if((length%2)!=0)               
		length += 1;
	/*********<Modify by Mida>***************/
	length /= 2;
	for (uint16_t i = 0; i < length; ++i)
		ret += p_data[i];
	return ~ret;
}

/**@brief Function for checking whether is the last sub-packet will be sent.
 *
 * @param[in]   p_packet		Pointer to the TCL packet.
 *
 * @return true If it is the last sub-packet, false if not.
 */
static bool tcl_is_ack_packet(tcl_packet_t* p_packet)
{
	return TCL_CHECK_ACK_FLAG(p_packet->header.flag);
}

/**@brief Function for initiating the send environment.
 *
 */
static void tcl_init_send_env(void)
{
	m_p_send_data = NULL;
	m_send_data_length_left = 0;
	m_send_sub_packet_number = 0;
	m_send_sequence_id = 0;
}

/**@brief Function for handling situation of failing to send packet.
 *
 */
static void tcl_send_failed(void)
{
	m_send_status = TCL_SEND_STATUS_FAILED;
	al_send_failed_handler(); // Notice AL.
	tcl_init_send_env(); // Initiate sending environment.
}

/**@brief Function for handling situation of successfully sending packet.
 *
 */
static void tcl_send_success(void)
{
	m_send_status = TCL_SEND_STATUS_SUCCESS;
	al_send_success_handler(); // Notice AL.
	tcl_init_send_env(); // Initiate sending environment.
}

/**@brief Function for initiating the TCL Header Structure.
 *
 * @param[in]   p_header		Pointer to the TCL header.
 *
 */
static void tcl_header_init(tcl_header_t* p_header)
{
	p_header->magic_number = TCL_HEADER_MAGIC_NUMBER;
	p_header->version = TCL_PROTOCOL_VERSION;
	p_header->payload_length = 0;
	p_header->sequence_id = 0;
	p_header->flag = 0;
	p_header->check_sum = 0;
}

/**@brief Function for initiating the receive environment.
 *
 */
static void tcl_init_recv_env(void)
{
	m_recv_data_length_count = 0;
	memset(&m_recv_payload, 0, TCL_PAYLOAD_MTU);
}

/**@brief Function for checking whether is the last sub-packet will be sent.
 *
 * @return true If it is the last sub-packet, false if not.
 */
static bool tcl_is_last_packet(void)
{
	return m_send_sequence_id == (m_send_sub_packet_number - 1);
}

/**@brief Function for loading the data to the header of sub-packet.
 *
 */
static void tcl_load_packet_header(void)
{
	m_send_packet.header.sequence_id = m_send_sequence_id;
	m_send_packet.header.check_sum = 0;
}

/**@brief Function for loading the data to the payload of sub-packet.
 *
 * @param[in]   length  		Real length of the payload.
 */
static uint16_t tcl_load_packet_payload(void)
{
	uint16_t length = BLE_UART_PAYLOAD_MTU;
	if (tcl_is_last_packet())         /*<Modify by Mida>2015-5-22*/
	{
		uint16_t i,left_length;
		left_length = m_send_data_length_left;
		for (i = 0; i < length; ++i)
			m_send_packet.payload[i] = m_p_send_data[i];
		for(i = left_length; i< BLE_UART_PAYLOAD_MTU ;++i)
			m_send_packet.payload[i] = 0;
	}
	else
	{
		for (uint16_t i = 0; i < length; ++i)
			m_send_packet.payload[i] = m_p_send_data[i];
	}
	return length;
}

/**@brief Function for loading the checksum of sub-packet.
 *
 * @param[in]   length  		Real length of the packet.
 */
static void tcl_load_packet_checksum(uint16_t length)
{
	m_send_packet.header.check_sum = checksum((uint16_t*)&m_send_packet, length);
}

/**@brief Function for loading the flag of sub-packet.
 *<Add by @Mida 2015-6-18>
 * @param[in]   p_data  		The pointer to the sending data.
 */
static void tcl_load_packet_flag(uint8_t * p_data)
{
	al_packet_t * al_packet = (al_packet_t *)p_data;
	uint8_t al_packet_commandID = al_packet->header.command_id;
	m_send_packet.header.flag = (m_send_packet.header.flag & 0xC3)|(al_packet_commandID << 2);
}

/**@brief Function for loading the data to the sub-packet.
 *
 * @return length				Real length of the packet.
 */
static uint16_t tcl_load_packet(void)
{
	tcl_load_packet_header();
	uint16_t length = TCL_HEADER_LENGTH + tcl_load_packet_payload();
	tcl_load_packet_checksum(length);
	return length;
}

/**@brief Function for sending sub-packet.
 *
 * @return @ref TCL_SUCCESS		Successfully sent the sub-packet.
 * @return @ref TCL_ERROR		Common failed.
 */
static uint32_t tcl_send_sub_packet(void)
{
	uint16_t length = tcl_load_packet();
	uint32_t err_code = ble_send_handler((uint8_t*)&m_send_packet, length);
	if (BLE_SUCCESS != err_code) {
		// Reaching here means BLE is busy or error.
		tcl_send_failed(); // Failed to send packet.
		return TCL_ERROR;
	}
	tcl_timer_start(&m_send_sub_packet_timer);	
	return TCL_SUCCESS;
}

/**@brief Function for sending sub-packet.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 */
static void tcl_set_send_env(uint8_t* p_data, uint16_t length)
{
	m_send_packet.header.payload_length = length;
	tcl_load_packet_flag(p_data);               //Loading the flag of tcl header.<Add by @Mida 2015-6-18>
	m_p_send_data = p_data;
	m_send_data_length_left = length;
	m_send_sub_packet_number = TCL_CALC_SUB_PACKET_NUMBER(length);
	m_send_sequence_id = 0;
	m_send_status = TCL_SEND_STATUS_SENDING;
}

/**@brief Function for getting the sending status.
 *
 * @note The Application Layer can check the packet whether has been sent by the status.
 *
 */
tcl_send_status_t tcl_send_status(void)
{
	return m_send_status;
}

/**@brief Function for sending packet from Application Layer to BLE Profile Layer.
 *
 * @note The Application layer should keep the data buffer until sending finishes.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref TCL_SUCCESS		Successfully sent the packet.
 * @return @ref TCL_ERROR		Common failed.
 * @return @ref TCL_WAIT		Wait and send again.
 */
uint32_t tcl_send_packet(uint8_t* p_data, uint16_t length)
{
	if (TCL_SEND_STATUS_SENDING == tcl_send_status())
		return TCL_WAIT;
	tcl_set_send_env(p_data, length);
	if(length <= BLE_UART_PAYLOAD_MTU)         //<Add by @Mida 2015-6-22>
	{
		uint32_t err_code;
		err_code = tcl_send_sub_packet();
		if(err_code == TCL_SUCCESS)
			tcl_send_success();   // Notice AL 
		return err_code;
	}
	else	
		return tcl_send_sub_packet();
}

/**@brief Function for sending ACK packet through the Transport Control Layer.
 *
 * @note Except the ACK flag bit, other flag bits should be set before calling this function.
 *
 * @return @ref TCL_SUCCESS		Successfully sent the ACK packet.
 * @return @ref TCL_ERROR		Common failed.
 */
static uint32_t tcl_send_ack_packet()
{
	TCL_SET_ACK_FLAG(m_send_ack_packet.header.flag);
	m_send_ack_packet.header.check_sum = 0;
	m_send_ack_packet.header.check_sum = checksum((uint16_t*)&m_send_ack_packet, TCL_HEADER_LENGTH);
	/******************<Motify by Mida>*************************/
	memset(&m_send_packet.payload, 0, BLE_UART_PAYLOAD_MTU);
	uint32_t err_code = ble_send_handler((uint8_t*)&m_send_ack_packet, BLE_UART_MTU);
	/******************<Motify by Mida>*************************/
	if (BLE_SUCCESS != err_code) {
		// Reaching here means BLE is busy or error.
		tcl_send_failed(); // Failed to send packet.
		return TCL_ERROR;
	}
	return TCL_SUCCESS;
}

/**@brief Function for process received ACK.
 *
 * @param[in]   flag  			Flag of the ACK packet.
 *
 * @note When receive the ACK packet we should check the flag firstly.
 * @note If the flag indicate that there is something wrong we should send the same packet again.

 * @return @ref TCL_SUCCESS		Successfully sent the sub-packet.
 * @return @ref TCL_ERROR		Common failed.
 */
static uint32_t tcl_process_recv_ack(tcl_packet_t p_packet)
{
	uint8_t flag = p_packet.header.flag;
	uint8_t sequence_id = p_packet.header.sequence_id;
	m_send_sequence_id = sequence_id + 1;
	tcl_timer_stop(&m_send_sub_packet_timer);
	if (true == TCL_CHECK_ERR_FLAG(flag)) {
		return tcl_send_sub_packet(); // Just send again the previous sub-packet.
	} else {
		if (m_send_sequence_id < m_send_sub_packet_number) { // Check whether is ACK for the last sub-packet.
			m_p_send_data += BLE_UART_PAYLOAD_MTU;
			m_send_data_length_left -= BLE_UART_PAYLOAD_MTU;
			return tcl_send_sub_packet();
		} 
		if (m_send_sequence_id == m_send_sub_packet_number) { // ACK for the last sub-packet.
			tcl_send_success(); // Notice AL.	
			nrf_gpio_pin_toggle(SEND_PACKET_FINISH_LED_PIN_NO);    // <Modify by Mida>2015-6-15
		} else {
			return TCL_ERROR;
		}
	}
	/*
	if (true == TCL_CHECK_ERR_FLAG(flag)) {
		return tcl_send_sub_packet(); // Just send again the previous sub-packet.
	} else {
		if (++m_send_sequence_id < m_send_sub_packet_number) { // Check whether is ACK for the last sub-packet.
			m_p_send_data += BLE_UART_PAYLOAD_MTU;
			m_send_data_length_left -= BLE_UART_PAYLOAD_MTU;
			return tcl_send_sub_packet();
		} 
		if (m_send_sequence_id == m_send_sub_packet_number) { // ACK for the last sub-packet.
			tcl_send_success(); // Notice AL.	
			nrf_gpio_pin_set(SEND_PACKET_FINISH_LED_PIN_NO);    // <Modify by Mida>2015-6-15
		} else {
			return TCL_ERROR;
		}
	}*/
	return TCL_SUCCESS;
}

/**@brief Function for downloading the payload to buffer.
 *
 * @param[in]   sequence_id			Sequence ID of this packet.
 * @param[in]   p_payload			Pointer to the payload of data packet.
 * @param[in]   length  			Length of the payload.
 *
 */
static void tcl_download_recv_packet_payload(uint8_t sequence_id, uint8_t* p_payload, uint16_t length)
{
	uint8_t* p_buffer = m_recv_payload + ((sequence_id) * BLE_UART_PAYLOAD_MTU);
	for (uint16_t i = 0; i < length; ++i) {
		if (0 == p_buffer[i]) {
			++m_recv_data_length_count;
			p_buffer[i] = p_payload[i];
		}
	}
}

/**@brief Function for processing received packet from the BLE Profile Layer.
 *
 * @param[in]   p_packet			Pointer to the TCL packet.
 * @param[in]   length  			Length of the data.
 *
 * @return @ref TCL_SUCCESS				Successfully sent the packet.
 * @return @ref TCL_ERROR				Common failed.
 * @return @ref TCL_ERROR_DATA_SIZE		Exceed the limit of data size.
 */
static uint32_t tcl_process_recv_packet(tcl_packet_t* p_packet, uint16_t length)
{
	uint16_t total_length = p_packet->header.payload_length; 
	if (total_length > TCL_PAYLOAD_MTU)
		return TCL_ERROR_DATA_SIZE;
	uint16_t payload_length = length - TCL_HEADER_LENGTH;
	tcl_download_recv_packet_payload(p_packet->header.sequence_id, p_packet->payload, payload_length);
	if (m_recv_data_length_count > total_length) { // Error
		tcl_init_recv_env();
		return TCL_ERROR;
	}
	if (m_recv_data_length_count == total_length) { // Successfully received the whole application packet
		// call the application packet handler of Application Layer.
		uint32_t err_code = al_recv_handler(m_recv_payload, total_length);
		// Process the error from Application Layer. Initiate the received environment only when succeed.
		tcl_init_recv_env();
		tcl_timer_stop(&m_recv_packet_timer);
	} else {
		tcl_timer_start(&m_recv_packet_timer); // Start receiving timer for waiting the next packet.
	}
	// Continue to receive other sub-packets.
	return TCL_SUCCESS;
}


/**@brief Function for checking the packet whether is a variable packet for this protocol.
 *
 * @note This function will check the magic number, the version and the checksum
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref TCL_SUCCESS					The packet is valid.
 * @return @ref TCL_ERROR_MAGIC_NUMBER		The magic number is wrong.
 * @return @ref TCL_ERROR_VERSION			The version is wrong.
 * @return @ref TCL_ERROR_CHECK_SUM			The checksum is wrong.
 */
static uint32_t tcl_check_recv_packet(uint8_t* p_data, uint16_t length)
{
	tcl_packet_t* p_packet = (tcl_packet_t*)p_data;
	if (TCL_HEADER_MAGIC_NUMBER != p_packet->header.magic_number) {
		return TCL_ERROR_MAGIC_NUMBER;
	}
	if (TCL_PROTOCOL_VERSION != p_packet->header.version) {
		return TCL_ERROR_VERSION;
	}
	if (0x0000 != checksum((uint16_t*)p_data, length)) {
		return TCL_ERROR_CHECK_SUM;
	}
	return TCL_SUCCESS;
}

/**@brief Function for received packet from the BLE Profile Layer.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref TCL_SUCCESS		Successfully sent the packet.
 * @return @ref TCL_ERROR		Common failed.
 */
uint32_t tcl_recv_packet(uint8_t* p_data, uint16_t length)
{
	uint32_t err_code = tcl_check_recv_packet(p_data, length);
	TCL_UNSET_FLAG(m_send_ack_packet.header.flag);
	if (TCL_SUCCESS != err_code) {
		TCL_SET_ERR_FLAG(m_send_ack_packet.header.flag);
		return tcl_send_ack_packet();
	}
	//tcl_send_ack_packet();            /*<Modified by @Mida>TCL recv packet don't need to reply an ack packet at once!*/
	tcl_packet_t* p_packet = (tcl_packet_t*)p_data;
	if (true == tcl_is_ack_packet(p_packet)) {
		 // Received a ACK packet.
		 return tcl_process_recv_ack(*p_packet);
	} else {
		 // Received a data packet.
		 return tcl_process_recv_packet(p_packet, length);
	}
}

/**@brief Function for processing the time-out situation.
 *
 * @note This function should be called by Chip Timer Handler, which should has 
		 granularity of TCL_RESEND_MAX_TIMES ms.
 * @note If time-out when sending the sub-packet, resend the previous sub-packet.
 * @note If resend TCL_RESEND_MAX_TIMES times but failed to receive the ACk packet,
 *		 it means failed to send the packet from Application Layer(AL). Notice AL.
 *
 */
 void tcl_timer_time_out(void)
{
	if (TCL_TIMER_START == m_send_sub_packet_timer.status) { // If the sub-packet timer is started.
		if (m_send_sub_packet_timer.count++ >= TCL_RESEND_MAX_TIMES) { // Failed to send ACK packet.
			//tcl_timer_stop(&m_send_sub_packet_timer);
			tcl_send_failed(); // Failed for time-out;
			tcl_timer_stop(&m_send_sub_packet_timer);
		} else {
			tcl_send_sub_packet(); // Just send again the previous sub-packet.
		}
	}
	if (TCL_TIMER_START == m_recv_packet_timer.status) { // If the recveiving timer is started.
		if (m_recv_packet_timer.count++ >= TCL_RECV_WAIT_MAX_TIMES) { // Failed to receive packet.
			tcl_timer_stop(&m_recv_packet_timer);
			tcl_init_recv_env();
		}
	}
}


/**@brief Function for initializing the TCL Packet Structure.
 *
 */
static void tcl_packet_init()
{
	tcl_header_init(&m_send_ack_packet.header);
	tcl_header_init(&m_send_packet.header);
	memset(&m_send_packet.payload, 0, BLE_UART_PAYLOAD_MTU);
	memset(&m_send_packet.payload, 0, BLE_UART_PAYLOAD_MTU);
}

/**@brief Function for initializing timers.
 *
 */
static void tcl_timer_init()
{
	tcl_timer_stop(&m_send_sub_packet_timer);
	tcl_timer_stop(&m_recv_packet_timer);
}
/**@brief Function for initializing the Transport Control Layer.
 *
 * @param[in]   p_init	Pointer to the TCL initiate structure.
 *
 */
void tcl_init(tcl_init_t* p_init)
{
	ble_send_handler = p_init->ble_send_handler;
	al_recv_handler = p_init->al_recv_handler;
	al_send_failed_handler = p_init->al_send_failed_handler;
	al_send_success_handler = p_init->al_send_success_handler;
	tcl_packet_init();
	tcl_init_recv_env();
	tcl_timer_init();
}

