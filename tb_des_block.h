#ifndef TB_DES_BLOCK_H
#define TB_DES_BLOCK_H

#define DOWNCNT_MAX 8

#include "Vdeserializer.h"
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <deque>
#include <bitset>


class DesInTx;
class DesGenInTx;
class DesInDrv;
class DesInMon;
class DesOutTx;
class DesOutMon;
class DesScb;

// transaction item
class DesInTx {
    public:
        uint8_t data_in;
        uint8_t valid_in;
};

struct DesGenOut
{
    DesInTx* tx;
    bool frame_finished;
};

//generator
class DesGenInTx {
    private:
        uint8_t test_data;
        uint8_t bit_cnt = 0;
    
    public:
        DesGenInTx(uint8_t test_data);
        DesInTx* rndDesInTx(void);
        DesGenOut not_rnd_DesInTx(void);

};

//Driver
class DesInDrv {
    private:
        Vdeserializer *dut;
    public:
        DesInDrv(Vdeserializer *dut);
        void drive(DesInTx *tx);
};

//Monitor IN
class DesInMon {
    private:
        Vdeserializer *dut;
        DesScb *scb;
    public:
        DesInMon(Vdeserializer *dut, DesScb *scb);
        void monitor(void);
};

// Transaction OUT
class DesOutTx {
    public:
        uint8_t data_out;
        uint8_t valid_out;
};

//Monitor OUT
class DesOutMon {
    private:
        Vdeserializer *dut;
        DesScb *scb;
    public:
        DesOutMon(Vdeserializer *dut, DesScb *scb);
        void monitor(void);
};

//Scoreboard
class DesScb {
    private:
        std::deque<DesInTx*> in_q;
        uint8_t bit_cnt=0;
        uint8_t expected_out = 0;
        uint8_t current_expected_out = 0;
        uint8_t prev_valid_out=0;

    public:
        // Input interface monitor port
        void writeIn(DesInTx *tx);
        void writeOut(DesOutTx* tx);
};


#endif

