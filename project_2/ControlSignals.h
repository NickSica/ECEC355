#ifndef __CTRLSIGS_H__
#define __CTRLSIGS_H__

#include <stdlib.h>
#include <stdint.h>

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
    uint8_t jal;
    uint8_t jalr;
} ControlSignals;

#endif
