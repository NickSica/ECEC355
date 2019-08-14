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
    
} ID;





#endif
