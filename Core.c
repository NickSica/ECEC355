#include "Core.h"
#include "Registers.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->PC = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    memset(core->reg_file, 0, NUM_REGS*sizeof(core->reg_file[0]));
    memset(core->data_mem, 0, NUM_BYTES*sizeof(core->data_mem[0]));
    return core;
}

bool tickFunc(Core *core)
{
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    
    // (Step 2) Pass into control, register file, immediate and ALU Control
    ControlSignals *ctrl_signals = (ControlSignals *) malloc(sizeof(ControlSignals));
    control(ctrl_signals, (instruction & 0b1111111), (instruction & (0b111 << 12)));
    
    uint8_t rs_1 = (instruction & (0b11111 << 15)) >> 15;
    uint8_t rs_2 = (instruction & (0b11111 << 20)) >> 20;
    uint8_t rd = (instruction & (0b11111 << 7)) >> 7;
    int read_data_1 = core->reg_file[rs_1]; 
    int read_data_2 = core->reg_file[rs_2]; 
    
    int imm = buildImm(instruction);

    // (Step 3) Pass into mux and from there into ALU
    int result = 0;
    int operand_2;
    uint8_t zero = 0;
    uint8_t alu_ctrl;

    if(ctrl_signals->aluSrc)
    {
        operand_2 = imm;
    }
    else
    {
        operand_2 = read_data_2;
    }

    alu_ctrl = aluControl(ctrl_signals->aluOp, ((instruction & (0b111 << 12)) >> 12), ((instruction & (0b1111111 << 25)) >> 25));
    alu(read_data_1, operand_2, alu_ctrl, &result, &zero);


    // (Step 4) Memory access, memory access, and register file writeback
    int ram_data;
    int w_data;

    if(ctrl_signals->memWrite)
    {
        core->data_mem[result] = read_data_2;
    }
    if(ctrl_signals->memRead)
    {
        ram_data = core->data_mem[result];
    }

    if(ctrl_signals->memToReg)
    {
        w_data = ram_data;
    }
    else
    {
        w_data = result;
    }

    if(rd != 0 && ctrl_signals->regWrite)
    {
        core->reg_file[rd] = w_data;
    }

    

    // (Step 5) Set PC to the correct value
    unsigned branch_PC = core->PC + (imm << 1);
    unsigned jump_PC = core->PC + (imm);

    if((ctrl_signals->beq && zero) || (ctrl_signals->bne && ~zero) || (ctrl_signals->blt && result)|| (ctrl_signals->bge && (~result || zero)))
    {
        core->PC = branch_PC;
    }
    else if(ctrl_signals->jump)
    {
        core->PC = jump_PC;
    }
    else
    {
        core->PC += 4;
    }
    

    printf("\nInstruction: %u\n", instruction);
    printf("%u     %u     %u     %d      %d\n", rd, rs_1, rs_2, imm, result);
    int i;
    for(i = 0; i < NUM_REGS; i++)
    {
        printf("%s: ", REGISTER_NAME[i]);
        printf("%lu\n", core->reg_file[i]);
    }

    free(ctrl_signals);
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }
    return true;
}

void alu(int r_data_1, int r_data_2, uint8_t ctrl_signal, int *result, uint8_t *zero)
{
    printf("%u", ctrl_signal);
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
    printf("\n%u     %u     %u\n", aluOp, funct3, funct7);
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
        case 0:         
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
        ctrl_signals->jump = 0;
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
        ctrl_signals->jump = 0;
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
        ctrl_signals->jump = 0;
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
        ctrl_signals->jump = 1;
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
        ctrl_signals->jump = 0;
    }   
    else if(opcode == 0b1100011)    // B-Type
    {
        ctrl_signals->regWrite = 0;
        ctrl_signals->aluSrc = 0;
        ctrl_signals->memWrite = 0;
        ctrl_signals->aluOp = 0b01;
        ctrl_signals->memToReg = 0;
        ctrl_signals->memRead = 0;
        ctrl_signals->jump = 0;
        switch(funct3)
        {
        case 0b000:
            ctrl_signals->beq = 1;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 0;
        case 0b001:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 1;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 0;
        case 0b100:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 1;
            ctrl_signals->bge = 0;
        case 0b101:
            ctrl_signals->beq = 0;
            ctrl_signals->bne = 0;
            ctrl_signals->blt = 0;
            ctrl_signals->bge = 1;
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
        ctrl_signals->jump = 1;
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

/* Pipelined Version Whoops
void registerFileRW(uint64_t *reg_file[NUM_REGS], uint8_t rs_1, uint8_t rs_2, uint8_t rd, uint8_t reg_write, int *w_data, int *read_data_1, int *read_data_2)
{
    if(rd != 0 && reg_write)
    {
        *reg_file[rd] = w_data;
    }

    if(rs_1 == rd)
    {
        *read_data_1 = w_data;
    }
    else
    {
        *read_data_1 = *reg_file[rs_1]; 
    }

    if(rs_2 == rd)
    {
        *read_data_2 = w_data;
    }
    else
    {
        *read_data_2 = *reg_file[rs_2]; 
    }
}
*/