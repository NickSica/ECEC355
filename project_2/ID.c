#include "ID.h"

uint8_t hazardDetection(unsigned int instruction, uint8_t ex_rd, ControlSignals *ctrl)
{
    if(ctrl->memRead)  // Load-Use Hazard
    {
	if((instruction & (0b11111 << 6)) == ex_rd)
	{
	    return 0b000;
	}
    }
    return 0b111;
}

void control(ControlSignals *ctrl_signals, unsigned opcode, uint8_t funct3)
{
    if(opcode == 0b0110011)             // R-Type
    {
        ctrl_signals->regWrite = 1;
        ctrl_signals->aluSrc = 0;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b10;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 0;
    }   
    else if(opcode == 0b0010011)        // I-Type
    {
        ctrl_signals->regWrite = 1;
        ctrl_signals->aluSrc = 1;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b10;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 0;
    }   
    else if(opcode == 0b0000011)       // LD
    {
        ctrl_signals->regWrite = 1;
        ctrl_signals->aluSrc = 1;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b00;
        ctrl_signals->memToReg = 1;
        ctrl_signals->memRead = 1;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 0;
    }   
    else if(opcode == 0b1100111)      // JALR
    {
        ctrl_signals->regWrite = 1;
        ctrl_signals->aluSrc = 1;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b00;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 1;
    }   
    else if(opcode == 0b0100011)    // SD
    {
        ctrl_signals->regWrite = 0;
        ctrl_signals->aluSrc = 1;
        ctrl_signals->memWrite = 1;
        ctrl_signals->aluOp = 0b00;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 0;
    }   
    else if(opcode == 0b1100011)    // B-Type
    {
        ctrl_signals->regWrite = 0;
        ctrl_signals->aluSrc = 0;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b01;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->jal = 0;
        ctrl_signals->jalr = 0;
        switch(funct3)
        {
        case 0b000:
            ctrl_signals->beq = 1;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 0;
            break;
        case 0b001:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 1;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 0;
            break;
        case 0b100:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 1;
            ctrl_signals->bge = 0;
            break;
        case 0b101:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 1;
            break;
        }
    }   
    else if(opcode == 0b1101111)    // JAL
    {
        ctrl_signals->regWrite = 1;
        ctrl_signals->aluSrc = 0;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b00;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->beq = 0;
        ctrl_signals->jal = 1;
        ctrl_signals->jalr = 0;
    }   
}

int buildImm(unsigned instr)
{
    int imm = 0;
    unsigned opcode = (instr & 0b1111111);
    if(opcode == 0b0010011 || opcode == 0b0000011 || opcode == 0b1100111)   // I-Type
    {
        imm |= ((instr & (0b111111111111 << 20)) >> 20);
        if(imm & 0x800)
        {
            imm |= 0xFFFFF000;
        }
    }
    else if(opcode == 0b0100011)        //S-Type
    {
        imm |= ((instr & (0b11111 << 7)) >> 7);
        imm |= ((instr & (0b1111111 << 25)) >> 20);
        if(imm & 0x800)
        {
            imm |= 0xFFFFF000;
        }
    }
    else if(opcode == 0b1100011)        // B-Type
    {
        imm |= ((instr & (0b1 << 7)) << 4);
        imm |= ((instr & (0b1111 << 8)) >> 7); 
        imm |= ((instr & (0b111111 << 25)) >> 20);
        imm |= ((instr & (0b1 << 31)) >> 19);
        if(imm & 0x1000)
        {
            imm |= 0xFFFFF000;
        }
    }
    else if(opcode == 0b1101111)        // J-Type
    {
        imm |= (instr & 0b11111111000000000000); 
        imm |= ((instr & (0b100000000000 << 9)) >> 9); 
        imm |= ((instr & (0b11111111110 << 20)) >> 20); 
        imm |= ((instr & (0b100000000000000000000 << 11)) >> 11);
        if(imm & 0x100000)
        {
            imm |= 0xFFF00000;
        }
    }

    return imm;
}
