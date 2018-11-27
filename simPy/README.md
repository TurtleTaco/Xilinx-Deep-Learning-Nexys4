1. Notes for naming conventions and how to compute basic parameters in json</br>
ofm_depth = output_channels</br>
if ifm_height*ifm_width < bram_depth && ofm_height*ofm_width < bram_depth</br>
    ofm_fragments = 1</br>
    mem_fragments = 1 = ifm_mem_fragments</br>
    height_offset = 0</br>

2. How to implement a layer with only convolution without pooling:</br>
when pool kernel size is 1*1 stride is 0, no pooling is performed</br>

3. How is the input parameter location in memory is computed:</br>
largest bias and weight file pair has 1024 biases and 10027008 weights</br>
consider 16 bits size of every number, it is total 20056064 bytes = 0x1320800 bytes</br>
assume the same space is reserved for every single layer parameters</br>
from 0x00000000 add 0x1320800 for every layer follows after</br>
