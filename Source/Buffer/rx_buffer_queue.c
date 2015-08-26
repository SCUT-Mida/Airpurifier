/* Copyright (c) 2015 [@Mida]. All Rights Reserved.
 *
 */

#include <rx_buffer_queue.h>
#include <application.h>

static rx_buffer_queue_t   m_rx_buffer_queue;        //The record Struct of RX buffer queue.

static bool IsRxBufferQueueEmypt(rx_buffer_queue_t  rx_buffer_queue) 
{
	if(rx_buffer_queue.read_index == rx_buffer_queue.write_index)
		return true;
	else
		return false;
}

/**@brief <Created by @Mida 2015-6-2>
* The func for initializing RX buffer queue and add the Rx buffer queue Event to the Schedule. 
* @param[in]   rx_buffer_elem_t*   p_buffer  The pointer to the RX buffer elements.
*/
void init_rx_buffer_queue_evt(rx_buffer_elem_t * p_buffer)
{
	m_rx_buffer_queue.p_buffer = p_buffer;
	m_rx_buffer_queue.read_index = 0;
	m_rx_buffer_queue.write_index = 0;	
}

/**@brief <Created by @Mida 2015-6-2> 
* The schedule func of checking RX buffer queue. 
*/
void rx_buffer_queue_evt_schedule(void)
{
	if(!IsRxBufferQueueEmypt(m_rx_buffer_queue))
	{		
		m_rx_buffer_queue.read_index &= ROTATION_MASK ;         //The '&' operation to make sure that the queue is rotation.
		uint8_t pos = m_rx_buffer_queue.read_index; 
		uint32_t err_code = al_recv_packet(m_rx_buffer_queue.p_buffer[pos].rx_buffer, m_rx_buffer_queue.p_buffer[pos].length);
		m_rx_buffer_queue.read_index ++;                                      //The data was taken,read index pointer to next one.	
	}		
}

/**@brief <Created by @Mida 2015-6-2>
* The func for adding One RX buffer to the buffer queue. 
* @param[in]   uint8_t*   p_data  	The pointer to the rx buffer data.
* @param[in]   uint16_t   length    The length of data.
*/
void add_rx_buffer_to_queue(uint8_t* p_data,uint16_t length)
{
	uint16_t i;
	m_rx_buffer_queue.write_index &= ROTATION_MASK ;       //The '&' operation to make sure that the queue is rotation.
	uint8_t pos =  m_rx_buffer_queue.write_index;
	for(i = 0 ;i < length ; i++)
		m_rx_buffer_queue.p_buffer[pos].rx_buffer[i] = p_data[i];
	m_rx_buffer_queue.p_buffer[pos].length = length;
	m_rx_buffer_queue.write_index ++ ;																	//The data was stored,write index pointer to next one.		
}

/**@brief <Created by @Mida 2015-6-2>
* The func for flushing the RX buffer queue. 
*/
void flush_rx_buffer_queue(void)
{
	m_rx_buffer_queue.read_index = m_rx_buffer_queue.write_index;
}

