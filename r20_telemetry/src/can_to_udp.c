/*
 * can_to_udp.c
 *
 *  Created on: Sep 5, 2019
 *      Author: Eskil
 */
#include "can_to_udp.h"

#include <string.h>
#include "xstatus.h"

static uint32_t swap_endian_32(uint32_t data);
static uint64_t swap_endian_64(uint64_t data);

int can_frame_to_udp(can_frame_t const * const frame, can_over_udp_msg_t * udp_msg, uint32_t * udp_length)
{
    udp_msg->fields.timestamp = swap_endian_64(frame->timestamp);
    udp_msg->fields.can_id = swap_endian_32(frame->can_id);
    udp_msg->fields.data_length = frame->data_length;

    memcpy(udp_msg->fields.data, frame->data, frame->data_length);

    *udp_length = HEADER_LENGTH + frame->data_length;

    return XST_SUCCESS;
}

static uint64_t swap_endian_64(uint64_t data)
{
    union {
        uint64_t u64;
        uint8_t u8[8];
    } x;

    x.u64 = data;

    for(int i = 0; i < 4; i++) {
        uint8_t temp = x.u8[i];
        x.u8[i] = x.u8[7-i];
        x.u8[7-i] = temp;
    }
    return x.u64;
}

static uint32_t swap_endian_32(uint32_t data)
{
    return (uint32_t)(swap_endian_64(data) >> 32);
}
