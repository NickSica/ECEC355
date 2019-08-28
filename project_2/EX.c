#include "EX.h"

uint8_t forwardUnit(uint8_t rs_1, uint8_t rs_2, uint8_t mem_rd, uint8_t wb_rd, uint8_t mem_w_reg, uint8_t wb_w_reg)
{
    uint8_t fwd = 0;
    if(mem_w_reg)
    {
	if(rs_1 == mem_rd)
	    fwd |= (0b10 << 2);
	if(rs_2 == mem_rd)
	    fwd |= 0b10;
    }

    if(wb_w_reg)
    {
	if(rs_1 == wb_rd)
	    fwd |= (0b01 << 2);
	if(rs_2 == wb_rd)
	    fwd |= 0b01;
    }	
    return fwd;
}

void alu(int64_t r_data_1, int64_t r_data_2, uint8_t ctrl_signal, int64_t *result, uint8_t *zero)
{
    *zero = (r_data_1 == r_data_2);
    switch(ctrl_signal)
    {
    case 0b0000:
        *result = r_data_1 & r_data_2;
        break;
    case 0b0001:
        *result = r_data_1 | r_data_2;
        break;
    case 0b0010:
        *result = r_data_1 + r_data_2;
        break;
    case 0b0011:
        *result = r_data_1 << r_data_2;
        break;
    case 0b0100:
        *result = r_data_1 >> r_data_2;
        break;
    case 0b0101:
        *result = r_data_1 ^ r_data_2;
        break;
    case 0b0110:
        *result = r_data_1 - r_data_2;
        break;
    case 0b0111:
        *result = r_data_1 < r_data_2 ? 1 : 0;
        break;
    }
}

uint8_t aluControl(uint8_t aluOp, uint8_t funct3, uint8_t funct7)
{
    if(aluOp == 0)
    {
        return 0b0010;
    }
    else if(aluOp == 0b01)
    {
        if(funct3 == 0b000 || funct3 == 0b001)     // BEQ/BNE
        {
            return 0b0110;
        }                     
        else if(funct3 == 0b100 || funct3 == 0b101)     // BLT/BGT
        {
            return 0b0111;
        }
    }
    else if(aluOp == 0b10)
    {
        switch(funct3)
        {
        case 0b000:         
            if(funct7 == 0b0000000)                 // ADD
            {
                return 0b0010;
            }
            else if(funct7 == 0b0100000)            // SUB
            {
                return 0b0110;
            }
            break;
        case 0b001:                                 // SLL
            return 0b0011;
        case 0b010:                                 // SLT(needed for bgt and blt)
            return 0b0111;
        case 0b100:                                 // XOR
            return 0b0101;
        case 0b101:                                 // SRL
            return 0b0100;
        case 0b110:                                 // OR
            return 0b0001;
        case 0b111:                                 // AND
            return 0b0000;
        }
    }
}





