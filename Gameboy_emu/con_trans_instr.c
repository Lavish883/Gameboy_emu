#include "con_trans_instr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
//char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };
//char* r16_group3_table[4] = { "BC","DE","HL","AF" };
//char* condition_code_table[4] = { "NZ","Z","NC", "C" };

char* r16_table_all[6];

#define reg_PC 4
#define reg_SP 3
#define reg_HL 2

// Jumps to the addr pointed by the next two bytes (little endian)
void uncond_jump_immediate16(CPU hCpu, long* pT_cycles_count) {
	uint8_t low_byte = read_immediate_mem_for_instructions(hCpu);
	uint8_t high_byte = read_immediate_mem_for_instructions(hCpu);

	uint16_t new_addr = (((uint16_t)high_byte) << 8) + low_byte;
	set_16_bit_reg_value(hCpu, reg_PC, new_addr);

	*pT_cycles_count += 16;
}

void cond_jump_immediate16(CPU hCpu, int cond_indx, long* pT_cycles_count) {
	bool cond = false;
	switch (cond_indx) {
		case(0): { cond = !(get_flag(hCpu, 'Z')); break; }
		case(1): { cond = (get_flag(hCpu, 'Z')); break; }
		case(2): { cond = !(get_flag(hCpu, 'C')); break; }
		case(3): { cond = (get_flag(hCpu, 'C')); break; }
		default: break;
	}

	// Since true jump
	if (cond == true) {
		// WILL HANDLE CYCLES BY ITSELF
		uncond_jump_immediate16(hCpu, pT_cycles_count);
	}
	else {
		// you read next data either way so
		read_immediate16_mem_for_instructions(hCpu);
		*pT_cycles_count += 12;
	}
}

void uncond_jump_immediate_signed8(CPU hCpu, long* pT_cycles_count) {
	int8_t jump_by = (int8_t)read_immediate_mem_for_instructions(hCpu);
	set_16_bit_reg_value(hCpu, reg_PC, get_16_bit_reg_value(hCpu, reg_PC) + jump_by);
	*pT_cycles_count += 12;
}
// Jump realtive if the cond is true by the immdeiate signed 8 bit
void cond_jump_immediate_signed8(CPU hCpu, int cond_indx, long* pT_cycles_count) {
	bool cond = false;
	switch (cond_indx) {
		case(0): {cond = !(get_flag(hCpu, 'Z')); break;}
		case(1): {cond = (get_flag(hCpu, 'Z')); break;}
		case(2): {cond = !(get_flag(hCpu, 'C')); break;}
		case(3): {cond = (get_flag(hCpu, 'C')); break;}
		default: break;
	}

	// Since true jump
	if (cond == true) {
		// WILL HANDLE CYCLES BY ITSELF
		uncond_jump_immediate_signed8(hCpu, pT_cycles_count);
	} else {
		// you read next data either way so
		read_immediate_mem_for_instructions(hCpu);
		*pT_cycles_count += 8;
	}
}

// First pushes the regPC to the stack and then jumps to immediate16
void uncond_call(CPU hCpu, long* pT_cycles_count) {
	uint16_t reg_PC_value = get_16_bit_reg_value(hCpu, reg_PC) + 2;
	
	uint8_t high_byte = (uint8_t)(reg_PC_value >> 8);
	uint8_t low_byte = (uint8_t)(reg_PC_value);
	uint16_t sp = get_16_bit_reg_value(hCpu, reg_SP);

	// PUSH PC to stack
	set_mem_for_instructions_at_addr(hCpu, sp - 1, high_byte);
	set_mem_for_instructions_at_addr(hCpu, sp - 2, low_byte);

	set_16_bit_reg_value(hCpu, reg_SP, sp - 2); // decrement SP

	*pT_cycles_count += 8;

	// NOW JUMP to immediate 16, handles it own cycles
	uncond_jump_immediate16(hCpu, pT_cycles_count);
}

