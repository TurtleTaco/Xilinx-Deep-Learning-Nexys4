1. Notes for naming conventions and how to compute basic parameters in json
ofm_depth = output_channels
if ifm_height*ifm_width < bram_depth && ofm_height*ofm_width < bram_depth
    ofm_fragments = 1
    mem_fragments = 1 = ifm_mem_fragments
    height_offset = 0

2. How to implement a layer with only convolution without pooling:
when pool kernel size is 1*1 stride is 0, no pooling is performed

3. How is the input parameter location in memory is computed:
largest bias and weight file pair has 1024 biases and 10027008 weights
consider 16 bits size of every number, it is total 20056064 bytes = 0x1320800 bytes
assume the same space is reserved for every single layer parameters
from 0x00000000 add 0x1320800 for every layer follows after
