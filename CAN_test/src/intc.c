//   _____ _____ _____
//  |   | |__   |   | | cpu0/src/intc.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 14-01-19
#include "xil_exception.h"
#include   "xparameters.h"
#include          "intc.h"

#define INTC_ID XPAR_SCUGIC_SINGLE_DEVICE_ID

void intc_init(XScuGic *intc_p){
    XScuGic_Config *cfg_p;

    cfg_p = XScuGic_LookupConfig(INTC_ID);
    XScuGic_CfgInitialize(intc_p, cfg_p, cfg_p->CpuBaseAddress);
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, intc_p);
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_SWI_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, intc_p);
}

void intc_enable(){
    Xil_ExceptionEnable();
}

void intc_connect_isr(XScuGic *intc_p, u32 irq_id, void *isr){
    XScuGic_Connect(intc_p, irq_id, (Xil_ExceptionHandler) isr, (void*)irq_id);
    XScuGic_SetPriorityTriggerType(intc_p, irq_id, 0xA0, 0x03); /* Figure out these magic numbers */
    XScuGic_Enable(intc_p, irq_id);
}

void intc_trig_swi(u32 id, u32 cpu){
	u32 mask = ((cpu << 16U) | id) & (XSCUGIC_SFI_TRIG_CPU_MASK | XSCUGIC_SFI_TRIG_INTID_MASK);
	XScuGic_WriteReg(
		XPAR_PS7_INTC_DIST_0_S_AXI_BASEADDR,
		XSCUGIC_SFI_TRIG_OFFSET,
		mask
	);
}
