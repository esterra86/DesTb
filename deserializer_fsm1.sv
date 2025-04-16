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

typedef enum logic [1:0] 
{  
    RESET  = 2'b00,
    WAIT   = 2'b01,
    ACTIVE = 2'b10,
    READY  = 2'b11
} t_State;

t_State state, nxt_state;
logic shift_en, cnt_rst;
logic [$clog2(DESERIALIZER_WD):0] downcnt;

always_ff @( posedge clk ) begin 
    if(rst) begin
        state <= RESET;
    end else begin
        state <= nxt_state;        
    end
end

always_ff @( posedge clk ) begin
    if(rst) begin
        data_out <= '0;
    end else begin
        if (shift_en) begin
           data_out <= {data_in, data_out[DESERIALIZER_WD-1:1]};
        end
    end
end

always_ff @( posedge clk ) begin
    if(cnt_rst) begin
        downcnt <= DESERIALIZER_WD;
    end else begin
        if (shift_en) begin
           downcnt <= downcnt-1;
        end
    end
end

always_comb begin : fsm_nxt_state
    case (state)
        RESET:  nxt_state = WAIT;
        WAIT:   nxt_state = valid_in ? ACTIVE : WAIT;
        ACTIVE: begin
            if(downcnt == 0) begin
               nxt_state = READY; 
            end else begin
                nxt_state = valid_in ? ACTIVE : WAIT;
            end
        end
        READY:  nxt_state = valid_in ? ACTIVE : READY;
    endcase
end
    
always_comb begin : fsm_ouputs
    case (state)
        RESET:  {valid_out, shift_en, cnt_rst} = 3'b001;
        WAIT:   {valid_out, shift_en, cnt_rst} = 3'b000;
        ACTIVE: {valid_out, shift_en, cnt_rst} = 3'b010;
        READY:  {valid_out, shift_en, cnt_rst} = 3'b101;
    endcase
end
endmodule