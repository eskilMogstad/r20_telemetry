#include "xparameters.h"
#include "xil_printf.h"
#include "canfd.h"
#include "intc.h"
#include "xil_types.h"

#include "sleep.h"

static XCanFd canfd0_i;
static XCanFd canfd1_i;

static XScuGic intc_i;

int main()
{
    // Interrupt setup
    intc_init(&intc_i);

    // Init canfd
    canfd_init(&canfd0_i, &intc_i, CANFD0_ID);
    canfd_init(&canfd1_i, &intc_i, CANFD1_ID);

    // Connecting ISRs
    //intc_connect_isr(&itc_i, ISR, FUNC);

    // Enable interrupts
    intc_enable();

    u32 data = 0xA5A5;

	while(1)
    {
	    canfd_tx_std(&canfd0_i, (u8 * )&data, 0x50, 4);
	    canfd_tx_std(&canfd1_i, (u8 * )&data, 0x51, 4);
	    usleep(1000000);
    }

    return XST_SUCCESS;
}

