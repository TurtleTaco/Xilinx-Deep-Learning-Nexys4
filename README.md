# Xilinx Deep Learning Core on Nexys4 board

Instructions:
1. Open Vivado 2018.1
2. source dariusDDR.tcl in TCL console
3. Add constraint file
4. Create HDL wrapper
5. Run synthesis, implementation, generate bitstream
6. Export bitstream
7. Launch SDK
8. Create empty application
9. Import source from src directory
10. Set run configuration, check microblaze
11. Connect to port /dev/ttyUSB* accordingly
12. Run
13. Outputs cycles takes to compute convolution and maxpooling on sample data

test bench for darius uses below fake instruction <br />
 <br />
general parameters: <br />
input: w * h * c = 6 * 6 * 8 <br />
weights: w * h * c = 1 * 1 * 8 / stride: 1 / pad: 0 <br />
output: 6 * 6 * 8 (conv1) <br />
pool: w * h = 2 * 2 / stride: 2 <br />
output: 3 * 3 * 8 <br />
 <br />
1. input image: 288 * 16 bits in size <br />
ifm_sw = np.array([1] * 288) <br />
2. parameter: 64 * 16 bits in size <br />
weights_sw = np.array([1] * 64) <br />
3. output: software simulation 72 * 16 bits in size <br />
[[[[8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.]] <br />
  [[8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.]] <br />
  [[8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.] <br />
   [8. 8. 8. 8. 8. 8. 8. 8.]]]]
