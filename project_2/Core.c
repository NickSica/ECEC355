#include "Core.h"
#include "Registers.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    memset(core->reg_file, 0, NUM_REGS*sizeof(core->reg_file[0]));
    memset(core->data_mem, 0, NUM_BYTES*sizeof(core->data_mem[0]));

    core->instr_fetch = malloc(sizeof(IF));
    core->instr_fetch->PC = 0;
    core->id = malloc(sizeof(ID));
    core->ex = malloc(sizeof(EX));
    core->mem = malloc(sizeof(MEM));
    core->wb = malloc(sizeof(WB));

    /* UNCOMMENT  TO SET DEFAULT VALUES FOR matrix  */ /* 
    core->reg_file[1] = core->instr_mem->last->addr;
    core->reg_file[2] = NUM_BYTES - 8;
    core->reg_file[10] = 0;
    core->reg_file[11] = 128;
    */

    for(int i = 0; i < 16; i++)
	core->data_mem[i*8] = i;

    return core;
}

bool tickFunc(Core *core)
{
    // Pass values along pipeline
    free(core->wb->ctrl);
    core->wb->ctrl = core->mem->ctrl;
    core->wb->result = core->mem->result;
    core->wb->rd = core->mem->rd;
    core->mem->ctrl = core->ex->ctrl;
    core->mem->rd = core->ex->rd;
    core->ex->rd = (core->instr_fetch->instruction & (0b11111 << 7)) >> 7;
    core->ex->rs_1 = (core->instr_fetch->instruction & (0b11111 << 15)) >> 15;
    core->ex->rs_2 = (core->instr_fetch->instruction & (0b11111 << 20)) >> 20;
    core->instr_fetch->instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    core->id->instruction = core->instr_fetch->instruction;
    core->id->PC = core->instr_fetch->PC;

    // Instructions are done from WB-ID so extra registers don't need to be allocated
    // WB
    int w_data;
    if(core->wb->ctrl->memToReg)
	w_data = core->wb->data;
    else if(core->wb->ctrl->jal || core->wb->ctrl->jalr)
	w_data = core->PC + 4;
    else
	w_data = core->wb->result;

    if(core->wb->rd != 0 && core->wb->ctrl->regWrite)
	core->reg_file[core->wb->rd] = w_data;
    
    
    // MEM
    // Maybe put this in below if statement? Was in non-pipelined version... core->data_mem[result] = 0;
    if(core->mem->ctrl->memWrite)
	for(int i = 0; i < 8; i++)
	    core->data_mem[core->mem->result + i] = (core->mem->mem_data & (0xFF << (i * 8)));

    if(core->mem->ctrl->memRead)
    {
	core->wb->data = 0;
	for(int i = 0; i < 8; i++)
	    core->wb->data |= (core->data_mem[core->mem->result + i] << (i * 8));
    }

    
    // EX
    uint8_t fwd = forwardUnit(core->ex->rs_1, core->ex->rs_2, core->mem->rd, core->wb->rd, core->mem->ctrl->regWrite, core->wb->ctrl->regWrite);
    
    uint8_t funct7 = ((core->ex->instruction & (0b1111111 << 25)) >> 25);
    uint8_t zero = 0;
    uint8_t alu_ctrl;
    int operand_1;
    int operand_2;
    if(((fwd & (0b11 << 2)) >> 2) == 0b00)
	operand_1 = core->ex->read_data_1;
    else if(((fwd & (0b11 << 2)) >> 2) == 0b01)
	operand_1 = w_data;
    else if(((fwd & (0b11 << 2)) >> 2) == 0b10)
	operand_1 = core->mem->result;

    if((fwd & 0b11) == 0b00)
	operand_2 = core->ex->read_data_2;
    else if((fwd & 0b11) == 0b01)
	operand_2 = w_data;
    else if((fwd & 0b11) == 0b10)
	operand_2 = core->mem->result;

    core->mem->mem_data = operand_2;
    if(core->ex->ctrl->aluSrc)
        operand_2 = core->ex->imm;
    
    if((core->ex->instruction & 0b1111111) == 0b0010011)
	funct7 = 0;
    
    alu_ctrl = aluControl(core->ex->ctrl->aluOp, ((core->ex->instruction & (0b111 << 12)) >> 12), funct7);
    alu(operand_1, operand_2, alu_ctrl, &(core->mem->result), &zero);


    // ID
    uint8_t en_pc;
    uint8_t if_id_en;
    core->id->ctrl = (ControlSignals *) malloc(sizeof(ControlSignals));
    control(core->id->ctrl, (core->id->instruction & 0b1111111), ((core->id->instruction & (0b111 << 12)) >> 12));
    core->ex->read_data_1 = core->reg_file[(core->instr_fetch->instruction & (0b11111 << 15)) >> 15]; 
    core->ex->read_data_2 = core->reg_file[(core->instr_fetch->instruction & (0b11111 << 20)) >> 20];
    core->ex->imm = buildImm(core->id->instruction);

    // Compute branch and jump PC's
    unsigned branch_PC = core->id->PC + core->id->imm;
    unsigned jump_PC;

    if(core->id->ctrl->jal)
        jump_PC = core->id->PC + core->id->imm;
    else if(core->id->ctrl->jalr)
        jump_PC = core->mem->result;


     // IF
    // Set PC to the correct values if it is enabled
    if(en_pc)
    {
	if((core->id->ctrl->beq && zero) || (core->id->ctrl->bne && !zero) || (core->ex->ctrl->blt && core->mem->result) || (core->ex->ctrl->bge && (~(core->mem->result) || zero)))
	    core->instr_fetch->PC = branch_PC;
	else if(core->id->ctrl->jal || core->id->ctrl->jalr)
	    core->instr_fetch->PC = jump_PC;
	else
	    core->instr_fetch->PC += 4;
    }

    if(if_id_en)
    {
	core->id->PC = core->instr_fetch->PC;
	core->id->instruction = core->instr_mem->instructions[core->id->PC / 4].instruction;
    }
	
    
    /* UNCOMMENT TO PRINT OUT THE INSTRUCTIONS, REGISTERS, AND DATA MEMORY
    printf("\nInstruction: %u\n", instruction);
    printf("rd: %u    rs1: %u    rs2: %u    imm: %d    result: %d\n", rd, rs_1, rs_2, imm, result);

    for(int i = 0; i < NUM_REGS; i++)
        printf("%s: %lu\n", REGISTER_NAME[i], core->reg_file[i]);

    for(int i = 0; i < NUM_BYTES; i += 8)
    {
	int data = 0;
	for(int j = 0; j < 7; j++)
	{
	    data |= (core->data_mem[i+j] << (j * 8));
	}
	printf("Data Address %d: %u\n", i, data);
    }
    */
    
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->instr_fetch->PC > core->instr_mem->last->addr)
        return false;
    return true;
}
