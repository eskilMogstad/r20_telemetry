//   _____ _____ _____
//  |   | |__   |   | | cpu0/src/intc.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 14-01-19
#ifndef SRC_INTC_INTC_H_
#define SRC_INTC_INTC_H_

#include "xscugic.h"

#define IRQ_TIMER XPAR_SCUTIMER_INTR
#define IRQ_1HZ_FIT     XPAR_FABRIC_FIT_1HZ_INTERRUPT_INTR
#define IRQ_100HZ_FIT   XPAR_FABRIC_FIT_100HZ_INTERRUPT_INTR
#define SWIRQ_TV_INIT        1U
#define SWIRQ_TV_CONFIGURE   2U
#define SWIRQ_TV_STEP        3U
#define SWIRQ_SYNC_INPUT     4U
#define SWIRQ_SYNC_OUTPUT    5U
#define SWIRQ_SYNC_STATUS    6U
#define SWIRQ_IDLE           7U
#define SWIRQ_TV_TOGGLE_LC   8U
#define SWIRQ_TV_TOGGLE_SKID 9U
#define SWIRQ_TEST          15U

#define CPU0_ID 0x00000001
#define CPU1_ID 0x00000002

void intc_init(XScuGic *intc_p);
void intc_enable();
void intc_connect_isr(XScuGic *intc_p, u32 irq_id, void *isr);
void intc_trig_swi(u32 id, u32 cpu);

#endif /* SRC_INTC_INTC_H_ */
