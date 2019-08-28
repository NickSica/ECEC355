#ifndef __IF_H__
#define __IF_H__

#include "Instruction_Memory.h"

typedef struct IF IF;
typedef struct IF
{
    Addr prevPC;
    Addr PC;
    unsigned instruction;
} IF;





#endif
