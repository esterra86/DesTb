module deserializer
#(
    parameter DESERIALIZER_WD = 8
)
(
    input logic clk,
    input logic rst,
    input logic valid_in,
    input logic data_in,

    output logic [DESERIALIZER_WD-1:0] data_out,
    output logic valid_out 
);



logic cnt_rst;
logic [$clog2(DESERIALIZER_WD):0] downcnt;


always_ff @( posedge clk ) begin
    if(rst) begin
        data_out <= '0;
        downcnt <= DESERIALIZER_WD;
    end else begin
        if (valid_in) begin
           data_out <= {data_in, data_out[DESERIALIZER_WD-1:1]};
           downcnt <= (cnt_rst) ? DESERIALIZER_WD-1: downcnt-1;
        end
        
    end
end

assign valid_out = (downcnt==0);
assign cnt_rst = downcnt==0;

endmodule