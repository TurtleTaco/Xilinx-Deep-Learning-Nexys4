`timescale 1 ns/ 1ps

module mpi_eth_stimulate
                    (
                     input clk,
                     //output reg [63:0] stream_out_DATA,
                     //output reg [7:0] stream_out_KEEP, 
                     //output reg stream_out_LAST,
                     //output reg stream_out_VALID,
                     output [63:0] stream_out_DATA,
                     output [7:0] stream_out_KEEP, 
                     output stream_out_LAST,
                     output stream_out_VALID,
                     input stream_out_READY,
                     input [63:0] stream_in_DATA,
                     input [7:0] stream_in_KEEP, 
                     input stream_in_LAST,
                     input stream_in_VALID,
                     output reg stream_in_READY,
                     input aux_resetn
                    );

    `include "utility.sv"
    `include "mpi.sv"

    parameter [47:0] MAC_ADDR_FPGA = 48'hfa163e55ca02; 
    parameter [47:0] MAC_ADDR_STIM = 48'hfa163e55ca01; 


    reg [15:0] dst_rank;
    reg [7:0] src_rank;
    reg [31:0] size;
    reg [63:0] trans_num;
    reg [63:0] data_in_reverse;
    
    mpi_interface stream(.clk(clk),
                      .stream_out_data(stream_out_DATA),
                      .stream_out_keep(stream_out_KEEP),
                      .stream_out_last(stream_out_LAST),
                      .stream_out_valid(stream_out_VALID),
                      .stream_out_ready(stream_out_READY),
                      .stream_in_data(stream_in_DATA),
                      .stream_in_keep(stream_in_KEEP),
                      .stream_in_last(stream_in_LAST),
                      .stream_in_valid(stream_in_VALID),
                      .stream_in_ready(stream_in_READY)                      
    
    );
    /*
    ethernet_interface stream
                (.clk(clk),
                 .stream_out_data(stream_out_DATA),
                 .stream_out_keep(stream_out_KEEP),
                 .stream_out_last(stream_out_LAST),
                 .stream_out_valid(stream_out_VALID),
                 .stream_out_ready(stream_out_READY),
    input [63:0] stream_in_data,
    input [7 :0] stream_in_keep,
    input stream_in_last,
    input stream_in_valid,
    input stream_in_ready  
    );
*/


     
    initial begin
        //stream = new();
        stream_in_READY = 1;
        #50
        trans_num = 64'd10;
        dst_rank = 16'd1;
        src_rank = 8'd0;
        
        $display("prepare 1st transaction for parameter_mem_info_float (weights)");
        size = 16'd2; // number of 32 bits data
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );

        // higher 32 bits: offset in mem to dma_in
        // lower  32 bits: size (bytes) in mem to dma_in
        // example: dma_in at offset 0, prepare to dma_in 64 * 16bits = 1024 bits = 128 bytes 
        // data: (hex) 00000000 00000080                 
        stream.write(
                    .data(64'h80),
                    .keep(8'hff),
                    .last(1'b1)
                    );
       
        $display("before wait for done");
        
        
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");
        


        $display("prepare 2nd transaction for parameters (weights)");
        size = 16'd128;
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );
        $display("second before stream write");
        // send all weights 128 bytes = 64 * 16 bits, every 16 bit weight is 1
        for(int i = 0; i < 16; i++) begin
            if (i == 15)
                stream.write(
                    .data(64'h0001000100010001),
                    .keep(8'hff),
                    .last(1'b1)
                    );  
            else
                stream.write(
                    .data(64'h0001000100010001),
                    .keep(8'hff),
                    .last(1'b0)
                    );
        end
        
        $display("before wait for done");
                    
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");
        
        $display("prepare 3rd transaction for data_mem_info_float");
        size = 16'd4; // modify
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );

        // setup mem offset, size for input image dma_in and output feature map dma_out, payload 4 floats
        // 1st flit: higher 32 bits: offset in mem to dma_in
        //           lower  32 bits: size (bytes) in mem to dma_in
        // 2nd flit: higher 32 bits: offset in mem to dma_out
        //           lower  32 bits: size (bytes) in mem to dma_out
        // example: dma_in at offset 0x00001000, prepare to dma_in 576 bytes as input image
        //          payload: (hex) 0x00001000 00000240
        // example: dma_out at offset 0x00005000, prepare to dma_out 576 bytes as output feature map
        //          payload: (hex) 0x00005000 00000240

        stream.write(
                    .data(64'h0000100000000240),
                    .keep(8'hff),
                    .last(1'b0)
                    );
        stream.write(
                    .data(64'h0000500000000240),
                    .keep(8'hff),
                    .last(1'b1)x
                    );
        
        $display("before wait for done");
                    
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");
        
        $display("prepare 4th transaction darius info");
        // {num_commands, command, batch_size, num_ranks} 
        size = 16'd35; // modify
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );
        // send 140 bytes = 4 bytes num_commands + 128 bytes command 
        // + 4 * 2 bytes (floats) batch_size and num_ranks = total 17.5 flits
        // 140 bytes = 140 * 8 bits = 1120 bits, size = 1120/32 = 35
        // 8 bytes per flit
        stream.write(
                    .data(64'h0006000600010001),
                    .keep(8'hff),
                    .last(1'b0)
                    );        
        stream.write(
                    .data(64'h0001000000060006),
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0001000100010001),
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h8000010000000024), //2147483904, 36
                    .keep(8'hff),
                    .last(1'b0)
                    );        
        stream.write(
                    .data(64'h0000012000000000), // 288, 0
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h8000C00000000024), // 2147532800, 36
                    .keep(8'hff),
                    .last(1'b0)
                    );                    
        stream.write(
                    .data(64'h8000200000000008), // 2147491840, 8
                    .keep(8'hff),
                    .last(1'b0)
                    );        
        stream.write(
                    .data(64'h0000004000000001), // 64 (32 bits), 0 (16 bits), 1 (16 bits)
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0006000600000000), // 6, 6, 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    );                    
        stream.write(
                    .data(64'h0000000000000000), // 0, 0, 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    );        
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    );
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b0)
                    ); 
        stream.write(
                    .data(64'h0000000000000000), // 0, 0
                    .keep(8'hff),
                    .last(1'b1)
                    ); 
        $display("before wait for done");                                
                                
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");   



        $display("prepare 5th transaction for accumulated count of cycle");
        size = 16'd1; // modify
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );

        stream.write(
                    .data(32'h00000000),
                    .keep(8'hff),
                    .last(1'b1)
                    );
        
        $display("before wait for done");
                    
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");


        $display("prepare 6th transaction for data (image)");
        size = 16'd144; // modify
        stream.mpi_send_header(
                              .dst_rank(dst_rank),
                              .src_rank(src_rank),
                              .size(size),
                              .mac_addr_dst(MAC_ADDR_FPGA),
                              .mac_addr_src(MAC_ADDR_STIM),
                              .ip_addr_dst(32'd0),
                              .ip_addr_src(32'd0)
                              );

        for(int i = 0; i < 72; i++) begin
            if (i == 71)
                stream.write(
                    .data(64'h0001000100010001),
                    .keep(8'hff),
                    .last(1'b1)
                    );  
            else
                stream.write(
                    .data(64'h0001000100010001),
                    .keep(8'hff),
                    .last(1'b0)
                    );
        end
        
        $display("before wait for done");
                    
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");

        $display("prepare 7th transaction to receive cycle count");
        size = 16'd1; // modify
        stream.mpi_recv(
                    .dst_rank(src_rank),
                    .src_rank(dst_rank), 
                    .size(size),
                    .mac_addr_dst(MAC_ADDR_STIM),
                    .mac_addr_src(MAC_ADDR_FPGA),
                    .ip_addr_dst(32'd0),
                    .ip_addr_src(32'd0)
            );
        
        $display("before wait for done");
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");

        display("prepare 8th transaction to receive output");
        size = 16'd144; // modify
        stream.mpi_recv(
                    .dst_rank(src_rank),
                    .src_rank(dst_rank), 
                    .size(size),
                    .mac_addr_dst(MAC_ADDR_STIM),
                    .mac_addr_src(MAC_ADDR_FPGA),
                    .ip_addr_dst(32'd0),
                    .ip_addr_src(32'd0)
            );
        
        $display("before wait for done");
                    
        stream.mpi_send_wait_for_done(
                                     .dst_rank(dst_rank),
                                     .src_rank(src_rank),
                                     .mac_addr_dst(MAC_ADDR_FPGA),
                                     .mac_addr_src(MAC_ADDR_STIM),
                                     .ip_addr_dst(32'd0),
                                     .ip_addr_src(32'd0)
                                     );
        $display("after wait for done");                                                                                                                                   
    end

endmodule
