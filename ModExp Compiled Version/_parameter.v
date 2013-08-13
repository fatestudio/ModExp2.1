// common header file
`ifndef __parameter_
`define __parameter_

`define DATA_WIDTH 64 
`define ADDR_WIDTH 6
`define TOTAL_ADDR (2 ** `ADDR_WIDTH)
`define TOTAL_BITS `DATA_WIDTH * `TOTAL_ADDR

`define DATA_WIDTH32 32
`define ADDR_WIDTH32 7
`define TOTAL_ADDR32 (2 ** `ADDR_WIDTH32)

`define DATA_WIDTH16 16
`define ADDR_WIDTH16 8
`define TOTAL_ADDR16 (2 ** `ADDR_WIDTH16)

`endif