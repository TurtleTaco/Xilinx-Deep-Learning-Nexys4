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


struct kernel_command {
    uint16_t ifm_height;
    uint16_t ifm_width;
    uint16_t kernel_height;
    uint16_t kernel_width;
    uint16_t stride;
    uint16_t pad;
    uint16_t ofm_height;
    uint16_t ofm_width;
    uint16_t ifm_slices;
    uint16_t ofm_slices;
    uint16_t ofm_fragments;
    uint16_t ifm_mem_fragments;
    uint32_t ifm_baseaddr;
    uint32_t ifm_packet_length;
    uint32_t ifm_depth_offset;
    uint32_t ifm_height_offset;
    uint32_t ofm_baseaddr;
    uint32_t ofm_packet_length;
    uint32_t weight_baseaddr;
    uint32_t weight_packet_length;
    uint32_t weight_depth_offset;
    uint16_t data_mode;                 // SET TO “0” FOR 16-BIT
    uint16_t activation_pack;        // SET TO “1” FOR 16-BIT
    uint16_t pool_input_height;
    uint16_t pool_input_width;
    uint16_t pool_kernel_height;
    uint16_t pool_kernel_width;
    uint16_t pool_output_height;
    uint16_t pool_output_width;
    uint16_t pool_stride;
    uint16_t relu;
    uint32_t rsved[12];
};




int main()
{
    init_platform();

    PRINT("Hello Darius\n");
    PRINT("ifm_reshape size is %d bytes\n", sizeof(ifm_reshape));
    PRINT("weights_reshape size is %d bytes\n", sizeof(weights_reshape));

    // Generate command array
    uint8_t* cmd = malloc(12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 10*sizeof(uint16_t) + 12*sizeof(uint32_t));
    kernel_command cmd_all;
    cmd_all = (kernel_command){
        .ifm_height = 6;
        .ifm_width = 6;
        .kernel_height = 1;
        .kernel_width = 1;
        .stride = 1;
        .pad = 0;
        .ofm_height = 6;
        .ofm_width = 6;
        .ifm_slices = 1;
        .ofm_slices = 1;
        .ofm_fragments = 1;
        .ifm_mem_fragments = 1;
        .ifm_baseaddr = 2147483904;
        .ifm_packet_length = 36;
        .ifm_depth_offset = 288;
        .ifm_height_offset = 0;
        .ofm_baseaddr = 2147532800;
        .ofm_packet_length = 36;
        .weight_baseaddr = 2147491840;
        .weight_packet_length = 8;
        .weight_depth_offset = 64;
        .data_mode = 0;                 // SET TO “0” FOR 16-BIT
        .activation_pack = 1;        // SET TO “1” FOR 16-BIT
        .pool_input_height = 6;
        .pool_input_width = 6;
        .pool_kernel_height = 0;
        .pool_kernel_width = 0;
        .pool_output_height = 0;
        .pool_output_width = 0;
        .pool_stride = 0;
        .relu = 0;
        .rsved[12] = cmd_rsvd;
    }

    memcpy(cmd, cmd_all, 128);
    // memcpy(cmd, cmd_conv, 12*sizeof(uint16_t));
    // memcpy(cmd + 12*sizeof(uint16_t), cmd_addr, 9*sizeof(uint32_t));
    // memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t), cmd_pool, 10*sizeof(uint16_t));
    // memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 10*sizeof(uint16_t), cmd_rsvd, 12*sizeof(uint32_t));

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
