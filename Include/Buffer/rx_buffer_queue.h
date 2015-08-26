/** @file   <Coded by @Mida 2015-6-1>
 *
 * @defgroup RX buffer queue Handler
 * @{
 * @details The RX buffer queue handler receives the packet of "Write" from Android  
 *          add_rx_buffer_to_queue() when the Android send a packet to the board.
 *					rx_buffer_queue_evt_schedule()was used in the main() forever loop!!
 */

#ifndef RX_BUFFER_QUEUE_H__
#define RX_BUFFER_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "app_error.h"
#include "ble_config.h"

#define RX_BUFFER_QUEUE_LENGTH  (uint8_t)8                 //Must be the power of 2.   
#define ROTATION_MASK        (RX_BUFFER_QUEUE_LENGTH)-1			//As the mask of index to build A rotation queue.

/**@brief RX buffer element instance structure. 
 */
typedef struct 
{
    uint8_t  rx_buffer[BLE_UART_CHAR_BUFFER_SIZE];                   /**< RX buffer element memory array. */
		uint16_t length;																								 /**< RX buffer length of array. */
} rx_buffer_elem_t;

/**@brief RX buffer queue element instance structure. 
 */
typedef struct 
{
    rx_buffer_elem_t * p_buffer;                                    /**< Pointer to RX buffer element. */
    uint8_t           write_index;                                 /**< Write position index. */                                     
    uint8_t           read_index;                                  /**< Read position index. */                                                                                                                                                                                         
} rx_buffer_queue_t;


/**@brief The func for initializing RX buffer queue and add the Rx buffer queue Event to the Schedule. 
 */
void init_rx_buffer_queue_evt(rx_buffer_elem_t * p_buffer);

/**@brief The schedule func of checking RX buffer queue. 
 */
//void rx_buffer_queue_evt_schedule(void * p_event_data, uint16_t event_size);
void rx_buffer_queue_evt_schedule(void);

/**@brief The func for adding One RX buffer to the buffer queue. 
 */
void add_rx_buffer_to_queue(uint8_t* p_data,uint16_t length);

/**@brief The func for flushing the RX buffer queue. 
 */
void flush_rx_buffer_queue(void);


#endif // RX_BUFFER_QUEUE_H__

/** @} */
