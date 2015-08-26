/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a Protocol which is consist of BLE Stack, BLE UART Profile Layer(L0), Transport Control 
			Layer(L1) and Application Layer(L2).
 *
 * @note	This file is related with the context of chip and BLE stack implement while the Transport Control Layer and 
			Application Layer is independent of the context. So if you change the context, please take good care of this
			file which may be changed carefully and specifically.
 * @note	The detailed description of this layer is in the document "BLE Communication Protocol of Car Air Purifier"
 *
 */
 
#ifndef PROTOCOL_H__
#define PROTOCOL_H__

// Header of BLE Service which is based on the specific BLE Stack.
#include <ble_uart.h>
// Header of Communication Protocol
#include <application.h>


typedef struct protocol_init_s
{
	ble_uart_t*		p_uart; 		// p_uart holds the pointer to the BLE UART Service.
	al_init_t*		p_al_init; 	// p_al_init holds the handlers for processing AL packet.
} protocol_init_t;


/**@brief Function for initializing Protocol module.
 */
void purifier_protocol_init(protocol_init_t* protocol_init);

#endif


