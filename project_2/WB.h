#ifndef __WB_H__
#define __WB_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct WB WB;
typedef struct WB
{
    Addr PC;
    ControlSignals *ctrl;
    int64_t r_mem_data;
    int64_t result;
    uint8_t rd;
} WB;

#endif
