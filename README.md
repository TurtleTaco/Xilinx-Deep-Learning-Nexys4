# Xilinx Deep Learning Core on Nexys4 board

Instructions:
1. Open Vivado 2018.1
2. source dariusDDR.tcl in TCL console (in Vivado_tcl folder)
3. Add constraint file (board specific)
4. Create HDL wrapper
5. Run synthesis, implementation, generate bitstream
6. Export bitstream
7. Launch SDK
8. Create empty application
9. Import source from src directory, there are two darius program v1 and v2 provided, which have the same functionality but different format
10. Set run configuration, check microblaze
11. Connect to port /dev/ttyUSB* accordingly
12. Run
13. Outputs "cycles" takes to compute convolution and maxpooling on sample data
