
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "tb_des_block.h"

#define MAX_SIM_TIME 250
#define RESET_START 3
#define RESET_END 6
#define VERIF_START_TIME 7
vluint64_t sim_time = 0;
unsigned char frame_cnt = 0;

uint8_t TEST_DATA[3] = {0x55,0x77,0x88};

void dut_reset (Vdeserializer *dut, vluint64_t &sim_time);

int main(int argc, char **argv, char **env)
{
    srand (time(NULL));
    Verilated::commandArgs(argc, argv);
    Vdeserializer *dut = new Vdeserializer;

    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    DesInTx   *tx;
    DesGenOut gen_out = {NULL, 0};

    // Here we create the driver, scoreboard, input and output monitor blocks
    DesInDrv  *drv    = new DesInDrv(dut);
    DesScb    *scb    = new DesScb();
    DesInMon  *inMon  = new DesInMon(dut, scb);
    DesOutMon *outMon = new DesOutMon(dut, scb);
    DesGenInTx *gen   = new DesGenInTx(TEST_DATA[frame_cnt]);


    while (sim_time < MAX_SIM_TIME) {
        dut_reset(dut, sim_time);
        dut->clk ^= 1;
        dut->eval();

        if (dut->clk == 1){

            if(gen_out.frame_finished){
                delete gen;
                frame_cnt = (frame_cnt ==2) ? 0 : frame_cnt +1;
                gen   = new DesGenInTx(TEST_DATA[frame_cnt]);
            }
            if (sim_time >= VERIF_START_TIME) {

                gen_out = gen->not_rnd_DesInTx();
                tx=gen_out.tx;

                drv->drive(tx);

                inMon->monitor();

                outMon->monitor();
            }
        }
        // end of positive edge processing

        m_trace->dump(sim_time);
        sim_time++;
    }

    m_trace->close();
    delete dut;
    delete outMon;
    delete inMon;
    delete scb;
    delete drv;
    exit(EXIT_SUCCESS);


}


void dut_reset (Vdeserializer *dut, vluint64_t &sim_time){
    dut->rst = 0;
    if(sim_time >= RESET_START && sim_time < RESET_END){
        dut->rst = 1;
        dut->valid_in = 0;
        dut->data_in =0;
    }
}

