/*
 * can_wrapper.c
 *
 *  Created on: Sep 4, 2019
 *      Author: Eskil
 */

#include "can_wrapper.h"

#include "xcanps.h"         // For can stuff
#include "netif/xadapter.h" // For u64_t
#include "platform.h"       // For get_time_us()

/*
 * Driver instance
 */
// Todo, add more interfaces
static XCanPs can_0;

int can_wrapper_init(uint16_t device_id)
{
    int status;
    XCanPs *can_instance = &can_0;
    XCanPs_Config *config_ptr;

    /*
     * Initialize the Can device.
     */
    config_ptr = XCanPs_LookupConfig(device_id);
    if (can_instance == NULL) 
    {
        return XST_FAILURE;
    }

    status = XCanPs_CfgInitialize(can_instance,
                                  config_ptr,
                                  config_ptr->BaseAddr);
    if(status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }

    /*
     * Self test can device
     */
    status = XCanPs_SelfTest(can_instance);
    if(status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }

    /*
     * Enter config mode to set up prescaler and bit timings
     */
    XCanPs_EnterMode(can_instance, XCANPS_MODE_CONFIG);
    while(XCanPs_GetMode(can_instance) != XCANPS_MODE_CONFIG);

    /*
     * Set BRPR and BTR
     */
    XCanPs_SetBaudRatePrescaler(can_instance,
                                BRPR_PRESCALER);
    XCanPs_SetBitTiming(can_instance,
                        BTR_SJW - 1,
                        BTR_TSEG_2 - 1,
                        BTR_TSEG_1 - 1);
    
    /*
     * Enter normal mode
     */
    XCanPs_EnterMode(can_instance, XCANPS_MODE_NORMAL);
    while(XCanPs_GetMode(can_instance) != XCANPS_MODE_NORMAL);

    return status;
}

int can_wrapper_receive_frame(can_frame_t *frame)
{
    int status;
    XCanPs *can_instance = &can_0;

    uint32_t rx_frame[XCANPS_MAX_FRAME_SIZE / sizeof(uint32_t)];

    /*
     * Check if RX FIFO is empty, throw error if so
     */
    if(XCanPs_IsRxEmpty(can_instance) == TRUE) {
        return XST_FAILURE;
    }

    /*
     * Retrieve CAN frame from RX buffer
     */
    status = XCanPs_Recv(can_instance, rx_frame);

    if(XST_SUCCESS != status) {
        return XST_FAILURE;
    }

    /*
     * Fill message with data
     * 
     * Retrieving can id and dlc length, inverse of:
     *  XCanPs_CreateIdValue
     *  XCanPs_CreateDlcValue
     */
    frame->can_id = (rx_frame[0] & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
    frame->data_length = (rx_frame[1] & XCANPS_DLCR_DLC_MASK) >> XCANPS_DLCR_DLC_SHIFT;
    frame->timestamp = get_time_us();
    // Since normal can, dlc corresponds to bytes in message
    memcpy(frame->data, &rx_frame[2], frame->data_length); // TODO double check if i fked up here

    // REMOVE pls
    static u8 counter = 0;
    frame->data[0] = counter++;

    return status;
}

int can_wrapper_send_frame(can_frame_t const * const frame)
{
    XCanPs *can_instance = &can_0;
    uint32_t tx_frame[XCANPS_MAX_FRAME_SIZE / sizeof(uint32_t)];

    /*
	 * Set values for id and datalength code
	 */
	tx_frame[0] = (uint32_t)XCanPs_CreateIdValue((uint32_t)frame->can_id, 0, 0, 0, 0);
	tx_frame[1] = (uint32_t)XCanPs_CreateDlcValue((uint32_t)frame->data_length);

    /*
     * Copy over data
     */
    memcpy(&tx_frame[2], frame->data, frame->data_length);

    /*
     * Check if buffer full, throw error if so
     */
    if(XCanPs_IsTxFifoFull(can_instance) == TRUE) {
        return XST_FAILURE;
    }

    /*
     * Send the frame
     */
    return XCanPs_Send(can_instance, tx_frame);
}
