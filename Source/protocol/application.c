/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Application Layer Protocol which abbreviated AL when the L0 means the BLE Profile layer 
 *			to access the BLE Stack and the L1 means the Transport Control Layer which transmits the packet from AL to BLE.
 *
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */

#include <application.h>
#include <string.h>
// The following environment is set and saved for one transmission.
// {
#define PAY_LOAD_MAX_LENGTH  8

static al_send_status_t		m_al_send_status;
static al_packet_t				m_al_send_packet; 					// Data packet to sent.
static al_data_t					m_al_recv_data; 						// The recv data packet .
static uint8_t            m_al_recv_value[8];					// The recv value .
static uint8_t        		execute_status_vaule[2];   	//The value of execute status.
static uint8_t            purify_status = 0;							//The value of purify status.
static SensorData 				m_al_sensor;                //The data of sensor 	        //Copy from main.c Line:88     <Modified by @Mida 2015-6-21>
		// }

static ble_send_handler_t			al_ble_send_handler;
// The following handler is for processing the packet.
// {
static al_process_handler_t al_process_dfu_handler; // Function for processing DFU packet.
static al_process_handler_t al_process_settting_handler; // Function for processing Setting packet.
static al_process_handler_t al_process_control_handler; // Function for processing Control packet.
static al_process_handler_t al_process_rt_monitor_handler; // Function for sending the real-time monitor data.
static al_process_handler_t al_process_status_handler; // Function for processing Status packet.
static al_process_handler_t al_process_test_handler; // Function for processing Test packet.
static al_process_handler_t al_process_log_handler; // Function for processing Log packet.
// }

/**@brief Function for getting the sending status.
 *
 * @note Check the packet whether has been sent by the status.
 */
al_send_status_t al_send_status(void)
{
	return m_al_send_status;
}

/**@brief Function for handling situation of failing to send packet.
 *
 * @note You can do more jobs when AL is noticed when failed to send packet. For example, 
 *		 send the previous packet again and the maximum of times for resending.
 */
void al_send_failed()
{
	m_al_send_status = AL_SEND_STATUS_FAILED;
}

/**@brief Function for handling situation of successffuly sending packet.
 *
 */
