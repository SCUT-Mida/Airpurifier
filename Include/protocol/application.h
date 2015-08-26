/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Application Layer Protocol which abbreviated AL when the L0 means the BLE Profile layer 
 *			to access the BLE Stack and the L1 means the Application Layer which transmits the packet from AL to BLE.
 *
 * @note	The payload of AL is a array of key-value. The data type of key-value is defined below.
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */

#ifndef APPLICATION_H__
#define APPLICATION_H__
#include <stdint.h>
#include <ble_config.h>
#include <car_air_purifier.h>

#define PACKET_HEADER_VERSION							(uint8_t)10 				// The value of 10 means 1.0
#define PACKET_HEADER_MAGIC_NUMBER				(uint8_t)0xAA
#define PACKET_HEADER_LENGTH							(uint32_t)8 				// Length of Packet header

// Definitions of the AL
#define AL_MTU										(uint16_t)12 // Note that the MTU of AL is maximum size of TCL payload.
#define AL_HEADER_LENGTH					(uint32_t)2 // Length of AL header
#define AL_PAYLOAD_MTU						(uint32_t)(AL_MTU-AL_HEADER_LENGTH) // Maximum size of AL payload
#define AL_KEY_HEADER_LENGTH				(uint32_t)2 // Note that the key header is total size of 'key_id' and 'key_length'.
#define AL_RESEND_MAX_COUNT					(uint8_t)3

/* @note The status codes are kept same with the TCL, so the status can transfer without translating which 
		may be a good way to reduce implement codes.
*/
#define AL_SUCCESS							(uint32_t)0 // Success.
#define AL_WAIT								(uint32_t)1 // The previous packet is sending. Just wait and send again.
#define AL_AGAIN							(uint32_t)2 // The BLE is busy or has no enough available buffer, should send again.
#define AL_ERROR							(uint32_t)3 // Common failed.
#define AL_ERROR_MEM						(uint32_t)4 // Memory error.
#define AL_ERROR_DATA_SIZE					(uint32_t)5 // Exceed the limit of data size.
#define AL_ERROR_MAGIC_NUMBER				(uint32_t)6 // The magic number is wrong.
#define AL_ERROR_VERSION					(uint32_t)7 // The version is wrong.
#define AL_ERROR_CHECK_SUM					(uint32_t)8 // The checksum is wrong.
#define AL_ERROR_COMMAND					(uint32_t)9 // The command id is wrong.
#define AL_ERROR_KEY						(uint32_t)10 // The key-id is wrong.
#define AL_ERROR_NO_HANDLER					(uint32_t)11 // The corresponding handler is null.

// Command ID of AL.
// {
#define AL_COMMAND_EXE_STAT					(uint8_t)0 // Status of executing of command.
#define AL_COMMAND_DFU						(uint8_t)1 // Firmware Update.
#define AL_COMMAND_SETTING					(uint8_t)2 // Setting.
#define AL_COMMAND_CONTROL					(uint8_t)3 // Control the purifier.
#define AL_COMMAND_RT_DATA					(uint8_t)4 // Get real-time data.
#define AL_COMMAND_OL_DATA					(uint8_t)5 // Get off-line data.
#define AL_COMMAND_STATUS					(uint8_t)6 // Get status of purifier.
#define AL_COMMAND_TEST						(uint8_t)7 // Test Mode.
#define AL_COMMAND_LOG						(uint8_t)8 // Log.
// }

// Key ID for Command.
// {
// Status of executing of command.
#define AL_KEY_EXE_STAT_FAILED			(uint8_t)0 // [Phone <- Purifier]: Failed to execute command.
#define AL_KEY_EXE_STAT_SUCCUSS			(uint8_t)1 // [Phone <- Purifier]: Successfully to execute command.

// Firmware Update.
#define AL_KEY_DFU_REQUEST				(uint8_t)0 // [Phone -> Purifier]: Request to execute OTA DFU
#define AL_KEY_DFU_STATUS				(uint8_t)1 // [Phone <-> Purifier]: Status of OTA DFU

// Setting.
#define AL_KEY_SETTING_PERIOD			(uint8_t)0 // [Phone -> Purifier]: Set period of collecting data.
#define AL_KEY_SETTING_TIME				(uint8_t)1 // [Phone -> Purifier]: Set time.