void cond_call(CPU hCpu, int cond_indx, long* pT_cycles_count) {
	bool cond = false;
	switch (cond_indx) {
		case(0): { cond = !(get_flag(hCpu, 'Z')); break; }
		case(1): { cond = (get_flag(hCpu, 'Z')); break; }
		case(2): { cond = !(get_flag(hCpu, 'C')); break; }
		case(3): { cond = (get_flag(hCpu, 'C')); break; }
		default: break;
	}

	// if true then CALL
	if (cond == true) {
		// HANDLES ITS OWN CYCLES
		uncond_call(hCpu, pT_cycles_count);
	} else {
		// you read next data either way so
		read_immediate16_mem_for_instructions(hCpu);
		*pT_cycles_count += 12;
	}
}

// Pops regPC from the stack  
void uncond_return(CPU hCpu, long* pT_cycles_count) {
	pop_from_stack(hCpu, reg_PC, pT_cycles_count);
}

void cond_return(CPU hCpu, int cond_indx, long* pT_cycles_count) {
	bool cond = false;
	switch (cond_indx) {
		case(0): { cond = !(get_flag(hCpu, 'Z')); break; }
		case(1): { cond = (get_flag(hCpu, 'Z')); break; }
		case(2): { cond = !(get_flag(hCpu, 'C')); break; }
		case(3): { cond = (get_flag(hCpu, 'C')); break; }
		default: break;
	}
	// if true then CALL
	if (cond == true) {
		// HANDLES ITS OWN CYCLES
		uncond_return(hCpu, pT_cycles_count); // 16 cycles
		*pT_cycles_count += 4; // 4 cycles for cond 
	}
	else {
		*pT_cycles_count += 8; // 4 cond cycles and 4 opcode
	}
}

// Pushes the 16 bit register to the stack
void push_on_stack(CPU hCpu, int reg_indx, long* pT_cycles_count) {
	// make sure that group3_table matches r16_all_table just got fix AF position
	reg_indx = reg_indx != 3 ? reg_indx : 5;

	uint16_t reg_value = get_16_bit_reg_value(hCpu, reg_indx);

	uint8_t high_byte = (uint8_t)(reg_value >> 8);
	uint8_t low_byte = (uint8_t)(reg_value);

	uint16_t sp = get_16_bit_reg_value(hCpu, reg_SP);

	// PUSH r16 to stack
	set_mem_for_instructions_at_addr(hCpu, sp - 1, high_byte);
	set_mem_for_instructions_at_addr(hCpu, sp - 2, low_byte);


	set_16_bit_reg_value(hCpu, reg_SP, sp - 2); // decrement SP

	*pT_cycles_count += 16;
}

//POP the 16 bit value from the stack and load into register
void pop_from_stack(CPU hCpu, int reg_indx, long* pT_cycles_count) {
	reg_indx = reg_indx != 3 ? reg_indx : 5; // fix all and r16_3 diff

	uint16_t sp = get_16_bit_reg_value(hCpu, reg_SP);

	// POP r16 from stack
	uint8_t low_byte = read_mem_for_instructions_at_addr(hCpu, sp);
	uint8_t high_byte = read_mem_for_instructions_at_addr(hCpu, sp + 1);

	uint16_t addr = (((uint16_t)high_byte) << 8) + low_byte;

	set_16_bit_reg_value(hCpu, reg_indx, addr);
	set_16_bit_reg_value(hCpu, reg_SP, (get_16_bit_reg_value(hCpu, reg_SP) + 2));

	*pT_cycles_count += 16;
}

void jump_to_addr_HL(CPU hCpu, long* pT_cycles_count) {
	uint16_t addr = get_16_bit_reg_value(hCpu, reg_HL);
	set_16_bit_reg_value(hCpu, reg_PC, addr);
	*pT_cycles_count += 4;
}

// pushses PC to stack then resets the PC to the addr 
void RST_to_addr(CPU hCpu, uint8_t addr, long* pT_cycles_count) {
	push_on_stack(hCpu, reg_PC, pT_cycles_count); // handles its cycles so need to do anything
	set_16_bit_reg_value(hCpu, reg_PC, addr);
}