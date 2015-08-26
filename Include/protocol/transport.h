/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Transport Layer Protocol which abbreviated TCL when the L0 means the BLE Profile layer 
 *			to access the BLE Stack and the L2 means the Application Layer which is the most upper layer.
 *
 * @note	Note that the sequence id is begin with 0.
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */
#ifndef TRANSPORT_H__
#define TRANSPORT_H__

#include <stdint.h>
#include <ble_config.h>
#include <ble_uart.h>

#define TCL_PROTOCOL_VERSION				(uint8_t)10 // The value of 10 means 1.0

// Definitions of the TCL
#define BLE_UART_MTU						(uint32_t)BLE_UART_CHAR_BUFFER_SIZE
#define BLE_UART_PAYLOAD_MTU				(uint32_t)(BLE_UART_CHAR_BUFFER_SIZE-TCL_HEADER_LENGTH)
#define TCL_MTU								(uint32_t)512
#define TCL_PAYLOAD_MTU						(uint32_t)504

#define TCL_HEADER_LENGTH					(uint32_t)8
#define TCL_HEADER_MAGIC_NUMBER				(uint8_t)0xAA

#define TCL_HEADER_FLAG_ACK_BIT_POS			(uint8_t)0 //  the bit position of 'acknowledgement' on the Flag of TCL Header.
#define TCL_HEADER_FLAG_ERR_BIT_POS			(uint8_t)1 //  the bit position of 'error' on the Flag of TCL Header.
#define TCL_HEADER_FLAG_RESERVED_BIT_POS	(uint8_t)2 //  the bit position of 'error' on the Flag of TCL Header.

#define TCL_SEND_TIMER_INTERVAL				(uint32_t)500 // The unit is ms.
#define TCL_RESEND_MAX_TIMES				(uint8_t)3
#define TCL_RECV_WAIT_MAX_TIMES				(uint8_t)3

#define TCL_SUCCESS							(uint32_t)0 // Success.
#define TCL_WAIT							(uint32_t)1 // Notice the upper layer to wait and send again.
#define TCL_AGAIN							(uint32_t)2 // The BLE is busy or has no enough available buffer, should send again.
#define TCL_ERROR							(uint32_t)3 // Common failed.
#define TCL_ERROR_MEM						(uint32_t)4 // Memory error.
#define TCL_ERROR_DATA_SIZE					(uint32_t)5 // Exceed the limit of data size.
#define TCL_ERROR_MAGIC_NUMBER				(uint32_t)6 // The magic number is wrong.
#define TCL_ERROR_VERSION					(uint32_t)7 // The version is wrong.
#define TCL_ERROR_CHECK_SUM					(uint32_t)8 // The checksum is wrong.

#ifndef BLE_SUCCESS
#define BLE_SUCCESS					NRF_SUCCESS
#endif

// Macros functions
#define TCL_SET_ACK_FLAG(flag)				(SET_BIT((flag),TCL_HEADER_FLAG_ACK_BIT_POS))
#define TCL_SET_ERR_FLAG(flag)				(SET_BIT((flag),TCL_HEADER_FLAG_ERR_BIT_POS))
#define TCL_UNSET_FLAG(flag)				((flag)=0)
#define TCL_CHECK_ACK_FLAG(flag)			(CHECK_BIT((flag),TCL_HEADER_FLAG_ACK_BIT_POS))
#define TCL_CHECK_ERR_FLAG(flag)			(CHECK_BIT((flag),TCL_HEADER_FLAG_ERR_BIT_POS))

#define TCL_CALC_SUB_PACKET_NUMBER(length)	(((uint32_t)(length)+(BLE_UART_PAYLOAD_MTU-1))/BLE_UART_PAYLOAD_MTU)
#define TCL_GET_ADDR_BY_SEQ_ID(base,id)		(uint8_t*)((uint8_t*)(base)+((uint32_t)(id)*BLE_UART_PAYLOAD_MTU))

