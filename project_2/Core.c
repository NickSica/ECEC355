#include "Core.h"
#include "Registers.h"

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));
    core->clk = 0;
    core->done = 0;
    core->instr_mem = i_mem;
    core->tick = tickFunc;
    memset(core->reg_file, 0, NUM_REGS*sizeof(core->reg_file[0]));
    memset(core->data_mem, 0, NUM_BYTES*sizeof(core->data_mem[0]));

    core->instr_fetch = malloc(sizeof(IF));
    core->instr_fetch->PC = 0;
    core->id = malloc(sizeof(ID));
    core->id->ctrl = calloc(1, sizeof(ControlSignals));
    core->ex = malloc(sizeof(EX));
    core->ex->ctrl = calloc(1, sizeof(ControlSignals));
    core->mem = malloc(sizeof(MEM));
    core->mem->ctrl = calloc(1, sizeof(ControlSignals));
    core->wb = malloc(sizeof(WB));
    core->wb->ctrl = calloc(1, sizeof(ControlSignals));

    /* UNCOMMENT  TO SET DEFAULT VALUES FOR matrix  */ /* 
    core->reg_file[1] = core->instr_mem->last->addr;
    core->reg_file[2] = NUM_BYTES - 8;
    core->reg_file[10] = 0;
    core->reg_file[11] = 128;

    for(int i = 0; i < 16; i++)
    core->data_mem[i*8] = i;
						       */

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_0 */ /*
    core->reg_file[1] = 0;
    core->reg_file[2] = 10;
    core->reg_file[3] = -15;
    core->reg_file[4] = 20;
    core->reg_file[5] = 30;
    core->reg_file[6] = -35;
    core->data_mem[40] = -63;
    core->data_mem[43] = 63;
						     */

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_1 */ /*
    core->reg_file[1] = 8;
    core->reg_file[3] = -4;
    core->reg_file[5] = 255;
    core->reg_file[6] = 1023;
						     */

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_1 */
    core->reg_file[1] = 8;
    core->reg_file[3] = -15;
    core->reg_file[5] = 255;
    core->reg_file[6] = 1023;
						    

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_2 */ /*
    core->reg_file[5] = 26;
    core->reg_file[6] = -27;
    core->data_mem[20] = 100; 
						     */
    
    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_3 */ /*
    core->reg_file[1] = 0;
    core->reg_file[2] = -5;
    core->reg_file[5] = -10;
    core->reg_file[6] = 25;
    core->data_mem[100] = -100; 
						     */

    /* UNCOMMENT TO SET DEFAULT VALUES FOR task_3 */ /*
    core->reg_file[1] = 8;
    core->reg_file[2] = -5;
    core->reg_file[5] = -10;
    core->reg_file[6] = 25;
    core->data_mem[100] = -100; 
						     */
    return core;
}

