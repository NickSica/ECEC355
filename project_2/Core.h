#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_REGS 64
#define NUM_BYTES 1024
#define BOOL bool

struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC; // Keep track of program counter
    Instruction_Memory *instr_mem;
    uint64_t reg_file[NUM_REGS];
    uint8_t data_mem[NUM_BYTES];
    
    // Simulation function
    bool (*tick)(Core *core);
} Core;

typedef struct ControlSignals ControlSignals;
typedef struct ControlSignals
{
    uint8_t regWrite;
    uint8_t aluSrc;
    uint8_t memWrite;
    uint8_t aluOp;
    uint8_t memToReg;
    uint8_t memRead;
    uint8_t beq;
    uint8_t bne;
    uint8_t blt;
    uint8_t bge;
    uint8_t jump;
} ControlSignals;

typedef struct IFtoID;
typedef struct IFtoID
{
    Addr branchPC;
    Instruction_Memory *instr_mem;
} IFtoID;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);
void alu(int r_data_1, int r_data_2, uint8_t ctrl_signal, int *result, uint8_t *zero);
uint8_t aluControl(uint8_t aluOp, uint8_t funct3, uint8_t funct7);
void control(ControlSignals *ctrl_signals, unsigned opcode, uint8_t funct3);
int buildImm(unsigned instr);

#endif
