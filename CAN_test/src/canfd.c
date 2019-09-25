//   _____ _____ _____
//  |   | |__   |   | | cpu0/src/zcan/canfd.c
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 16-04-19
#include "xil_exception.h"

#include "canfd.h"
//#include "../hsm/hsm.h"
//#include "../utility/log.h"
/* Forward Function Declarations: */
static void  canfd_cfg(XCanFd *canfd_p, u32 id, XScuGic *intc_p);

static void canfd_isr_tx(void *cb_ref);
static void canfd_isr_rx(void *cb_ref);
static void canfd_isr_ev(void *cb_ref);
static void canfd_isr_er(void *cb_ref);

static   u8 canfd_get_len(u32 *frame);
static   u8 canfd_is_ext(u32 *frame);
static  u32 canfd_get_id(u32 *frame);
static void canfd_print_errors(XCanFd *canfd_p);

/* Function Definitions: */
void canfd_init(XCanFd *canfd_p, XScuGic *intc_p, u32 id){
    XCanFd_Reset(canfd_p);
    XCanFd_Config *cfg_p = XCanFd_LookupConfig(id);
    XCanFd_CfgInitialize(canfd_p, cfg_p, cfg_p->BaseAddress);
    canfd_cfg(canfd_p, id, intc_p);
}

static void canfd_cfg(XCanFd *canfd_p, u32 id, XScuGic *intc_p){
    XCanFd_EnterMode(canfd_p, XCANFD_MODE_CONFIG);
    while(XCanFd_GetMode(canfd_p) != XCANFD_MODE_CONFIG);

    /*
     *  Bit-timing configuration:
     *
     *  The CAN-FD core adds 1 to all bit timing values,
     *  hence 1 is subtracted from the defined values.
     */
    XCanFd_SetBaudRatePrescaler(canfd_p, BRP_NOM - 1);
    XCanFd_SetBitTiming(canfd_p, SJW_NOM - 1, TS2_NOM - 1, TS1_NOM - 1);
    XCanFd_SetFBaudRatePrescaler(canfd_p, BRP_FD - 1);
    XCanFd_SetFBitTiming(canfd_p, SJW_FD - 1, TS2_FD - 1, TS1_FD - 1);

    /*
     *  Acceptance filter refresh:
     */
    XCanFd_AcceptFilterDisable(canfd_p, XCANFD_AFR_UAF_ALL_MASK); /* Unclear if this is necessary */
    XCanFd_AcceptFilterEnable(canfd_p, XCANFD_AFR_UAF_ALL_MASK);

    /*
     *  Bit-rate switch (BRS) configuration:
     *
     *  EnableNominal uses nominal bit-rate for payload,
     *  while DisableNominal leaves bit-rate switching to the application.
     */
#ifdef ENABLE_BRS
    XCanFd_SetBitRateSwitch_DisableNominal(canfd_p);
#else
    XCanFd_SetBitRateSwitch_EnableNominal(canfd_p);
#endif

    /*
     *  Transceiver Delay Compensation (TDC) configuration:
     *
     *  Compensates for the delay between TX falling edge and
     *  RX falling edge.
     *
     *  Delay comes from PHY TX to RX loop and trace lengths.
     */
    XCanFd_Enable_Tranceiver_Delay_Compensation(canfd_p);
    XCanFd_Set_Tranceiver_Delay_Compensation(canfd_p, TXRX_DELAY);

    /*
     *  Interrupt Request (IRQ) configuration:
     *
     *  (1) Determine IRQ ID and mask.
     *  (2) Map interrupt service routines (ISRs) to their respective IRQ events.
     *  (3) Set IRQ priority and trigger type.
     *  (4) Connect the device IRQ handler to the hardware IRQ handler.
     *  (5) Enable IRQs, both in device and in HW.
     */
    u32 irq_id;
    u32 irq_mask;
    switch(id){
        case CANFD0_ID:
            irq_id   =   CANFD0_IRQ_ID;
            irq_mask = CANFD0_IRQ_MASK;
            break;
        case CANFD1_ID:
            irq_id   =   CANFD1_IRQ_ID;
            irq_mask = CANFD1_IRQ_MASK;
            break;
        default:
            return;
    }

    XCanFd_SetHandler(canfd_p, XCANFD_HANDLER_SEND,  (void*) canfd_isr_tx, (void*) canfd_p);
    XCanFd_SetHandler(canfd_p, XCANFD_HANDLER_RECV,  (void*) canfd_isr_rx, (void*) canfd_p);
    XCanFd_SetHandler(canfd_p, XCANFD_HANDLER_ERROR, (void*) canfd_isr_er, (void*) canfd_p);
    XCanFd_SetHandler(canfd_p, XCANFD_HANDLER_EVENT, (void*) canfd_isr_ev, (void*) canfd_p);

    XScuGic_SetPriorityTriggerType(intc_p, irq_id, 0xA0, 0x3); /* Figure out these magic numbers */
    XScuGic_Connect(intc_p, irq_id, (Xil_ExceptionHandler) XCanFd_IntrHandler, canfd_p);

    XScuGic_Enable(intc_p, irq_id);
    XCanFd_InterruptEnable(canfd_p, irq_mask);

    XCanFd_EnterMode(canfd_p, XCANFD_MODE_NORMAL);
    while(XCanFd_GetMode(canfd_p) != XCANFD_MODE_NORMAL);
}

