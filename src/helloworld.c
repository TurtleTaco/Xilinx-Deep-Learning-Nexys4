#include <stdio.h>
#include "stdlib.h"
#include "platform.h"
#include "xil_printf.h"

#define BRAMSIZE 128
#define ifm_len 288
#define weights_len 576
#define cmd_len 124

#define PRINT xil_printf

unsigned int * ddr = (unsigned int*) 0x80000000;

// BRAM
unsigned int * cmd_baseaddr = (unsigned int*) 0x80000000;
unsigned int * ifm_baseaddr = (unsigned int*) 0x80000100;
unsigned int * weights_baseaddr = (unsigned int*) 0x80002000; // need only up to 0xc0002100 because only 288 words
unsigned int * debug_baseaddr = (unsigned int*) 0x80000400;
unsigned int * ofm_baseaddr = (unsigned int*) 0x8000c000;

// DATAFLOW
unsigned int * CNNDATAFLOW_BASEADDR = (unsigned int*) 0x44a00000;
unsigned int * NUM_COMMANDS_BASEADDR = (unsigned int*) (0x44a00000 + 0x60);
unsigned int * CMD_BASEADDR_BASEADDR = (unsigned int*) (0x44a00000 + 0x70);
unsigned int * CYCLE_COUNT_BASEADDR = (unsigned int*) (0x44a00000 + 0xd0);

// Initializing DARIUS
int16_t ifm_reshape[ifm_len] = {
		90, 58, 68, 204, 115, 33, 193, 253, 0, 20, 99, 191, 176, 249, 43, 229, 71, 83, 70, 152,
		226, 252, 136, 131, 190, 165, 51, 76, 224, 116, 49, 116, 147, 185, 182, 226, 117, 184, 83, 184,
		85, 221, 134, 105, 198, 251, 154, 205, 95, 13, 145, 53, 246, 247, 76, 136, 202, 194, 166, 73,
		50, 246, 152, 136, 72, 177, 174, 166, 103, 84, 53, 107, 112, 79, 184, 73, 19, 174, 34, 109,
		228, 43, 62, 157, 162, 109, 219, 103, 81, 177, 182, 254, 95, 145, 126, 241, 129, 95, 136, 9,
		221, 149, 172, 193, 142, 82, 66, 230, 182, 225, 164, 242, 175, 120, 179, 250, 81, 136, 151, 77,
		41, 229, 82, 70, 73, 222, 46, 112, 3, 233, 6, 104, 104, 97, 87, 79, 105, 29, 9, 118,
		234, 76, 107, 152, 207, 236, 115, 3, 27, 127, 19, 1, 19, 56, 67, 199, 20, 196, 48, 55,
		32, 177, 230, 211, 222, 204, 82, 35, 85, 175, 152, 120, 20, 227, 44, 117, 241, 79, 46, 128,
		89, 92, 28, 188, 228, 139, 173, 127, 215, 24, 84, 217, 226, 206, 162, 174, 180, 129, 239, 152,
		7, 82, 206, 211, 24, 155, 180, 98, 36, 34, 252, 89, 178, 186, 89, 236, 204, 0, 214, 159,
		65, 220, 195, 102, 220, 6, 56, 8, 183, 34, 186, 201, 54, 12, 12, 149, 63, 221, 50, 85,
		197, 64, 195, 196, 227, 246, 202, 38, 146, 65, 16, 170, 74, 199, 87, 90, 52, 107, 79, 91,
		89, 20, 240, 30, 128, 124, 247, 238, 117, 78, 152, 162, 196, 170, 192, 230, 158, 237, 89, 220,
		166, 90, 140, 156, 114, 96, 78, 227
	};

