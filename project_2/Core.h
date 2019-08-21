#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "IF.h"
#include "ID.h"
#include "EX.h"
#include "MEM.h"
#include "WB.h"

#define NUM_REGS 64
#define NUM_BYTES 1024
#define BOOL bool

struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Instruction_Memory *instr_mem;
    uint64_t reg_file[NUM_REGS];
    uint8_t data_mem[NUM_BYTES];
    IF *instr_fetch;
    ID *id;
    EX *ex;
    MEM *mem;
    WB *wb;

    // Simulation function
    bool (*tick)(Core *core);
} Core;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);

#endif
