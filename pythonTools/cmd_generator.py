from random import *
import numpy as np
from math import *

# Input Feature Map (IFM) dimensions
ifm_height = 6
ifm_width = 6
ifm_depth = 8 # has to be greater or equal to C_NUM_OF_ROWS

# Kernel Window dimensions
kernel_height = 1
kernel_width = 1

# Other arguments
pad = 0
stride = 1

'''
input: w * h * c = (6 * 6 * 8)

weights: w * h * c = (3 * 3 * 8) * 8 / stride: 1 / pad: 0

pool: w * h = 2 * 2 / stride: 2



'''

# Channels
channels = 8 # has to be greater or equal to C_NUM_OF_COLS

# Maxpool dimensions
pool_kernel_height = 2
pool_kernel_width = 2
pool_stride = 2

# CNNDataflow IP Constants
C_MAX_ADDR_WIDTH = 12
C_MAX_ITER_WIDTH = 6
C_MAX_IMG_DIMENSION_WIDTH = 10
C_MAX_INPUT_WIDTH = 16
C_NUM_OF_ROWS = 8
C_NUM_OF_COLS = 8

# address specific
cmd_baseaddr = 0x80000000
# ifm_baseaddr = 0x80000100
ifm_baseaddr = 0x8000c000 # for conv2
weights_baseaddr = 0x80002000
ofm_baseaddr = 0x8000c000

CNNDATAFLOW_BASEADDR = 0x44a00000
NUM_COMMANDS_OFFSET = 0x60
CMD_BASEADDR_OFFSET = 0x70
CYCLE_COUNT_OFFSET = 0xd0


# # Input Feature Map (IFM) dimensions
# ifm_height = 14
# ifm_width = 14
# ifm_depth = 64 # has to be greater or equal to C_NUM_OF_ROWS
#
# # Kernel Window dimensions
# kernel_height = 3
# kernel_width = 3
#
# # Other arguments
# pad = 0
# stride = 1
#
# # Channels
# channels = 32 # has to be greater or equal to C_NUM_OF_COLS
#
# # Maxpool dimensions
# pool_kernel_height = 2
# pool_kernel_width = 2
# pool_stride = 2
#
# # CNNDataflow IP Constants
# C_MAX_ADDR_WIDTH = 12
# C_MAX_ITER_WIDTH = 6
# C_MAX_IMG_DIMENSION_WIDTH = 10
# C_MAX_INPUT_WIDTH = 16
# C_NUM_OF_ROWS = 8
# C_NUM_OF_COLS = 8
#
# # address specific
# cmd_baseaddr = 0xc0000000
# ifm_baseaddr = 0xc0000100 # 3221225728
# weights_baseaddr = 0xc0008000 # 3221258240
# ofm_baseaddr = 0xc00c0000 # 3222011904
#
# CNNDATAFLOW_BASEADDR = 0x43C00000
# NUM_COMMANDS_OFFSET = 0x60
# CMD_BASEADDR_OFFSET = 0x70
# CYCLE_COUNT_OFFSET = 0xd0