bool tickFunc(Core *core)
{
    // Clocked components
    // MEM/WB Registers
    free(core->wb->ctrl);
    core->wb->r_mem_data = core->mem->r_mem_data;
    core->wb->result = core->mem->result;
    core->wb->ctrl = core->mem->ctrl;
    core->wb->rd = core->mem->rd;
    core->wb->PC = core->mem->PC;

    // EX/MEM Registers
    core->mem->result = core->ex->result;
    core->mem->w_mem_data = core->ex->w_mem_data;
    core->mem->ctrl = core->ex->ctrl;
    core->mem->rd = core->ex->rd;
    core->mem->PC = core->ex->PC;

    // ID/EX Registers
    core->ex->ctrl = core->id->ctrl;
    core->ex->read_data_1 = core->id->read_data_1;
    core->ex->read_data_2 = core->id->read_data_2;
    core->ex->imm = core->id->imm;
    core->ex->rd = (core->id->instruction & (0b11111 << 7)) >> 7;
    core->ex->rs_1 = (core->id->instruction & (0b11111 << 15)) >> 15;
    core->ex->rs_2 = (core->id->instruction & (0b11111 << 20)) >> 20;
    core->ex->funct7 = core->id->funct7;
    core->ex->funct3 = (core->id->instruction & (0b111 << 12)) >> 12;
    core->ex->PC = core->id->PC;
    
    // IF/ID Registers
    core->id->instruction = core->instr_fetch->instruction;
    core->id->PC = core->instr_fetch->prevPC;

    
    // The stages are done in reverse order because of the dependence of the earlier stages on the later stages
    // Using C instead of an HDL causes this
    // WB
    int w_data;
    if(core->wb->ctrl->memToReg)
	w_data = core->wb->r_mem_data;
    else
	w_data = core->wb->result;

    if(core->wb->rd != 0 && core->wb->ctrl->regWrite)
	core->reg_file[core->wb->rd] = w_data;


    // MEM
    if(core->mem->ctrl->memWrite)
	for(int i = 0; i < 8; i++)
	    core->data_mem[core->mem->result + i] = (core->mem->w_mem_data & (0xFF << (i * 8)));

    if(core->mem->ctrl->memRead)
    {
	core->mem->r_mem_data = 0;
	for(int i = 0; i < 8; i++)
	    core->mem->r_mem_data |= (int8_t)(core->data_mem[core->mem->result + i] << (i * 8));
    }


    // EX
    uint8_t fwd = forwardUnit(core->ex->rs_1, core->ex->rs_2, core->mem->rd, core->wb->rd, core->mem->ctrl->regWrite, core->wb->ctrl->regWrite);
    uint8_t zero = 0;
    uint8_t alu_ctrl;
    int operand_1 = 0;
    int operand_2 = 0;
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

    core->ex->w_mem_data = operand_2;
    if(core->ex->ctrl->aluSrc)
        operand_2 = core->ex->imm;
    

    if(core->ex->ctrl->jal || core->ex->ctrl->jalr)
    {
	operand_1 = core->ex->PC;
	operand_2 = 4;
    }
    
    alu_ctrl = aluControl(core->ex->ctrl->aluOp, core->ex->funct3, core->ex->funct7);
    alu(operand_1, operand_2, alu_ctrl, &(core->ex->result), &zero);

    
    // ID
    uint8_t hazard_bit = hazardDetection(core->id->instruction, core->ex->rd, core->ex->ctrl);
    uint8_t en_pc = hazard_bit & 0b1; // Leaving the possibility for the hazard detection to do more
    uint8_t if_id_en = (hazard_bit & (0b1 << 1)) >> 1;
    uint8_t ctrl_en = (hazard_bit & (0b1 << 2)) >> 2;
    printf("HAZARD BIT: %u\n", hazard_bit);
    if(core->done)
	en_pc = 0;    
    
    core->id->read_data_1 = core->reg_file[(core->id->instruction & (0b11111 << 15)) >> 15]; 
    core->id->read_data_2 = core->reg_file[(core->id->instruction & (0b11111 << 20)) >> 20];
    core->id->imm = buildImm(core->id->instruction);

    core->id->ctrl = malloc(sizeof(ControlSignals));
    if(ctrl_en)
    {
	control(core->id->ctrl, (core->id->instruction & 0b1111111), ((core->id->instruction & (0b111 << 12)) >> 12));
    }
    else
    {
	memset(core->id->ctrl, 0, sizeof(ControlSignals));
    }

    uint8_t branch = 0;
    if((core->id->ctrl->beq && (core->id->read_data_1 == core->id->read_data_2)) || (core->id->ctrl->bne && (core->id->read_data_1 != core->id->read_data_2)) ||
       (core->id->ctrl->blt && (core->id->read_data_1 < core->id->read_data_2)) || (core->id->ctrl->bge && (core->id->read_data_1 >= core->id->read_data_2)))
    {
	branch = 1;
    }

    if((core->id->instruction & 0b1111111) == 0b0010011)
	core->id->funct7 = 0;
    else
	core->id->funct7 = (core->id->instruction & (0b1111111 << 25)) >> 25;

    
    // Compute branch and jump PC's
    unsigned branch_PC = core->id->PC + core->id->imm;
    unsigned jump_PC;

    if(core->id->ctrl->jal)
        jump_PC = core->id->PC + core->id->imm;
    else if(core->id->ctrl->jalr)
        jump_PC = core->ex->result;
    
    
    // IF
    // Set PC to the correct values if it is enabled
    if(core->done || branch || core->id->ctrl->jalr) // Flush IF/ID on branch
	core->instr_fetch->instruction = 0b00000000000000000000000000010011; // Insert NOPs to finish up
    else if(if_id_en)
	core->instr_fetch->instruction = core->instr_mem->instructions[core->id->PC / 4].instruction;

    core->instr_fetch->prevPC = core->instr_fetch->PC;  // The instructions won't get the right PC if this isn't set
    if(en_pc)
    {
	if(branch)
	    core->instr_fetch->PC = branch_PC;
	else if(core->id->ctrl->jal || core->id->ctrl->jalr)
	    core->instr_fetch->PC = jump_PC;
	else
	    core->instr_fetch->PC += 4;
    }

    
    /* UNCOMMENT TO PRINT OUT THE INSTRUCTIONS, REGISTERS, AND DATA MEMORY */
    printf("\nID Stage Instruction: %u\n", core->id->instruction);
    printf("EX Stage rd: %u    rs1: %u    rs2: %u    imm: %d    operand_1: %d    operand_2: %d    result: %d    MEM_DATA: %d\n",
	   core->ex->rd, core->ex->rs_1, core->ex->rs_2, core->ex->imm, operand_1, operand_2, core->ex->result, core->ex->w_mem_data);
    printf("MEM_STAGE MEM_DATA: %d\n", core->mem->w_mem_data);

    for(int i = 0; i < NUM_REGS; i++)
        printf("%s: %ld\n", REGISTER_NAME[i], core->reg_file[i]);


    /* UNCOMMENT TO SEE DATA AS DOUBLE-WORDS */ /*
    for(int i = 0; i < NUM_BYTES; i += 8)
    {
	
	signed long data = 0;
	for(int j = 0; j < 7; j++)
	{
	    data |= core->data_mem[i+j] << (j * 8);
	}
	printf("Data Address %d: %ld\n", i, data);
    }
						*/

    /* UNCOMMENT TO SEE DATA AS UNSIGNED BYTES */
    for(int i = 0; i < NUM_BYTES; i++)
	printf("Data Address %d: %u\n", i, core->data_mem[i]);

    
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->instr_fetch->PC > core->instr_mem->last->addr)
	core->done = 1;
    
    if(core->wb->PC > core->instr_mem->last->addr)
	return false;
    
    return true;
}







