#ifndef __EX_H__
#define __EX_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct EX EX;
typedef struct EX
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
    int64_t w_mem_data;
    int result;
    uint8_t funct7;
    uint8_t funct3;
} EX;

uint8_t forwardUnit(uint8_t rs_1, uint8_t rs_2, uint8_t mem_rd, uint8_t wb_rd, uint8_t mem_w_reg, uint8_t wb_w_reg);
void alu(int r_data_1, int r_data_2, uint8_t ctrl_signal, int *result, uint8_t *zero);
uint8_t aluControl(uint8_t aluOp, uint8_t funct3, uint8_t funct7);

#endif