/**@brief Function for handling the received packet from AL.
 *
 * @param[in]   p_data  	Pointer to the data buffer.
 * @param[in]   length  	Length of the data received.
 *
 * @return err_code return from Application Layer for processing the received data.
 */

typedef uint32_t (*al_recv_handler_t) (uint8_t* p_data, uint16_t length);

/**@brief Function for sending packet to the BLE Profile Layer.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return err_code return from BLE Profile Layer for sending data.
 */
typedef uint32_t (*ble_send_handler_t) (uint8_t* p_data, uint16_t length);

/**@brief Function for noticing AL when failed to sending the packet from it.
 *
 */
typedef void (*al_send_failed_handler_t)(void);

/**@brief Function for noticing AL when successful to sending the packet from it.
 *
 */
typedef void (*al_send_success_handler_t)(void);

typedef struct tcl_init_s
{
	ble_send_handler_t			ble_send_handler;
	al_recv_handler_t			al_recv_handler;
	al_send_failed_handler_t	al_send_failed_handler;
	al_send_success_handler_t	al_send_success_handler;
} tcl_init_t;

typedef struct tcl_header_s
{
	uint8_t		magic_number;
	uint8_t		version; // The version of this "BLE Communication Protocol of Car Air Purifier". For example 15 means 1.5
	uint16_t	payload_length; // The length(number of bytes) of the TCL Payload.
	uint8_t		sequence_id; // The sequence ID of this packet.
	uint8_t		flag;
	uint16_t	check_sum;
} tcl_header_t;

typedef struct tcl_packet_s
{
	tcl_header_t	header;
	uint8_t			payload[BLE_UART_PAYLOAD_MTU];
} tcl_packet_t;


typedef struct tcl_ack_packet_s
{
	tcl_header_t	header;
	uint8_t			payload[BLE_UART_PAYLOAD_MTU];
} tcl_ack_packet_t;

typedef enum
{
    TCL_SEND_STATUS_SUCCESS,	// Successfully sent or means idle to send.
    TCL_SEND_STATUS_SENDING,	// Sending.
		TCL_SEND_STATUS_FAILED		// Failed to send.
} tcl_send_status_t;

typedef enum
{
    TCL_TIMER_STOP,		// TCL timer stops.
    TCL_TIMER_START		// TCL timer starts.
} tcl_timer_status_t;

typedef struct tcl_timer_s
{
	tcl_timer_status_t	status; // Status of this timer.
	uint8_t count;
} tcl_timer_t;

/**@brief Function for initializing the Transport Control Layer.
 *
 * @param[in]   p_init  Pointer to the TCL initiate Module.
 */
void tcl_init(tcl_init_t* p_init);

/**@brief Function for getting the sending status.
 *
 * @note The Application Layer can check the packet whether has been sent by the status.
 */
tcl_send_status_t tcl_send_status(void);

/**@brief Function for sending packet from the Application Layer to BLE Profile Layer.
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
uint32_t tcl_send_packet(uint8_t* p_data, uint16_t length);

/**@brief Function for received packet from the BLE Profile Layer.
 *
 * @param[in]   p_data  		Pointer to the data buffer.
 * @param[in]   length  		Length of the data.
 *
 * @return @ref TCL_SUCCESS		Successfully sent the packet.
 * @return @ref TCL_ERROR		Common failed.
 */
uint32_t tcl_recv_packet(uint8_t* p_data, uint16_t length);

/**@brief Function for processing the time-out situation.
 *
 * @note This function should be called by Chip Timer Handler, which should has 
		 granularity of TCL_RESEND_MAX_TIMES ms.
 * @note If time-out when sending the sub-packet, resend the previous sub-packet.
 * @note If resend TCL_RESEND_MAX_TIMES times but failed to receive the ACk packet,
 *		 it means failed to send the packet from Application Layer(AL). Notice AL.
 *
 */
 void tcl_timer_time_out(void);
 
#endif


