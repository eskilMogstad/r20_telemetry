/*
 * can_to_udp.h
 *
 *  Created on: Sep 5, 2019
 *      Author: Eskil
 */

#ifndef SRC_CAN_TO_UDP_H_
#define SRC_CAN_TO_UDP_H_

#include "can_wrapper.h"

#define HEADER_LENGTH (sizeof(int64_t) + sizeof(uint32_t) + sizeof(uint8_t))
#define MAX_MSG_LEN   (HEADER_LENGTH + MAX_DATA_LEN)

typedef union {
    uint8_t bytes[MAX_MSG_LEN];
    struct {
        uint64_t timestamp;
        uint32_t can_id;
        uint8_t data_length;
        uint8_t data[MAX_DATA_LEN];
    }fields;
} can_over_udp_msg_t;

/*
 * Translates can frame to udp msg
 *
 * @param[in] frame can frame to be translated
 * @param[out] udp_msg udp msg to be placed in buffer
 *
 * @return XST_SUCCESS
 */
int can_frame_to_udp(can_frame_t const * const frame, can_over_udp_msg_t * udp_msg, uint32_t * udp_length);

#endif /* SRC_CAN_TO_UDP_H_ */
