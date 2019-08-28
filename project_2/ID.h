#ifndef __ID_H__
#define __ID_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct ID ID;
typedef struct ID
{
    Addr PC;
    unsigned instruction;
    ControlSignals *ctrl;
    int64_t read_data_1;
    int64_t read_data_2;
    int16_t imm;
    uint8_t rd;
    uint8_t rs_1;
    uint8_t rs_2;
    uint8_t funct7;
} ID;

uint8_t hazardDetection(unsigned int instruction, uint8_t ex_rd, ControlSignals *ctrl);
void control(ControlSignals *ctrl_signals, unsigned opcode, uint8_t funct3);
int buildImm(unsigned instr);

#endif
