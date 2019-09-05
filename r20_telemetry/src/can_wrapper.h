/*
 * can_wrapper.h
 *
 *  Created on: Sep 4, 2019
 *      Author: Eskil
 */

#ifndef SRC_CAN_WRAPPER_H_
#define SRC_CAN_WRAPPER_H_

#include <stdint.h>

/*
 * CAN bit timing constants, given in actual values
 *
 * Current config
 *  
 * | Clock Rate | Sample point |   Bitrate   | SJW | Prescaler | Time quanta | Seg1 | Seg2 |
 * |:----------:|:------------:|:-----------:|-----|-----------|-------------|------|------|
 * |   100 MHz  |      75%     | 1000 KBit/s | 1   | 5         | 20          | 14   | 5    |
 */
#define BRPR_PRESCALER  4

#define BTR_SJW         1
#define BTR_TSEG_2      5
#define BTR_TSEG_1      14

/*
 * Datatype for storing can message
 */
#define MAX_DATA_LEN    64U

typedef struct {
    uint64_t timestamp;
    uint32_t can_id;
    uint8_t data_length;
    uint8_t data[MAX_DATA_LEN];
} can_frame_t;

/*
 * Initializes a CAN peripheral for normal operation. Uses bit timings given above.
 * 
 * @param device_id Unique id of can device, valid values:
 *                  XPAR_XCANPS_0_DEVICE_ID
 *                  XPAR_XCANPS_1_DEVICE_ID
 * 
 * @return XST_SUCCESS if setups was successfull, XST_FAILURE otherwise
 */
int can_wrapper_init(uint16_t device_id);


/*
 * Retrieves a can message from the FIFO, if there is one.
 *
 * @param[out] frame Storage for the retrieved can frame
 * 
 * @return XST_SUCCESS if a message was stored in message successfully, XST_FAILURE otherwise
 */
int can_wrapper_receive_frame(can_frame_t *frame);

/*
 * Places a can message to the TX FIFO
 *
 * @param[in] frame The can frame to be sent
 *
 * @return XST_SUCCESS if the message was placed successfully in the TX FIFO, XST_FAILURE otherwise
 */
int can_wrapper_send_frame(can_frame_t const * const frame);


#endif /* SRC_CAN_WRAPPER_H_ */