// Control the purifier.
#define AL_KEY_CONTROL_PURIFY_CLOSE	(uint8_t)0 // [Phone -> Purifier]: Stop to purify.
#define AL_KEY_CONTROL_PURIFY_OPEN	(uint8_t)1 // [Phone -> Purifier]: Start to purify.
#define AL_KEY_CONTROL_REVOLVING		(uint8_t)2 // [Phone -> Purifier]: Set the speed of revolving speed.
#define AL_KEY_CONTROL_POWEROFF			(uint8_t)3 // [Phone -> Purifier]: Power off.

// Get real-time data.
#define AL_KEY_RT_DATA_PM25				(uint8_t)0 // [Phone <- Purifier]: Real-time value of PM2.5.
#define AL_KEY_RT_DATA_TVOC				(uint8_t)1 // [Phone <- Purifier]: Real-time value of TVOC.
#define AL_KEY_RT_DATA_TEMP				(uint8_t)2 // [Phone <- Purifier]: Real-time value of Temperature.
#define AL_KEY_RT_DATA_HUMI				(uint8_t)3 // [Phone <- Purifier]: Real-time value of Humidity.

 // Get off-line data.
#define AL_KEY_OL_DATA_TIME				(uint8_t)0 // [Phone <- Purifier]: Time stamp of recording.
#define AL_KEY_OL_DATA_PM25				(uint8_t)0 // [Phone <- Purifier]: Off-line value of PM2.5.
#define AL_KEY_OL_DATA_TVOC				(uint8_t)1 // [Phone <- Purifier]: Off-line value of TVOC.
#define AL_KEY_OL_DATA_TEMP				(uint8_t)2 // [Phone <- Purifier]: Off-line value of Temperature.
#define AL_KEY_OL_DATA_HUMI				(uint8_t)3 // [Phone <- Purifier]: Off-line value of Humidity.

// Get status of purifier.
#define AL_KEY_STATUS_BATT_CAP			(uint8_t)0 // [Phone <-> Purifier]: Battery capacity.
#define AL_KEY_STATUS_PURIFY				(uint8_t)1 // [Phone <-> Purifier]: Purify status.

// Test Mode.
#define AL_KEY_TEST_ECHO				(uint8_t)0 // [Phone <-> Purifier]: Echo service.
#define AL_KEY_TEST_FLASH				(uint8_t)0 // [Phone <-> Purifier]: Test the Flash.
#define AL_KEY_TEST_LCD					(uint8_t)0 // [Phone <-> Purifier]: Test the LCD display.
#define AL_KEY_TEST_LED					(uint8_t)0 // [Phone <-> Purifier]: Test the LED.
#define AL_KEY_TEST_MOTOR				(uint8_t)0 // [Phone <-> Purifier]: Test the motor.

// Log.
#define AL_KEY_LOG_SWITCH				(uint8_t)0 // [Phone -> Purifier]: Switch of logging(on-off)
#define AL_KEY_LOG_READING				(uint8_t)1 // [Phone <-> Purifier]: Read the log.

// }

/**@brief Function for sending packet to the BLE Profile Layer.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return err_code return from BLE Profile Layer for sending data.
 */
typedef uint32_t (*ble_send_handler_t) (uint8_t* p_data, uint16_t length);

/**@brief Function for processing Test packet.
 *
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
typedef uint32_t (*al_process_handler_t)(uint8_t* p_data, uint16_t length);

/* This is an example function of above handler.
 *@brief Function for processing DFU packet.
 *
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.

uint32_t al_process_dfu_packet(uint8_t* p_data, uint16_t length)
{
	al_data_t* p_kv = (al_data_t*)p_data;
	switch(p_kv->key_id) {
	default:
		return AL_ERROR_KEY;
	}
	return AL_SUCCESS;
}
 */
 
typedef struct al_init_s
{
	ble_send_handler_t   ble_send_hander;    //Function for sending packet to Android
	al_process_handler_t dfu_handler; // Function for processing DFU packet.
	al_process_handler_t settting_handler; // Function for processing Setting packet.
	al_process_handler_t control_handler; // Function for processing Control packet.
	al_process_handler_t real_time_monitor_handler; // Function for sending the real-time monitor data.
	al_process_handler_t status_handler; // Function for processing Status packet.
	al_process_handler_t test_handler; // Function for processing Test packet.
	al_process_handler_t log_handler; // Function for processing Log packet.	
} al_init_t;

