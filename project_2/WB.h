#ifndef __WB_H__
#define __WB_H__

#include "ControlSignals.h"
#include "Instruction_Memory.h"

typedef struct WB WB;
typedef struct WB
{
    ControlSignals *ctrl;
    int data;
    int result;
    uint8_t rd;
} WB;





#endif
