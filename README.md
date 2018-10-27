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

test bench for darius uses below fake instruction
input image: 288 * 16 bits in size
ifm_sw = np.array([1] * 288)
parameter: 64 * 16 bits in size
weights_sw = np.array([1] * 64)
output: software simulation 288 * 16 bits in size
[[[[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]
  [[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]
  [[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]
  [[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]
  [[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]
  [[64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]
   [64. 64. 64. 64. 64. 64. 64. 64.]]]]
