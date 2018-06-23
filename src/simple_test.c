#include <stdio.h>
#include "stdlib.h"
#include "platform.h"
#include "xil_printf.h"

#define ifm_len 288
#define weights_len 64

#define PRINT xil_printf

// BRAM
unsigned int * cmd_baseaddr = (unsigned int*) 0x80000000;
unsigned int * ifm_baseaddr = (unsigned int*) 0x80000100;
unsigned int * weights_baseaddr = (unsigned int*) 0x80002000; // need only up to 0xc0002100 because only 288 words
unsigned int * ofm_baseaddr = (unsigned int*) 0x8000c000;

// DATAFLOW
unsigned int * CNNDATAFLOW_BASEADDR = (unsigned int*) 0x44a00000;
unsigned int * NUM_COMMANDS_BASEADDR = (unsigned int*) (0x44a00000 + 0x60);
unsigned int * CMD_BASEADDR_BASEADDR = (unsigned int*) (0x44a00000 + 0x70);
unsigned int * CYCLE_COUNT_BASEADDR = (unsigned int*) (0x44a00000 + 0xd0);

// Initializing DARIUS
int16_t ifm_reshape[ifm_len] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1
	};

int16_t weights_reshape[weights_len] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1
};

uint16_t cmd_conv[12] = {6, 6, 1, 1, 1, 0, 6, 6, 1, 1, 1, 1};
uint32_t cmd_addr[9] = {2147483904, 36, 288, 0, 2147532800, 36, 2147491840, 8, 64};
// uint16_t cmd_mode[2] = {0, 0};
uint16_t cmd_pool[10] = {0, 1, 6, 6, 0, 0, 0, 0, 0, 0};
uint32_t cmd_rsvd[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main()
{
    init_platform();

    PRINT("Hello Darius\n");
    PRINT("ifm_reshape size is %d bytes\n", sizeof(ifm_reshape));
    PRINT("weights_reshape size is %d bytes\n", sizeof(weights_reshape));

    // Generate command array
    uint8_t* cmd = malloc(12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 10*sizeof(uint16_t) + 12*sizeof(uint32_t));
    memcpy(cmd, cmd_conv, 12*sizeof(uint16_t));
    memcpy(cmd + 12*sizeof(uint16_t), cmd_addr, 9*sizeof(uint32_t));
    memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t), cmd_pool, 10*sizeof(uint16_t));
    memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 10*sizeof(uint16_t), cmd_rsvd, 12*sizeof(uint32_t));

    PRINT("cmd size is %d bytes\n", 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 10*sizeof(uint16_t) + 12*sizeof(uint32_t));

	// write ifm ,weights and cmd to BRAM
	memcpy(ifm_baseaddr, ifm_reshape, sizeof(ifm_reshape));
	memcpy(weights_baseaddr, weights_reshape, sizeof(weights_reshape));
	memcpy(cmd_baseaddr, cmd, 128);

	// write DATAFLOW attributes
	*(NUM_COMMANDS_BASEADDR) = 1;
	*(CMD_BASEADDR_BASEADDR) = 0x80000000;

	// read CNNDataflow IP state

	unsigned int state = *(CNNDATAFLOW_BASEADDR + 0x0);
	if (state == 4){
		PRINT("state: IP IDLE\nStarting IP \n");
		*(CNNDATAFLOW_BASEADDR + 0x0) = 1;
		PRINT("state: IP Started \n");
	}
	else {
		PRINT("state %d: IP BUSY \n", state);
	}

	// read CNNDataflow IP state for DONE
	int iter_count = 0;
	while (1){
		unsigned int done = *(CNNDATAFLOW_BASEADDR + 0x0);
		if (done == 6){
			PRINT("state: IP DONE \n");
			break;
		}
		else {
			PRINT("%d . ", done);
			PRINT("%d\n", iter_count);
			iter_count += 1;
		}
	}

	// pull results from CYCLE_COUNT_BASEADDR
	unsigned int hw_cycle = *(CYCLE_COUNT_BASEADDR); // BRAM is word addressable, hw_cycle is 4 bytes
	PRINT("CNNDataflow IP cycles: %d \n", hw_cycle);

    cleanup_platform();
    return 0;
}
