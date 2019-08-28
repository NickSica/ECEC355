#ifndef __MEM_H__
#define __MEM_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct MEM MEM;
typedef struct MEM
{
    Addr PC;
    ControlSignals *ctrl;
    int64_t result;
    int64_t w_mem_data;
    int64_t r_mem_data;
    uint8_t rd;
} MEM;
#endif