typedef struct packet_header_s
{
	uint8_t		magic_number;
	uint8_t		version; // The version of this "BLE Communication Protocol of Car Air Purifier". For example 15 means 1.5
	uint16_t	payload_length; // The length(number of bytes) of the TCL Payload.
	uint8_t		sequence_id; // The sequence ID of this packet.
	uint8_t		flag;
	uint16_t	check_sum;
} packet_header_t;

typedef struct al_header_s
{
	uint8_t		command_id; // Command ID.
	uint8_t		payload_length; // Length of Payload;
} al_header_t;

typedef struct al_packet_s
{
	packet_header_t packet_header;
	al_header_t			al_header;
	uint8_t					payload[AL_PAYLOAD_MTU];
} al_packet_t;

// The data type of AL.
typedef struct al_data_s
{
	uint8_t		key_id; // Common ID.
	uint8_t		key_length; // Length of Payload.
	uint8_t*	p_value; // Point to value.
} al_data_t;

// Note that if time out, the sending status should be set as AL_SEND_STATUS_SENDING.
typedef enum
{
    AL_SEND_STATUS_SUCCESS,	// Successfully sent or means idle to send.
    AL_SEND_STATUS_SENDING,	// Sending.
		AL_SEND_STATUS_FAILED	// Failed to send.
} al_send_status_t;

// Note that if time out, the executing status should be set as AL_EXECUTE_STATUS_FAILED.
typedef enum
{
		AL_EXECUTE_STATUS_FAILED,			// Failed to execute.
    AL_EXECUTE_STATUS_SUCCESS,		// Successfully executed.
    AL_EXECUTE_STATUS_EXECUTING		// Executing.
} al_execute_status_t;

/**@brief Function for initializing the Application Layer.
 *
 * @param[in]   p_init	Pointer to the TCL initiate structure.
 *
 */
void al_init(al_init_t* p_init);

/**@brief Function for getting the sending status.
 *
 * @note Check the packet whether has been sent by the status.
 */
al_send_status_t al_send_status(void);

/**@brief Function for getting the sending status.
 *
 * @note Check the packet whether has been sent by the status.
 */
al_execute_status_t al_execute_status(void);

/**@brief Function for sending packet through the Application Layer.
 *
 * @note The AL will copy the data pointing by pointer of every key-value in the array.
 *
 * @param[in]   command_id  	Command ID.
 * @param[in]   p_kv  			Pointer to the array of key-value.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 */
uint32_t al_send_packet(uint8_t command_id, al_data_t* p_kv);

/**@brief Function for sending packet through the Application Layer.
 *
 * @note The Application layer should keep the data buffer until sending finishes.
 *
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 */
uint32_t al_recv_packet(uint8_t* p_data, uint16_t length);

/**@brief Function for handling situation of failing to send packet.
 *
 */
void al_send_failed(void);

/**@brief Function for handling situation of successffuly sending packet.
 *
 */
void al_send_success(void);

/*@brief Function for passing the sensor data from main.c to application.c.
 *<Add by Mida 2015-6-21>
 * @param[in]   sensor  		The sample data of sensor.
 */
void pass_to_al_sensor_data(SensorData sensor);

/*@brief Function for processing Control packet.
 *<Add by Mida>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_control_packet(uint8_t* p_data, uint16_t length);

/*@brief Function for processing Setting packet.
 *<Add by Mida>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_setting_packet(uint8_t* p_data, uint16_t length);

/*@brief Function for monitoring the real-time data.
 *<Add by @Mida 2015-7-22>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_rt_monitor_packet(uint8_t* p_data, uint16_t length);

/*@brief Function for notify the hardware status to the Android.
 *<Add by @Mida 2015-7-24>
 * @param[in]   p_data  		Pointer to the data received.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref AL_SUCCESS		Successfully sent the packet.
 * @return @ref AL_ERROR		Common failed.
 * @return @ref AL_ERROR_KEY	The Key id is wrong.
 */
uint32_t al_process_status_packet(uint8_t* p_data, uint16_t length);



#endif

