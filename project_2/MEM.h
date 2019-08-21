#ifndef __MEM_H__
#define __MEM_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct MEM MEM;
typedef struct MEM
{
    ControlSignals *ctrl;
    int result;
    int mem_data;
    uint8_t rd;
} MEM;
#endif
