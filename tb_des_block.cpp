#include "tb_des_block.h"
extern vluint64_t sim_time;
extern unsigned char frame_cnt;

//-------------------------------transaction item generator
DesGenInTx::DesGenInTx(uint8_t test_data){
    this->test_data = test_data;
}

DesInTx* DesGenInTx::rndDesInTx(){
    if(rand()%5 == 0){
        DesInTx *tx = new DesInTx();
        tx->data_in = rand() % 2;
        return tx;

    } else {
        return NULL; 
    }
}

DesGenOut DesGenInTx::not_rnd_DesInTx(){

    if(rand()%3 == 0){
        DesInTx *tx = new DesInTx();
        tx->data_in = (test_data >> bit_cnt) & 0x01;
        bit_cnt++;
           
        return {tx, (bit_cnt == 8)};

    } else {
        return {NULL, false}; 
    }
}

//--------------------------------DRIVER
DesInDrv::DesInDrv(Vdeserializer *dut){
            this->dut = dut;
}

void DesInDrv::drive(DesInTx *tx){

            dut->valid_in = 0;
            dut->data_in =0;

            if(tx != NULL){
                    dut->valid_in = 1;
                    dut->data_in  = tx->data_in;
            }
            delete tx;
 }

 //------------------------------MONITOR IN

DesInMon::DesInMon(Vdeserializer *dut, DesScb *scb){
            this->dut = dut;
            this->scb = scb;
}

void DesInMon::monitor(){

    DesInTx *tx = new DesInTx();
    tx->data_in = dut->data_in;
    tx->valid_in = dut->valid_in;
    scb->writeIn(tx);

}

//---------------------------MONITOR OUT
DesOutMon::DesOutMon(Vdeserializer *dut, DesScb *scb){
            this->dut = dut;
            this->scb = scb;
}

void DesOutMon::monitor(){
            DesOutTx *tx = new DesOutTx();
            tx->data_out = dut->data_out;
            tx->valid_out = dut->valid_out;
            scb->writeOut(tx);
}

//-------------------------SCOREBOARD
void DesScb::writeIn(DesInTx *tx){
            in_q.push_back(tx);
}

void DesScb::writeOut(DesOutTx* tx){
            if(in_q.empty()){
                std::cout <<"Fatal Error in DesScb: empty DesInTx queue" << std::endl;
                exit(1);
            }

            DesInTx* in;
            in = in_q.front();
            in_q.pop_front();



            if(!prev_valid_out && tx->valid_out){
                current_expected_out = expected_out;
                bit_cnt=0;
                expected_out =0;
            }

            if (in->valid_in) {
                expected_out |= (in->data_in << bit_cnt);
                bit_cnt++;
            }
            
            if(tx->valid_out == 1){

                if(tx->data_out != current_expected_out){
                    std::cout << std::endl;
                    std::cout << "data_out INCORRECT" << std::endl;
                    std::cout << "  Expected: " << std::hex << (int)current_expected_out
                              << "  Actual: " << (int)tx->data_out << std::endl;
                    std::cout << "  Simtime: " << std::dec <<  sim_time << std::endl;
                } else {
                    std::cout << std::endl;
                    std::cout << "data_out MATCHES" << std::endl;
                    std::cout << "  Expected: " << std::hex << (int)current_expected_out
                              << "  Actual: " << (int)tx->data_out << std::endl;
                    std::cout << "  Simtime: " << std::dec <<  sim_time << std::endl;
                }
            }

        

            prev_valid_out = tx->valid_out;

            delete in;
            delete tx;
        }