class Darius(object):
    def __init__(self, ifm_height, ifm_width, ifm_depth, kernel_height,
                 kernel_width, pad, stride, channels,
                 pool_kernel_height, pool_kernel_width, pool_stride,
                 ifm_baseaddr, weights_baseaddr, ofm_baseaddr):
        """Return a new Convolution with Maxpool object"""

        self.ifm_height = ifm_height
        self.ifm_width = ifm_width
        self.ifm_depth = ifm_depth
        self.kernel_height = kernel_height
        self.kernel_width = kernel_width
        self.pad = pad
        self.stride = stride
        self.channels = channels
        self.pool_kernel_height = pool_kernel_height
        self.pool_kernel_width = pool_kernel_width
        self.pool_stride = pool_stride
        self.ifm_baseaddr = ifm_baseaddr
        self.weights_baseaddr = weights_baseaddr
        self.ofm_baseaddr = ofm_baseaddr

        def derive_attributes():
            self.ofm_height = ceil((self.ifm_height + 2 * self.pad - self.kernel_height) / self.stride + 1)
            self.ofm_width = ceil((self.ifm_width + 2 * self.pad - self.kernel_width) / self.stride + 1)
            self.ofm_depth = self.channels
            self.ifm_slices = ceil(self.ifm_depth / C_NUM_OF_ROWS)
            self.ofm_slices = ceil(self.channels / C_NUM_OF_COLS)
            self.ofm_fragments = 1
            self.ifm_mem_fragments = 1

            self.ifm_packet_length = self.ifm_width * self.ifm_height * self.ifm_slices
            self.ifm_depth_offset = self.ifm_width * self.ifm_height * self.ifm_depth
            self.ifm_height_offset = 0

            self.ofm_offset = self.ofm_height * self.ofm_width * self.ofm_depth

            self.weights_packet_length = self.kernel_height * self.kernel_width * self.ifm_depth
            self.weight_depth_offset = self.kernel_height * self.kernel_width * self.ifm_depth * \
                                       C_NUM_OF_COLS
            self.weight_offset = self.kernel_height * self.kernel_width * self.ifm_depth
            self.weight_pkt_offset = C_NUM_OF_ROWS * self.kernel_height * self.kernel_width

            self.reserved = 0

            self.pool_input_height = self.ofm_height
            self.pool_input_width = self.ofm_width
            # divid by one so to make it float calculation
            try:
                self.pool_output_height = ceil(
                    ((self.pool_input_height - self.pool_kernel_height) / 1.0) / self.pool_stride / 1.0 + 1)
                self.pool_output_width = ceil(
                    ((self.pool_input_width - self.pool_kernel_width) / 1.0) / self.pool_stride / 1.0 + 1)
            except ZeroDivisionError:
                self.pool_output_height = 0
                self.pool_output_width = 0
                print("INFO: POOL STRIDE OF 0 DISABLES MAXPOOLING; ONLY CONVOLUTION WOULD HAPPEN!")

            # pool_stride has to be a multiple of 2
            if (self.pool_stride != 0 \
                    and self.pool_output_height > 5 and self.pool_output_width > 5 \
                    and self.pool_output_width * self.pool_kernel_width < 1 << 9 \
                    and self.pool_output_width < 1 << 8):

                # WHEN MAXPOOL IS ENABLED, THE OUTPUT SIZE WILL BE SMALLER
                # THERFORE, THE OFM_PACKET_LENGTH HAS TO BE ADJUSTED ACCORDINGLY
                self.ofm_packet_length = self.pool_output_height * self.pool_output_width * self.ofm_slices
            else:
                print("pool_stride: %d", self.pool_stride)
                print("pool_output_height: %d", self.pool_output_height)
                print("pool_output_width: %d", self.pool_output_width)
                print("pool_kernel_width: %d", self.pool_kernel_width)
                self.pool_output_height = 0
                self.pool_output_width = 0
                self.pool_kernel_height = 0
                self.pool_kernel_width = 0
                self.pool_stride = 0
                self.ofm_packet_length = self.ofm_height * self.ofm_width * self.ofm_slices

        derive_attributes()

    def reshape_ifm(self, input_fm, reshape_fm):
        hw_index = 0
        for i in range(0, self.ifm_slices):
            for j in range(0, self.ifm_height * self.ifm_width):
                for k in range(0, C_NUM_OF_ROWS):
                    index = i * (self.ifm_height * self.ifm_width) * C_NUM_OF_ROWS + k * (self.ifm_height * self.ifm_width) + j
                    reshape_fm[hw_index] = input_fm[index]
                    hw_index = hw_index + 1

    def reshape_weights(self, weights_sw, reshape_weights):
        weights_index = 0
        for i in range(0, self.ofm_slices):
            for j in range(0, self.ifm_slices):
                for k in range(0, self.kernel_height * self.kernel_width):
                    for r in range(0, C_NUM_OF_ROWS):
                        for c in range(0, C_NUM_OF_COLS):
                            addr = i * C_NUM_OF_COLS * self.weight_offset + \
                                   c * self.weight_offset + \
                                   j * self.weight_pkt_offset + \
                                   r * self.kernel_height * self.kernel_width + \
                                   k
                            reshape_weights[weights_index] = weights_sw[addr]
                            weights_index = weights_index + 1

    def IP_cmd(self):
        """ Construct convolution command for CNNDataflow IP if the arguments
        inputed are in supported range of Convolution Overlay """

        # The IFM demensions to be in range (6,32)
        if (self.ifm_height < 6 or self.ifm_width < 6 or self.ifm_height > 32 or self.ifm_width > 32):
            print("ERROR: THE IFM VOLUME IS EITHER SMALLER/LARGER THAN SUPPORTED")
            print("TIP: Make sure IFM height and width are in range from 6 to 32")
            return False

        # The IFM depth to be multiples of 8 and are in range (8,1024)
        if (self.ifm_depth <= 512 or self.ifm_depth >= 8):
            if (self.ifm_depth % 8 != 0):
                print("ERROR: THE IFM DEPTH NEEDS TO BE IN MULTIPLES OF 8 IN THE RANGE 8 TO 512")
                return False
        else:
            print("ERROR: THE IFM DEPTH NEEDS TO BE IN MULTIPLES OF 8 IN THE RANGE 8 TO 512")
            return False

        # The Kernel demensions to be in range (1,16)
        if (self.kernel_height < 1 or self.kernel_width < 1 or self.kernel_height > 16 or self.kernel_width > 16):
            print("ERROR: THE KERNEL DIMENSIONS ARE EITHER SMALLER/LARGER THAN SUPPORTED")
            print("TIP: Make sure Kernel height and width are in range from 1 to 16")
            return False

        if (self.stride > 4 or self.stride == 0 or (self.stride != 1 and self.stride % 2 != 0)):
            print("ERROR: THIS STRIDE FOR CONVOLUTION IS NOT RECOMMENDED")
            print("TIP: Make sure stride is either 1, 2 and 4")
            return False

        # The Number of Pad bits to be in range (0,16)
        if (self.pad < 0 or self.pad > 16):
            print("ERROR: THE PADDED BITS ARE EITHER SMALLER/LARGER THAN SUPPORTED")
            print("TIP: Make sure Pad is in range from 0 to 16")
            return False

        # The OFM Channels to be multiples of 8 and are in range (8,1024)
        if (self.ofm_depth <= 512 or self.ofm_depth >= 8):
            if (self.ofm_depth % 8 != 0):
                print("ERROR: THE NUMBER OF CHANNELS NEEDS TO BE IN MULTIPLES OF 8 IN THE RANGE 8 TO 512")
                return False
        else:
            print("ERROR: THE NUMBER OF CHANNELS NEEDS TO BE IN MULTIPLES OF 8 IN THE RANGE 8 TO 512")
            return False

        # The accumulation loopback has 10 cycle delay
        if (self.ofm_height * self.ofm_width < 10):
            print("ERROR: THE OFM VOLUME IS SMALLER THAN SUPPORTED")
            print("TIP: Manage the IFM dimensions, kernel dimensions and other "
                  "arguments such that ofm volume is of moderate size ")
            return False

        # The 2D dimensions are limited by BRAM chosen
        if (self.ifm_height * self.ifm_width > (1 << C_MAX_ADDR_WIDTH) or self.ofm_height * self.ofm_width > (
                1 << C_MAX_ADDR_WIDTH)):
            print("ERROR: THE IFM/OFM PLANE DOES NOT FIT IN THE LINE BUFFER")
            return False

        # The max allowable block read (BTT) by the datamover is limited by
        # 2^23. The num of channels is currently limited by this number
        if (self.ofm_height * self.ofm_width * self.channels * (C_MAX_INPUT_WIDTH / 8) > 1 << 23):
            print("ERROR: THE NUMBER OF CHANNELS IS LARGER THAN THE MAXIMUM "
                  "ALLOWABLE BYTES-TO-TRANSFER(BTT) OF DATAMOVER")
            print("TIP: Decrease the number of channels")
            return False

        while True:
            print("All IP arguments are in supported range")
            cmd_conv = np.array([self.ifm_height, self.ifm_width, self.kernel_height,
                                 self.kernel_width, self.stride, self.pad, self.ofm_height,
                                 self.ofm_width, self.ifm_slices, self.ofm_slices,
                                 self.ofm_fragments, self.ifm_mem_fragments],
                                dtype='uint16')

            print("cmd_conv\n")
            print(cmd_conv.tobytes())
            print(cmd_conv)
            print("\n")
            print("----------------------------------")

            cmd_addr = np.array([self.ifm_baseaddr, self.ifm_packet_length,
                                 self.ifm_depth_offset, self.ifm_height_offset,
                                 self.ofm_baseaddr, self.ofm_packet_length,
                                 self.weights_baseaddr, self.weights_packet_length,
                                 self.weight_depth_offset],
                                dtype='uint32')

            print("cmd_addr\n")
            print(cmd_addr.tobytes())
            print(cmd_addr)
            print("\n")
            print("----------------------------------")

            cmd_mode = np.array([0, 0], dtype='uint16')


            print("cmd_mode\n")
            print(cmd_mode.tobytes())
            print(cmd_mode)
            print("\n")
            print("----------------------------------")

            cmd_pool = np.array([0, 1, self.pool_input_height, self.pool_input_width,
                                 self.pool_kernel_height, self.pool_kernel_width,
                                 self.pool_output_height, self.pool_output_width,
                                 self.pool_stride, 0],
                                dtype='uint16')

            print("cmd_pool\n")
            print(cmd_pool.tobytes())
            print(cmd_pool)
            print("\n")
            print("----------------------------------")

            cmd_rsvd = np.zeros((12,), dtype='uint32')

            print("cmd_rsvd\n")
            print(cmd_rsvd.tobytes())
            print(cmd_rsvd)
            print("\n")
            print("----------------------------------")

            IP_cmd = cmd_conv.tobytes() + \
                     cmd_addr.tobytes() + \
                     cmd_mode.tobytes() + \
                     cmd_pool.tobytes() + \
                     cmd_rsvd.tobytes()

            f = open('./tmp/outputs', 'wb')
            f.write(IP_cmd)
            f.close()

            a1 = np.frombuffer(buffer=IP_cmd, dtype='uint16', count=12, offset=0)
            a2 = np.frombuffer(buffer=IP_cmd, dtype='uint32', count=9, offset=24);
            a3 = np.frombuffer(buffer=IP_cmd, dtype='uint16', count=2, offset=60);
            a4 = np.frombuffer(buffer=IP_cmd, dtype='uint16', count=8, offset=64);
            a5 = np.frombuffer(buffer=IP_cmd, dtype='uint32', count=12, offset=80);


            print("----------------------------------------------\n")

            print(a1)
            print(a2)
            print(a3)
            print(a4)
            print(a5)

            print("-----------------------END--------------------\n")

            break
        return IP_cmd