void al_send_success(void)
{
	m_al_send_status = AL_SEND_STATUS_SUCCESS;
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

/**@brief Function for loading the checksum of sub-packet.
 *
 * @param[in]   length  		Real length of the packet.
 */
static void al_load_packet_checksum(uint16_t length)
{
	m_al_send_packet.packet_header.check_sum = checksum((uint16_t*)&m_al_send_packet, length);
}

/**@brief Function for checking the packet whether is a variable packet for this protocol.
 *
 * @note This function will check the magic number, the version and the checksum
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS					The packet is valid.
 * @return @ref Al_ERROR_MAGIC_NUMBER		The magic number is wrong.
 * @return @ref AL_ERROR_VERSION			The version is wrong.
 * @return @ref AL_ERROR_CHECK_SUM			The checksum is wrong.
 */
static uint32_t al_check_recv_packet(uint8_t* p_data, uint16_t length)
{
	al_packet_t* p_packet = (al_packet_t*)p_data;
	if (PACKET_HEADER_MAGIC_NUMBER != p_packet->packet_header.magic_number) {
		return AL_ERROR_MAGIC_NUMBER;
	}
	if (PACKET_HEADER_VERSION != p_packet->packet_header.version) {
		return AL_ERROR_VERSION;
	}
	if (0x0000 != checksum((uint16_t*)p_data, length)) {
		return AL_ERROR_CHECK_SUM;
	}
	return AL_SUCCESS;
}

/**@brief Function for loading the Values to al_data_t.
 *<Modify by Mida>
 * @note The AL will copy the data pointing by pointer of every key-value in the array.
 *
 * @param[in]   p_kv  		Pointer to the array of send key-value
 */
static void al_load_k_value(al_data_t* p_kv)
{
	uint8_t index = 0;
	m_al_send_packet.payload[(index)++] = p_kv->key_id;
	m_al_send_packet.payload[(index)++] = p_kv->key_length;
	for (uint32_t i = 0; i < p_kv->key_length; ++i) {
		m_al_send_packet.payload[(index)++] = p_kv->p_value[i];
	}
	for (uint32_t i = p_kv->key_length; i < PAY_LOAD_MAX_LENGTH; ++i) {
		m_al_send_packet.payload[(index)++] = 0;
	}
}

/**@brief Function for calculating total length of AL payload.
 *
 * @note The AL will copy the data pointing by pointer of every key-value in the array.
 *
 * @param[in]   p_kv  			Pointer to the array of key-value.
 * @param[in]   kv_number  		Array number.
 *
 * @return Total length of AL payload.
 */
static uint16_t al_calc_payload_length(al_data_t p_kv)
{
	uint16_t length = 0;
		length += AL_KEY_HEADER_LENGTH + p_kv.key_length;
	return length;
}

/*@brief Function for download the key-value from received packet.
 *<Modify by Mida>
 * @param[in]   p_data  		Pointer to the data received.
 */
static void al_download_payload(uint8_t* p_data)
{
	al_data_t* p_kv = &m_al_recv_data;
	uint8_t index = 0;
	p_kv->key_id = p_data[(index)++];
	p_kv->key_length = p_data[(index)++];
	for (uint8_t i = 0; i < p_kv->key_length; ++i) 
	{
		p_kv->p_value[i] = p_data[(index)++];
	}
}

/*@brief Function for download the key-value from received packet.
 *<Modify by Mida>
 * @param[in]   p_data  		Pointer to the data received.
 */
static void al_set_header(al_data_t p_kv)
{
	m_al_send_packet.packet_header.magic_number 		= PACKET_HEADER_MAGIC_NUMBER;
	m_al_send_packet.packet_header.version 					= PACKET_HEADER_VERSION	;
	m_al_send_packet.packet_header.payload_length 	= al_calc_payload_length(p_kv) + AL_HEADER_LENGTH;
	m_al_send_packet.packet_header.sequence_id   		= 0x00;
	m_al_send_packet.packet_header.flag							= 0x00;
	m_al_send_packet.packet_header.check_sum				= 0x0000;
}

/**@brief Function for sending packet to Phone through TCL.
 *
 * @note The AL will copy the data pointing by pointer of every key-value in the array.
 *
 * @param[in]   command_id  	Command ID.
 * @param[in]   p_kv  			Pointer to the array of key-value.
 *
 * @return @ref AL_SUCCESS				Successfully sent the packet.
 * @return @ref AL_WAIT					The previous packet is sending. Just wait and send again.
 * @return @ref AL_ERROR				Common failed.
 * @return @ref AL_ERROR_DATA_SIZE		Exceed the limit of data size..
 */
uint32_t al_send_packet(uint8_t command_id, al_data_t* p_kv)
{
	if (AL_SEND_STATUS_SENDING == al_send_status())        
		return AL_WAIT;
	m_al_send_status = AL_SEND_STATUS_SENDING;
	
	al_set_header(*p_kv);
	uint16_t payload_length = al_calc_payload_length(*p_kv);
	if (payload_length > AL_PAYLOAD_MTU)
		return AL_ERROR_DATA_SIZE;
		
	m_al_send_packet.al_header.command_id = command_id;
	m_al_send_packet.al_header.payload_length = payload_length;
	
	al_load_k_value(p_kv);

	uint16_t length = payload_length + PACKET_HEADER_LENGTH + AL_HEADER_LENGTH;     //The total length of packet
	al_load_packet_checksum(length);																								//Add the checksum of packet
	return al_ble_send_handler((uint8_t*)&m_al_send_packet, length);
}

/**@brief Function for sending status of executing to Phone through TCL.
 *
 * @note The 1st byte of comment indicates the command of executing packet and 
 *       the 2nd byte indicates the corresponding key. Following these two bytes,
 *       other comment can be set. For example, the reason for failing executing 
 *       can be set as a number in the 3rd byte of comment.
 *
 * @param[in]   is_success  	Succeed to execute or not.
 * @param[in]   p_comment  	  Pointer to the key-value's p_value of sending.
 * @param[in]   comment_length  	The length of key-value's p_value.
 *
 */
static uint32_t al_send_execute_status_packet(bool is_success, uint8_t* p_comment, uint8_t comment_length)
{
	al_data_t comment;
	comment.key_id = is_success ? AL_KEY_EXE_STAT_SUCCUSS : AL_KEY_EXE_STAT_FAILED;
	comment.p_value = p_comment;
	comment.key_length = comment_length;
	return al_send_packet(AL_COMMAND_EXE_STAT, &comment);
}

/**@brief Function for sending real-time of monitoring data to Phone through TCL.
 *
 * @note The 1st byte of comment indicates the command of executing packet and 
 *       the 2nd byte indicates the corresponding key. Following these two bytes,
 *       other comment can be set. For example, the reason for failing executing 
 *       can be set as a number in the 3rd byte of comment.
 *
 * @param[in]   key_id  			The key ID.
 * @param[in]   p_comment  	  Pointer to the key-value's p_value of sending.
 * @param[in]   comment_length  	The length of key-value's p_value.
 *
 */
static uint32_t al_send_rt_monitor_packet(uint8_t key_id, uint8_t* p_comment, uint8_t comment_length)
{
	al_data_t comment;
	comment.key_id = key_id;
	comment.key_length = comment_length;
	comment.p_value = p_comment;
	return al_send_packet(AL_COMMAND_RT_DATA, &comment);
}

/**@brief Function for sending status of hardware to Phone through TCL.
 *
 * @note The 1st byte of comment indicates the command of executing packet and 
 *       the 2nd byte indicates the corresponding key. Following these two bytes,
 *       other comment can be set. For example, the reason for failing executing 
 *       can be set as a number in the 3rd byte of comment.
 *
 * @param[in]   key_id  			The key ID.
 * @param[in]   p_comment  	  Pointer to the key-value's p_value of sending.
 * @param[in]   comment_length  	The length of key-value's p_value.
 *
 */
static uint32_t al_send_status_packet(uint8_t key_id, uint8_t* p_comment, uint8_t comment_length)
{
	al_data_t comment;
	comment.key_id = key_id;
	comment.key_length = comment_length;
	comment.p_value = p_comment;
	return al_send_packet(AL_COMMAND_STATUS, &comment);
}


/**@brief Function for receiving packet from Phone through TCL.
 *
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS				Successfully sent the packet.
 * @return @ref AL_ERROR				Common failed. 
 * @return @ref AL_ERROR_COMMAND		The command id is wrong.
 */
uint32_t al_recv_packet(uint8_t* p_data, uint16_t length)
{
	uint32_t err_code = al_check_recv_packet(p_data, length);
	if(AL_SUCCESS == err_code)
	{
		al_packet_t* p_packet =(al_packet_t*)p_data;
		al_header_t al_header = p_packet->al_header;
		uint32_t payload_length = al_header.payload_length;                       //The al payload_length
		switch(al_header.command_id) {
		case AL_COMMAND_DFU:
			if (0 != al_process_dfu_handler)
				return al_process_dfu_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_SETTING:
			execute_status_vaule[0] = AL_COMMAND_SETTING;
			if (0 != al_process_settting_handler)
				return al_process_settting_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_CONTROL:
			execute_status_vaule[0] = AL_COMMAND_CONTROL;
			if (0 != al_process_control_handler)
				return al_process_control_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_RT_DATA:
			if (0 != al_process_rt_monitor_handler)
				return al_process_rt_monitor_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_STATUS:
			if (0 != al_process_status_handler)
				return al_process_status_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_TEST:
			if (0 != al_process_test_handler)
				return al_process_test_handler(p_packet->payload, payload_length);
			break;
		case AL_COMMAND_LOG:
			if (0 != al_process_log_handler)
				return al_process_log_handler(p_packet->payload, payload_length);
			break;
		default:
			return AL_ERROR_COMMAND;
		}
		return AL_ERROR_NO_HANDLER;
	}else{
		return AL_ERROR;
	}
}


/**@brief Function for initializing the Application Layer.
 *
 * @param[in]   p_init	Pointer to the TCL initiate structure.
 *
 */
void al_init(al_init_t* p_init)
{
	al_ble_send_handler    = p_init->ble_send_hander; // Function for sending packet.
	al_process_dfu_handler = p_init->dfu_handler; // Function for processing DFU packet.
	al_process_settting_handler = p_init->settting_handler; // Function for processing Setting packet.
	al_process_control_handler = p_init->control_handler; // Function for processing Control packet.
	al_process_rt_monitor_handler = p_init->real_time_monitor_handler; // Function for sending the real-time monitor data.
	al_process_status_handler = p_init->status_handler; // Function for processing Status packet.
	al_process_test_handler = p_init->test_handler; // Function for processing Test packet.
	al_process_log_handler = p_init->log_handler; // Function for processing Log packet.
	m_al_recv_data.p_value  = m_al_recv_value;
	GetSensorData(&m_al_sensor);                  //<Add by @Mida 2015-6-22>Get the sensor data. 
}

/*@brief Function for passing the sensor data from main.c to application.c.
 *<Add by Mida 2015-6-21>
 * @param[in]   sensor  		The sample data of sensor.
 */
void pass_to_al_sensor_data(SensorData sensor)
{
	m_al_sensor = sensor;
}

/*@brief Function for processing Control packet.
 *<Modify by Mida>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_control_packet(uint8_t* p_data, uint16_t length)
{	
	al_download_payload(p_data);							//Download the payload to the m_al_recv_packet<Add by @Mida 2015-7-21>
  al_data_t* p_kv = &m_al_recv_data; 
	execute_status_vaule[1] = p_kv->key_id;
	switch(p_kv->key_id) {
		case AL_KEY_CONTROL_PURIFY_CLOSE:             // [Phone -> Purifier]: Stop to purify.
				purify_status = AL_KEY_CONTROL_PURIFY_CLOSE;
				nrf_gpio_pin_clear(PURIFIER_LED_PIN_NO);  
				CloseFan();
				al_send_execute_status_packet(AL_KEY_EXE_STAT_SUCCUSS,execute_status_vaule,2);		
				break;
		case AL_KEY_CONTROL_PURIFY_OPEN:             // [Phone -> Purifier]: Start to purify.
				purify_status = AL_KEY_CONTROL_PURIFY_OPEN;
				nrf_gpio_pin_set(PURIFIER_LED_PIN_NO); 
				OpenFan(60);
				al_send_execute_status_packet(AL_KEY_EXE_STAT_SUCCUSS,execute_status_vaule,2);		
				break;
		case AL_KEY_CONTROL_REVOLVING:					// [Phone -> Purifier]: Set the speed of revolving speed.
			break;
		case AL_KEY_CONTROL_POWEROFF:					  // [Phone -> Purifier]: Power off.
			break;
	default:
		return AL_ERROR_KEY;
	}
	return AL_SUCCESS;
}

/*@brief Function for processing Setting packet.
 *<Add by @Mida 2015-7-16>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_setting_packet(uint8_t* p_data, uint16_t length)
{
	al_download_payload(p_data);							//Download the payload to the m_al_recv_packet<Add by @Mida 2015-7-21>
  al_data_t* p_kv = &m_al_recv_data;  
	execute_status_vaule[1] = p_kv->key_id;
	CalenderTime *rtc;
	switch(p_kv->key_id) {
		case AL_KEY_SETTING_PERIOD:             					// [Phone -> Purifier]: Setting the sample period.
			break;
		case AL_KEY_SETTING_TIME:					  							// [Phone -> Purifier]: Setting time.
			rtc = (CalenderTime *)p_kv->p_value;
			SetCalenderTime(rtc);
//			al_send_execute_status_packet(AL_KEY_EXE_STAT_SUCCUSS,execute_status_vaule,2);		
			break;
		default:
			return AL_ERROR_KEY;
	}
	return AL_SUCCESS;
}

/*@brief Function for monitoring the real-time data.
 *<Add by @Mida 2015-7-22>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_rt_monitor_packet(uint8_t* p_data, uint16_t length)
{
	al_download_payload(p_data);							//Download the payload to the m_al_recv_packet<Add by @Mida 2015-7-21>
  al_data_t* p_kv = &m_al_recv_data;  
	switch(p_kv->key_id) {
		case  AL_KEY_RT_DATA_PM25 :	al_send_rt_monitor_packet(AL_KEY_RT_DATA_PM25,(uint8_t *)&m_al_sensor.pm2_5,4); 			break;
		case 	AL_KEY_RT_DATA_TVOC	:	al_send_rt_monitor_packet(AL_KEY_RT_DATA_TVOC,(uint8_t *)&m_al_sensor.tvoc,4);				break;	
		case 	AL_KEY_RT_DATA_TEMP	:	al_send_rt_monitor_packet(AL_KEY_RT_DATA_TEMP,(uint8_t *)&m_al_sensor.temperature,4);	break;				
		case 	AL_KEY_RT_DATA_HUMI	:	al_send_rt_monitor_packet(AL_KEY_RT_DATA_HUMI,(uint8_t *)&m_al_sensor.humidity,4);		break;					
		default:
			return AL_ERROR_KEY;
	}
	return AL_SUCCESS;
}

/*@brief Function for notify the hardware status to the Android.
 *<Add by @Mida 2015-7-24>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_status_packet(uint8_t* p_data, uint16_t length)
{
	al_download_payload(p_data);							//Download the payload to the m_al_recv_packet<Add by @Mida 2015-7-21>
  al_data_t* p_kv = &m_al_recv_data;  
	switch(p_kv->key_id){
		case AL_KEY_STATUS_BATT_CAP	:		al_send_status_packet(AL_KEY_STATUS_BATT_CAP,(uint8_t *)100,1);  break;
		case AL_KEY_STATUS_PURIFY		:		al_send_status_packet(AL_KEY_STATUS_PURIFY	,&purify_status,1);	break;
		
		default:
			return AL_ERROR_KEY;
	}
	return AL_SUCCESS;
	
}



