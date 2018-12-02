tb_mpi_eth.sv <br />
Test bench for darius uses below fake instruction <br />
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

make simFPGA ARGS=1 LOGICALFILE=./telepathy/middlewareInput/conf0/mpiLogical.xml MAPFILE=./telepathy/middlewareInput/conf0/mpiMap.xml SIM_DIR=./telepathy/sim PROJECTNAME=simTest