# ifm_sw = np.array([90, 0, 71, 190, 147, 85, 95, 202, 72, 112, 228, 81, 129, 142, 175, 41, 3, 105, 207, 19,
# 32, 85, 241, 228, 226, 7, 36, 204, 220, 54, 197, 146, 52, 128, 196, 166, 58, 20, 83, 165,
# 185, 221, 13, 194, 177, 79, 43, 177, 95, 82, 120, 229, 233, 29, 236, 56, 177, 175, 79, 139,
# 206, 82, 34, 0, 6, 12, 64, 65, 107, 124, 170, 90, 68, 99, 70, 51, 182, 134, 145, 166,
# 174, 184, 62, 182, 136, 66, 179, 82, 6, 9, 115, 67, 230, 152, 46, 173, 162, 206, 252, 214,
# 56, 12, 195, 16, 79, 247, 192, 140, 204, 191, 152, 76, 226, 105, 53, 73, 166, 73, 157, 254,
# 9, 230, 250, 70, 104, 118, 3, 199, 211, 120, 128, 127, 174, 211, 89, 159, 8, 149, 196, 170,
# 91, 238, 230, 156, 115, 176, 226, 224, 117, 198, 246, 50, 103, 19, 162, 95, 221, 182, 81, 73,
# 104, 234, 27, 20, 222, 20, 89, 215, 180, 24, 178, 65, 183, 63, 227, 74, 89, 117, 158, 114,
# 33, 249, 252, 116, 184, 251, 247, 246, 84, 174, 109, 145, 149, 225, 136, 222, 97, 76, 127, 196,
# 204, 227, 92, 24, 129, 155, 186, 220, 34, 221, 246, 199, 20, 78, 237, 96, 193, 43, 136, 49,
# 83, 154, 76, 152, 53, 34, 219, 126, 172, 164, 151, 46, 87, 107, 19, 48, 82, 44, 28, 84,
# 239, 180, 89, 195, 186, 50, 202, 87, 240, 152, 89, 78, 253, 229, 131, 116, 184, 205, 136, 136,
# 107, 109, 103, 241, 193, 242, 77, 112, 79, 152, 1, 55, 35, 117, 188, 217, 152, 98, 236, 102,
# 201, 85, 38, 90, 30, 162, 220, 227])
# weights_sw = np.array([242, 191, 90, 250, 69, 7, 243, 101, 82, 17, 139, 51, 117, 10, 109, 62, 158, 86, 150, 16,
# 2, 160, 204, 149, 237, 153, 165, 129, 12, 112, 101, 215, 176, 180, 39, 90, 209, 140, 11, 252,
# 209, 161, 60, 112, 225, 77, 116, 144, 112, 83, 12, 57, 213, 91, 238, 140, 63, 219, 244, 9,
# 125, 45, 201, 49, 118, 159, 146, 28, 93, 20, 154, 92, 120, 63, 29, 38, 133, 248, 113, 247,
# 59, 88, 240, 147, 152, 182, 5, 248, 139, 223, 163, 128, 156, 26, 84, 19, 52, 34, 0, 39,
# 245, 19, 253, 140, 65, 217, 1, 240, 65, 49, 70, 146, 77, 174, 231, 71, 51, 195, 168, 202,
# 84, 155, 179, 99, 108, 199, 235, 164, 118, 157, 125, 231, 27, 75, 91, 1, 223, 76, 216, 93,
# 61, 177, 149, 17, 126, 209, 68, 46, 203, 78, 164, 71, 41, 129, 70, 76, 240, 48, 4, 249,
# 66, 203, 56, 118, 75, 228, 122, 130, 79, 164, 37, 168, 88, 162, 173, 51, 8, 146, 224, 107,
# 45, 75, 64, 100, 25, 225, 165, 216, 94, 143, 152, 171, 239, 97, 96, 114, 11, 183, 30, 83,
# 151, 189, 60, 214, 73, 242, 75, 53, 99, 168, 162, 91, 239, 81, 145, 191, 236, 2, 178, 167,
# 45, 54, 245, 159, 157, 151, 36, 236, 106, 80, 73, 158, 184, 107, 90, 58, 7, 22, 127, 33,
# 195, 138, 169, 172, 125, 24, 241, 44, 77, 36, 39, 197, 153, 40, 222, 225, 154, 230, 166, 90,
# 70, 200, 49, 110, 58, 78, 179, 31, 224, 115, 42, 231, 137, 54, 197, 163, 120, 77, 247, 243,
# 198, 211, 66, 138, 22, 165, 231, 144, 25, 71, 188, 177, 124, 253, 40, 177, 66, 91, 224, 223,
# 167, 80, 194, 161, 78, 187, 13, 200, 254, 49, 10, 91, 171, 108, 125, 206, 200, 180, 222, 20,
# 228, 58, 134, 219, 38, 109, 94, 245, 236, 70, 13, 237, 58, 100, 220, 188, 103, 119, 250, 239,
# 119, 7, 144, 247, 165, 12, 141, 109, 133, 202, 28, 193, 159, 107, 31, 23, 84, 240, 61, 33,
# 84, 244, 3, 18, 254, 106, 233, 216, 130, 87, 62, 119, 56, 234, 240, 21, 70, 90, 223, 165,
# 32, 71, 210, 242, 194, 248, 32, 64, 150, 101, 137, 213, 123, 170, 151, 35, 227, 238, 17, 233,
# 152, 92, 114, 15, 46, 112, 148, 171, 107, 238, 243, 67, 156, 209, 235, 43, 126, 124, 195, 12,
# 140, 207, 190, 231, 120, 239, 91, 228, 43, 169, 156, 120, 69, 171, 71, 111, 174, 172, 112, 249,
# 3, 214, 227, 218, 50, 132, 121, 81, 17, 11, 106, 173, 215, 23, 149, 247, 56, 151, 104, 146,
# 37, 142, 190, 16, 8, 163, 2, 186, 191, 120, 86, 219, 210, 50, 10, 31, 2, 29, 70, 176,
# 3, 133, 164, 183, 123, 205, 139, 205, 19, 158, 218, 21, 200, 133, 158, 95, 103, 43, 157, 49,
# 237, 155, 7, 203, 122, 218, 125, 38, 251, 239, 137, 149, 163, 118, 50, 172, 165, 236, 150, 11,
# 121, 34, 45, 36, 125, 133, 195, 254, 37, 77, 236, 8, 159, 253, 56, 168, 170, 59, 67, 46,
# 153, 60, 240, 35, 52, 67, 38, 166, 69, 101, 224, 200, 76, 232, 245, 91, 155, 14, 210, 60,
# 70, 82, 82, 62, 91, 166, 109, 107, 253, 33, 99, 145, 203, 17, 242, 167])

