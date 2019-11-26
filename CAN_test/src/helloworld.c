#include "xparameters.h"
#include "xil_printf.h"
#include "canfd.h"
#include "intc.h"
#include "xil_types.h"

#include "sleep.h"

#include "gpio.h"

static XCanFd canfd0_i;
static XCanFd canfd1_i;

static XScuGic intc_i;

#include "xiic.h"
#include "xiic_l.h"

XIic iic;

#define IIC_ADDRESS 0x78

int write_adc(u32 iic_addr, u8 data);
int read_adc(u32 iic_addr, u8 bytes_to_read, u8 * data);

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

    gpio_initialize();

    u32 data = 0xA5A5;

	int led_on = 0;


	u8 adc_config = 0xC2;
	u8 adc_data[3];



	while(1)
    {
		sleep(1);
		write_adc(XPAR_IIC_0_BASEADDR,
					  adc_config);
		read_adc(XPAR_IIC_0_BASEADDR,
				 3,
				 adc_data);
		if(adc_data[0] == adc_config) {
			xil_printf("Config equal\n");
		}
		else {
			xil_printf("Config not equal!\n");
		}
		xil_printf("dat[1]: %d dat[2]: %d\n", adc_data[1], adc_data[2]);
		xil_printf("measured %d\n", ((adc_data[1] & 0xF) << 6) | (adc_data[2] >> 1));

//		if(led_on)
//		{
//			xil_printf("led on\n");
//			gpio_set(LED_GREEN_0);
//			gpio_set(LED_GREEN_1);
//			gpio_set(LED_RED_0);
//			gpio_set(LED_RED_1);
//		}
//		else{
//			xil_printf("led off\n");
//			gpio_clear(LED_GREEN_0);
//			gpio_clear(LED_GREEN_1);
//			gpio_clear(LED_RED_0);
//			gpio_clear(LED_RED_1);
//		}
//		led_on = !led_on;
//	    canfd_tx_std(&canfd0_i, (u8 * )&data, 0x50, 4);
//	    canfd_tx_std(&canfd1_i, (u8 * )&data, 0x51, 4);
    }

    return XST_SUCCESS;
}

int write_adc(u32 iic_addr, u8 data)
{
	int status = XST_SUCCESS;
	u8 tx_buffer[4];
	tx_buffer[0] = data;
	int bytes_to_send = 1;

	int byte_count = XIic_Send(iic_addr,
							   IIC_ADDRESS,
							   tx_buffer,
							   bytes_to_send,
							   XIIC_STOP);

	if(byte_count != bytes_to_send) {
		status = XST_FAILURE;
	}

	return status;
}

int read_adc(u32 iic_addr, u8 bytes_to_read, u8 * data) {
	int status;

	int byte_count = XIic_Recv(iic_addr,
			                   IIC_ADDRESS,
							   data,
							   bytes_to_read,
							   XIIC_STOP);

	if(byte_count != bytes_to_read) {
		status = XST_FAILURE;
	}
	return status;
}
