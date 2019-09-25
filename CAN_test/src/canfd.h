//   _____ _____ _____
//  |   | |__   |   | | cpu0/src/zcan/canfd.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 16-04-19
#ifndef SRC_ZCAN_CANFD_H_
#define SRC_ZCAN_CANFD_H_

#include "XCanFd.h"
#include "XScuGic.h"

#include "canfd_cfg.h"

void canfd_init(XCanFd *canfd_p, XScuGic *intc_p, u32 id);
//s32 canfd_rx(XCanFd *canfd_p, CanardCANFrame *p_frame);
//s32 canfd_tx(XCanFd *canfd_p, const CanardCANFrame *const p_frame);
s32 canfd_tx_std(XCanFd *canfd_p, u8 *data, u32 id, u8 len);
u8  canfd_get_rx_buf_count(XCanFd *canfd_p);

#endif /* SRC_CAN_CANFD_H_ */