# ifm_sw = np.random.randint(0,255, ifm_width*ifm_height*ifm_depth, dtype=np.int16)
# weights_sw = np.random.randint(0,255, channels*ifm_depth*kernel_height*kernel_width, dtype=np.int16)

ifm_sw = np.array([1] * 288)
weights_sw = np.array([1] * 64)


print("ifm_sw length: " + str(len(ifm_sw)))
print("weights_sw length: " + str(len(weights_sw)))

conv_maxpool = Darius(ifm_height, ifm_width, ifm_depth,
                                   kernel_height, kernel_width, pad, stride,
                                   channels, pool_kernel_height,
                                   pool_kernel_width, pool_stride,
                                   ifm_baseaddr, weights_baseaddr,
                                   ofm_baseaddr)

# for i in range(len(ifm_sw)):
#     print(str(ifm_sw[i]) + ", ", end='', flush=True)
#
# print('\n')
#
# for i in range(len(weights_sw)):
#     print(str(weights_sw[i]) + ", ", end='', flush=True)

ifm_sw_reshape = np.zeros(len(ifm_sw))
weights_sw_reshape = np.zeros(len(weights_sw))

conv_maxpool.reshape_ifm(ifm_sw, ifm_sw_reshape)
conv_maxpool.reshape_weights(weights_sw, weights_sw_reshape)

# # print(ifm_sw)
# print(ifm_sw_reshape)
#
# print("------------------------------------------------")
#
# # print(weights_sw)
# print(weights_sw_reshape)

for i in range(288):
    if i % 20 == 0:
        print('\n')
        print(str(int(ifm_sw_reshape[i])) + ", ", end='', flush=True)
    else:
        print(str(int(ifm_sw_reshape[i])) + ", ", end='', flush=True)

print('\n' + "------------------------------------")

for i in range(64):
    if i % 20 == 0:
        print('\n')
        print(str(int(weights_sw_reshape[i])) + ", ", end='', flush=True)
    else:
        print(str(int(weights_sw_reshape[i])) + ", ", end='', flush=True)


IP_cmd = conv_maxpool.IP_cmd()

print("Command to CNNDataflow IP: \n" + str(IP_cmd))