//s32 canfd_rx(XCanFd *canfd_p, CanardCANFrame *p_frame){
//    u32 raw_buf[18];
//
//    s32 status = XCanFd_Recv_Sequential(canfd_p, raw_buf);
//    if(status != XST_SUCCESS){return XST_FAILURE;}
//
//    p_frame->data_len = canfd_get_len(raw_buf);
//    p_frame->id = canfd_get_id(raw_buf);
//    memcpy(p_frame->data, &raw_buf[2], p_frame->data_len);
//
//    if(canfd_is_ext(raw_buf)){
//        p_frame->id |= CANARD_CAN_FRAME_EFF;
//    }
//
//    return XST_SUCCESS;
//}

//s32 canfd_tx(XCanFd *canfd_p, const CanardCANFrame *const p_frame){
//    u32 raw_buf[18];
//
//    u32 id_ext = p_frame->id & 0x0003FFFF;
//    u32 id_std = (p_frame->id >> 18) & 0x07FF;
//
//    raw_buf[0] = XCanFd_CreateIdValue(id_std, 0, 1, id_ext, 0);
//    raw_buf[1] = XCanFd_Create_CanFD_Dlc_BrsValue(XCanFd_GetLen2Dlc(p_frame->data_len));
//    memcpy(&raw_buf[2], p_frame->data, p_frame->data_len);
//
//    return XCanFd_Send(canfd_p, raw_buf, NULL);
//}

s32 canfd_tx_std(XCanFd *canfd_p, u8 *data, u32 id, u8 len){
    u32 raw_buf[18];

    raw_buf[0] = XCanFd_CreateIdValue(id, 0, 0, 0, 0);
    raw_buf[1] = XCanFd_Create_CanFD_Dlc_BrsValue(XCanFd_GetLen2Dlc(len));
    memcpy(&raw_buf[2], data, len);

    return XCanFd_Send(canfd_p, raw_buf, NULL);
}

u8 canfd_get_rx_buf_count(XCanFd *canfd_p){
    return (u8)XCanFd_GetNofMessages_Stored_Rx_Fifo(canfd_p, 0);
}


static void canfd_isr_tx(void *cb_ref){
    // TODO: Determine if this ISR is necessary at all.
}

static void canfd_isr_rx(void *cb_ref){
//    if(((XCanFd *)cb_ref)->CanFdConfig.DeviceId == CANFD0_ID){
//        hsm_add_ev_isr(EV_RX_CANFD0);
//    }
//    else if(((XCanFd *)cb_ref)->CanFdConfig.DeviceId == CANFD1_ID){
//        hsm_add_ev_isr(EV_RX_CANFD1);
//    }
    xil_printf("CPU0_CANFD_ISR_RX - Received frame.\r\n");
}

static void canfd_isr_ev(void *cb_ref){
    // TODO: Configure event handling.
}

static void canfd_isr_er(void *cb_ref){
    canfd_print_errors((XCanFd*)cb_ref);
    // TODO: Configure error handling.
}

static u8 canfd_get_len(u32 *frame){
    return (u8) XCanFd_GetDlc2len(frame[1] & XCANFD_DLCR_DLC_MASK);
}

static u8 canfd_is_ext(u32 *frame){
    return (u8) ((frame[0] & XCANFD_IDR_IDE_MASK) >> XCANFD_IDR_IDE_SHIFT);
}

static u32 canfd_get_id(u32 *frame){
    u32 id = (frame[0] & XCANFD_IDR_ID1_MASK) >> XCANFD_IDR_ID1_SHIFT;

    if(canfd_is_ext(frame)){
        id <<= 18; /* HOW DOES CAN-FRAMES WORK!? */
        id |= (frame[0] & XCANFD_IDR_ID2_MASK) >> XCANFD_IDR_ID2_SHIFT;
    }
    return id;
}

static void canfd_print_errors(XCanFd *canfd_p){
    u32 esr = XCanFd_GetBusErrorStatus(canfd_p);

    xil_printf("CAN-FD Core Errors:\r\n");
    if((esr & XCANFD_ESR_ACKER_MASK) != 0){
        xil_printf("ACK Error\r\n");
    }
    if((esr & XCANFD_ESR_BERR_MASK) != 0){
        xil_printf("Bit Error\r\n");
    }
    if((esr & XCANFD_ESR_STER_MASK) != 0){
        xil_printf("Stuff Error\r\n");
    }
    if((esr & XCANFD_ESR_FMER_MASK) != 0){
        xil_printf("Form Error\r\n");
    }
    if((esr & XCANFD_ESR_CRCER_MASK) != 0){
        xil_printf("CRC Error\r\n");
    }
    if((esr & XCANFD_ESR_F_BERR_MASK) != 0){
        xil_printf("FD Bit Error\r\n");
    }
    if((esr & XCANFD_ESR_F_STER_MASK) != 0){
        xil_printf("FD Stuff Error\r\n");
    }
    if((esr & XCANFD_ESR_F_FMER_MASK) != 0){
        xil_printf("FD Form Error\r\n");
    }
    if((esr & XCANFD_ESR_F_CRCER_MASK) != 0){
        xil_printf("FD CRC Error\r\n");
    }

}