int16_t weights_reshape[weights_len] = {
	242, 120, 126, 236, 25, 84, 69, 122, 17, 88, 129, 151, 91, 87, 214, 118, 150, 163, 56, 90,
	13, 223, 106, 45, 129, 39, 168, 172, 206, 64, 146, 8, 209, 65, 45, 153, 38, 227, 191, 153,
	77, 195, 143, 200, 100, 112, 29, 101, 238, 235, 30, 42, 144, 235, 139, 210, 49, 1, 53, 243,
	193, 231, 95, 107, 191, 63, 209, 2, 71, 244, 171, 218, 139, 240, 70, 36, 224, 62, 227, 50,
	16, 128, 118, 58, 200, 165, 173, 36, 12, 245, 88, 125, 200, 150, 37, 159, 140, 49, 75, 40,
	109, 238, 120, 60, 116, 168, 152, 49, 220, 148, 70, 224, 140, 164, 83, 231, 247, 43, 205, 60,
	118, 223, 99, 198, 159, 120, 103, 253, 90, 29, 68, 178, 188, 3, 71, 125, 51, 147, 76, 236,
	223, 119, 218, 172, 2, 156, 75, 7, 254, 32, 215, 125, 112, 19, 162, 24, 180, 101, 142, 253,
	11, 70, 64, 222, 94, 17, 86, 240, 144, 202, 171, 110, 188, 171, 176, 200, 63, 118, 151, 137,
	165, 126, 19, 70, 159, 76, 168, 211, 107, 239, 43, 33, 250, 38, 46, 167, 177, 18, 111, 38,
	117, 152, 240, 106, 167, 56, 50, 165, 160, 26, 228, 22, 49, 71, 23, 133, 101, 253, 173, 241,
	222, 137, 190, 56, 252, 146, 100, 225, 245, 233, 219, 35, 112, 84, 239, 58, 103, 107, 3, 76,
	219, 157, 189, 54, 12, 124, 158, 82, 146, 216, 162, 66, 31, 91, 157, 99, 69, 133, 203, 45,
	124, 254, 174, 251, 10, 182, 48, 80, 80, 234, 132, 236, 204, 84, 122, 127, 10, 210, 149, 195,
	215, 140, 51, 44, 20, 213, 16, 168, 209, 77, 25, 154, 236, 152, 210, 52, 83, 155, 97, 78,
	119, 238, 133, 232, 244, 125, 60, 197, 141, 195, 218, 82, 28, 93, 91, 138, 23, 228, 49, 145,
	7, 248, 78, 54, 253, 106, 172, 239, 109, 5, 4, 73, 194, 240, 121, 150, 149, 19, 130, 33,
	91, 242, 247, 254, 176, 65, 8, 77, 228, 123, 8, 170, 161, 174, 225, 230, 70, 92, 50, 67,
	12, 179, 96, 179, 250, 243, 164, 245, 9, 231, 214, 163, 109, 12, 21, 62, 93, 61, 239, 22,
	84, 43, 237, 203, 243, 113, 164, 245, 40, 233, 112, 137, 62, 248, 249, 158, 161, 21, 81, 11,
	237, 52, 79, 195, 171, 194, 56, 37, 180, 217, 146, 36, 58, 170, 163, 59, 60, 231, 165, 166,
	13, 114, 10, 38, 57, 99, 114, 31, 239, 67, 183, 91, 125, 27, 73, 120, 133, 140, 200, 91,
	20, 177, 81, 165, 240, 169, 155, 17, 101, 247, 71, 159, 177, 216, 249, 149, 158, 139, 66, 184,
	78, 70, 17, 121, 153, 34, 164, 138, 108, 248, 151, 77, 39, 1, 224, 39, 134, 151, 2, 67,
	112, 71, 216, 90, 237, 15, 31, 166, 213, 108, 11, 224, 119, 156, 123, 155, 45, 75, 242, 77,
	202, 207, 133, 166, 154, 149, 145, 231, 61, 156, 7, 242, 82, 59, 41, 157, 66, 130, 3, 163,
	86, 223, 203, 107, 187, 90, 11, 34, 165, 0, 37, 169, 125, 32, 104, 236, 90, 240, 107, 197,
	219, 35, 186, 46, 225, 51, 94, 70, 58, 46, 2, 69, 91, 199, 183, 115, 7, 209, 205, 14,
	201, 91, 75, 247, 28, 190, 158, 109, 92, 17, 191, 144, 33, 120, 203, 167
};

//unsigned char cmd[cmd_len] = {0x06,0x00,0x06,0x00,0x03,0x00,0x03,0x00,0x01,0x00,0x00,0x00,0x04,0x00,0x04,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x01,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint16_t cmd_conv[12] = {6, 6, 3, 3, 1, 0, 4, 4, 1, 1, 1, 1};
uint32_t cmd_addr[9] = {2147483904, 36, 288, 0, 2148270080, 16, 2147491840, 72, 576};
uint16_t cmd_mode[2] = {0, 0};
uint16_t cmd_pool[8] = {4, 4, 0, 0, 0, 0, 0, 0};
uint32_t cmd_rsvd[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Total size is 12*16+9*32+2*16+8*16+12*32 = 1024

int main()
{
    init_platform();

    PRINT("Hello Darius\n");
    PRINT("ifm_reshape size is %d bytes\n", sizeof(ifm_reshape));
    PRINT("weights_reshape size is %d bytes\n", sizeof(weights_reshape));

    // Generate command array
    uint8_t* cmd = malloc(12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 2*sizeof(uint16_t) + 8*sizeof(uint16_t) + 12*sizeof(uint32_t));
    memcpy(cmd, cmd_conv, 12*sizeof(uint16_t));
    memcpy(cmd + 12*sizeof(uint16_t), cmd_addr, 9*sizeof(uint32_t));
    memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t), cmd_mode, 2*sizeof(uint16_t));
    memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 2*sizeof(uint16_t), cmd_pool, 8*sizeof(uint16_t));
    memcpy(cmd + 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 2*sizeof(uint16_t) + 8*sizeof(uint16_t), cmd_rsvd, 12*sizeof(uint32_t));

    PRINT("cmd size is %d bytes\n", 12*sizeof(uint16_t) + 9*sizeof(uint32_t) + 2*sizeof(uint16_t) + 8*sizeof(uint16_t) + 12*sizeof(uint32_t));

    PRINT("H: %d", cmd[0]);

	// write ifm ,weights and cmd to BRAM
	memcpy(ifm_baseaddr, ifm_reshape, sizeof(ifm_reshape));
	memcpy(weights_baseaddr, weights_reshape, sizeof(weights_reshape));
	memcpy(debug_baseaddr, weights_reshape, sizeof(weights_reshape));
	memcpy(cmd_baseaddr, cmd, 128);

	//unsigned int * debug_addr = (unsigned int*) 0xc0000000;
	//memcpy(debug_addr, cmd, 128);

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
