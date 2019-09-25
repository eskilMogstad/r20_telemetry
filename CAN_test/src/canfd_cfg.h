//   _____ _____ _____
//  |   | |__   |   | | cpu0/src/zcan/canfd_cfg.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 16-04-19
#ifndef SRC_ZCAN_CANFD_CFG_H_
#define SRC_ZCAN_CANFD_CFG_H_

#include "xparameters.h"
#include "xcanfd_hw.h"

/* Define baud rates and enable baud rate switching (BRS) */
#define NOM_1MBIT
#define FD_4MBIT
#define ENABLE_BRS

/* Nominal Bit Timing */
#ifdef NOM_1MBIT
#define BRP_NOM 10U
#define TS1_NOM 5U
#define TS2_NOM 2U
#define SJW_NOM 1U
#endif

/* Flexible Bit Timing */
#ifdef FD_1MBIT
#define BRP_FD  10U
#define TS1_FD  5U
#define TS2_FD  2U
#define SJW_FD  1U
#endif

#ifdef FD_2MBIT
#define BRP_FD  4U
#define TS1_FD  7U
#define TS2_FD  2U
#define SJW_FD  1U
#endif

#ifdef FD_4MBIT
#define BRP_FD  2U
#define TS1_FD  7U
#define TS2_FD  2U
#define SJW_FD  1U
#endif

#ifdef FD_8MBIT
#define BRP_FD  1U
#define TS1_FD  7U
#define TS2_FD  2U
#define SJW_FD  1U
#endif

/* TXR Delay Compensation (# of CAN_CLK cycles) */
#define TXRX_DELAY 12U

/* PL->PS Interrupt Configuration */
#define IRQ_PRI  0x00 /* Priority - 0-248 in multiples of 8, where 0 is highest. */
#define IRQ_TRIG 0b11 /* Trigger Type - 0b11 = Rising Edge */

/* Device Definitions */
#define CANFD0_ID     XPAR_CANFD_0_DEVICE_ID
#define CANFD0_IRQ_ID XPAR_FABRIC_CANFD_0_VEC_ID
#define CANFD1_ID     XPAR_CANFD_1_DEVICE_ID
#define CANFD1_IRQ_ID XPAR_FABRIC_CANFD_1_VEC_ID

/* IRQ Masks */
#define CANFD0_IRQ_MASK (XCANFD_IXR_RXOK_MASK | XCANFD_IXR_ERROR_MASK)
#define CANFD1_IRQ_MASK (XCANFD_IXR_RXOK_MASK | XCANFD_IXR_ERROR_MASK)

#endif /* SRC_CAN_CANFD_CFG_H_ */
