#ifndef __ID_H__
#define __ID_H__

#include "Instruction_Memory.h"
#include "ControlSignals.h"

typedef struct ID ID;
typedef struct ID
{
    Addr PC;
    unsigned instruction;
    ControlSignals *ctrl;
    int read_data_1;
    int read_data_2;
    int imm;
    uint8_t rd;
    uint8_t rs_1;
    uint8_t rs_2;
} ID;

void control(ControlSignals *ctrl_signals, unsigned opcode, uint8_t funct3);
int buildImm(unsigned instr);

#endif
