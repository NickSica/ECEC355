#ifndef __MEM_H__
#define __MEM_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct MEM MEM;
typedef struct MEM
{
    Addr PC;
    ControlSignals *ctrl;
    int result;
    int w_mem_data;
    int r_mem_data;
    uint8_t rd;
} MEM;
#endif
