#include <stdio.h>
#include <inttypes.h>
#include "stdlib.h"
#include "platform.h"
#include "xil_printf.h"

#define ifm_len 288
#define weights_len 64

#define PRINT xil_printf

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
    uint16_t data_mode;                 // SET TO 0 FOR 16-BIT
    uint16_t activation_pack;        // SET TO 1 FOR 16-BIT
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
uint16_t cmd_pool[10] = {0, 1, 6, 6, 0, 0, 0, 0, 0, 0};
uint32_t cmd_rsvd[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void struct_init(struct kernel_command *cmd_ptr){
	cmd_ptr->ifm_height = 6;
	cmd_ptr->ifm_width = 6;
	cmd_ptr->kernel_height = 1;
	cmd_ptr->kernel_width = 1;
	cmd_ptr->stride = 1;
	cmd_ptr->pad = 0;
	cmd_ptr->ofm_height = 6;
	cmd_ptr->ofm_width = 6;
	cmd_ptr->ifm_slices = 1;
	cmd_ptr->ofm_slices = 1;
	cmd_ptr->ofm_fragments = 1;
	cmd_ptr->ifm_mem_fragments = 1;
	cmd_ptr->ifm_baseaddr = 2147483904;
	cmd_ptr->ifm_packet_length = 36;
	cmd_ptr->ifm_depth_offset = 288;
	cmd_ptr->ifm_height_offset = 0;
	cmd_ptr->ofm_baseaddr = 2147532800;
	cmd_ptr->ofm_packet_length = 36;
	cmd_ptr->weight_baseaddr = 2147491840;
	cmd_ptr->weight_packet_length = 8;
	cmd_ptr->weight_depth_offset = 64;
	cmd_ptr->data_mode = 0;                 // SET TO “0” FOR 16-BIT
	cmd_ptr->activation_pack = 1;        // SET TO “1” FOR 16-BIT
	cmd_ptr->pool_input_height = 6;
	cmd_ptr->pool_input_width = 6;
	cmd_ptr->pool_kernel_height = 0;
	cmd_ptr->pool_kernel_width = 0;
	cmd_ptr->pool_output_height = 0;
	cmd_ptr->pool_output_width = 0;
	cmd_ptr->pool_stride = 0;
	cmd_ptr->relu = 0;
	memcpy(cmd_ptr->rsved, cmd_rsvd, sizeof(cmd_ptr->rsved));
}


int main()
{
    init_platform();

    PRINT("Hello Darius\n");
    PRINT("ifm_reshape size is %d bytes\n", sizeof(ifm_reshape));
    PRINT("weights_reshape size is %d bytes\n", sizeof(weights_reshape));

    // Generate command struct
    struct kernel_command cmd_all;
    struct_init(&cmd_all);

    //ofm reveiver
    int16_t output[288] = {0};

    PRINT("cmd size is %d bytes\n", sizeof(struct kernel_command));

	// write ifm ,weights and cmd to BRAM
	memcpy(ifm_baseaddr, ifm_reshape, sizeof(ifm_reshape));
	memcpy(weights_baseaddr, weights_reshape, sizeof(weights_reshape));
//	PRINT("base: %d \n", cmd_all.ifm_baseaddr);
	memcpy(cmd_baseaddr, &cmd_all, sizeof(struct kernel_command));

	// write DATAFLOW attributes
	*(NUM_COMMANDS_BASEADDR) = 1;
	*(CMD_BASEADDR_BASEADDR) = 0x80000000;

	// read CNNDataflow IP state
	unsigned int state = *(CNNDATAFLOW_BASEADDR + 0x0);
	PRINT("state now is : %d \n", state);
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

	// pull ofm
	PRINT("The output is: \n");
	memcpy(output, ofm_baseaddr, 576);
	for (int index = 0; index < 288; index++){

		if (index % 8 == 0 && index != 0){
			printf("\n");
		}
		if (index % 48 == 0 && index != 0){
			printf("\n");
		}
		printf("%" PRId16 " ", output[index]);
	}

    cleanup_platform();
    return 0;
}
