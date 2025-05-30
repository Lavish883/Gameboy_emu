#include "load_instr.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define reg_A 7
#define reg_AT_HL 6
#define reg_SP 3
#define reg_HL 2


//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
//char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };
// char* r16_group1_table[4] = { "BC", "DE", "HL", "SP" };
//char* r16_group2_table[4] = { "BC","DE","HL+","HL-" };

// LD A,B copy the val of B to A also handles [HL], basically A = B
void load_8bit_reg_to_8bit_reg(uint8_t* regA, uint8_t* regB, long* pT_cycles_count) {
	*regA = *regB;
	*pT_cycles_count += 4;
}

// LD r16 n16
void load_16bit_reg_to_immediate16(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t low_byte = read_immediate_mem_for_instructions(hCpu);
	uint8_t high_byte = read_immediate_mem_for_instructions(hCpu);
	set_16_bit_reg_value(hCpu, reg_index, (((uint16_t)high_byte) << 8) + low_byte);
	*pT_cycles_count += 12;
}

// LD A, [r16]
// get the data at the location in the mem that the 16bit reg is and put that into A
void load_8bit_data_from_mem_addr_to_A(CPU hCpu, int reg_index, long* pT_cycles_count) {
	// FIXs reg_indx for get_16_bit_reg_value as HL- is in 3rd indx but should be 2nd indx in table_all
	int corrected_indx = reg_index == 3 ? 2 : reg_index;
	uint8_t data = read_mem_for_instructions_at_addr(hCpu, get_16_bit_reg_value(hCpu, corrected_indx));
	*(get_8_bit_reg_addr(hCpu, reg_A)) = data;

	//increment or decrement for HL+ (2) or HL- (3)
	if (reg_index > 1) {
		uint16_t reg_16bit_value = get_16_bit_reg_value(hCpu, corrected_indx);
		reg_16bit_value = reg_index == 2 ? reg_16bit_value + 1 : reg_16bit_value - 1;
		set_16_bit_reg_value(hCpu, corrected_indx, reg_16bit_value);
	}
	*pT_cycles_count += 8;
}

// LD [a16], A
// stores the value of A at the addr described by next 2 bytes
void load_8bit_data_to_immediate16_addr_from_A(CPU hCpu, long* pT_cycles_count) {
	uint16_t addr = read_immediate16_mem_for_instructions(hCpu);
	set_mem_for_instructions_at_addr(hCpu, addr, *get_8_bit_reg_addr(hCpu, reg_A));
	*pT_cycles_count += 12;
}

// LD A, [a16]
// sets the value of A to the value at the addr described by next 2 bytes
void load_8bit_data_to_A_from_immediate16_addr(CPU hCpu, long* pT_cycles_count) {
	uint16_t addr = read_immediate16_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	*pReg = read_mem_for_instructions_at_addr(hCpu, addr);
	*pT_cycles_count += 16;
}

// LD [r16], A
// store the data of A into the addr that r16 is
void load_8bit_data_from_A_to_mem_addr(CPU hCpu, int reg_index, long* pT_cycles_count) {
	// FIXs reg_indx for get_16_bit_reg_value as HL- is in 3rd indx but should be 2nd indx in table_all
	int corrected_indx = reg_index == 3 ? 2 : reg_index;
	uint16_t addr = get_16_bit_reg_value(hCpu, corrected_indx);
	uint8_t value = *(get_8_bit_reg_addr(hCpu, reg_A));
	set_mem_for_instructions_at_addr(hCpu, addr, value);


	//increment or decrement for HL+ (2) or HL- (3)
	if (reg_index > 1) {
		uint16_t reg_16bit_value = get_16_bit_reg_value(hCpu, corrected_indx);
		reg_16bit_value = reg_index == 2 ? reg_16bit_value + 1 : reg_16bit_value - 1;
		set_16_bit_reg_value(hCpu, corrected_indx, reg_16bit_value);
	}
	*pT_cycles_count += 8;
}

// LD r8, n8 
// loads the value right after in the memory to that reg
void load_8bit_data_from_immediate8_to_reg(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t data = read_immediate_mem_for_instructions(hCpu);
	
	*pReg = data;
	*pT_cycles_count += reg_index == reg_AT_HL? 12: 8;
}

// LDH [a8], A
// store the data of A into the addr of 0xFF00 + immediate 8
void ldh_8bit_data_to_immediate8_from_A(CPU hCpu, long* pT_cycles_count) {
	uint8_t data = read_immediate_mem_for_instructions(hCpu);
	set_mem_for_instructions_at_addr(hCpu, 0xFF00 + data, *get_8_bit_reg_addr(hCpu, reg_A));
	*pT_cycles_count += 12;
}

void ldh_8bit_data_from_immediate_to_A(CPU hCpu, long* pT_cycles_count) {
	uint8_t data = read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	*pReg = read_mem_for_instructions_at_addr(hCpu, 0xFF00 + data);
	*pT_cycles_count += 12;
}

// LAD [a16], SP
// stores the sp at the address speficied by immdeiate 16
void load_SP_to_immediate16(CPU hCpu, long* pT_cycles_count) {
	uint16_t addr = read_immediate16_mem_for_instructions(hCpu);
	uint16_t SP = get_16_bit_reg_value(hCpu, reg_SP);

	uint8_t low_byte = (uint8_t)SP;
	uint8_t high_byte = (uint8_t)(SP >> 8);

	set_mem_for_instructions_at_addr(hCpu, addr, low_byte);
	set_mem_for_instructions_at_addr(hCpu, addr + 1, high_byte);

	*pT_cycles_count += 20;
}

// LD SP,HL sets SP to HL
void load_SP_from_HL(CPU hCpu, long* pT_cycles_count) {
	set_16_bit_reg_value(hCpu, reg_SP, get_16_bit_reg_value(hCpu, reg_HL));
	*pT_cycles_count += 8;
}

// LD HL, SP + e8 sets HL to SP + signed 8 bit value
void load_HL_from_SP_and_immediate_signed_8bit(CPU hCpu, long* pT_cycles_count) {
	int8_t e8 = (int8_t)read_immediate_mem_for_instructions(hCpu);
	uint16_t SP = get_16_bit_reg_value(hCpu, reg_SP);

	// Lower byte calculation (for flag logic)
	uint8_t low_sp = SP & 0xFF;
	uint8_t low_e8 = (uint8_t)e8;

	set_16_bit_reg_value(hCpu, reg_HL, e8 + SP);

	// Set flagse
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((low_sp & 0xF) + (low_e8 & 0xF)) > 0xF);
	set_flag(hCpu, 'C', (low_sp + low_e8) > 0xFF);

	*pT_cycles_count += 12;
}

void load_A_from_addr_of_C_from_internal_ram(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg_a = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t* pReg_c = get_8_bit_reg_addr(hCpu, 1);

	*pReg_a = read_mem_for_instructions_at_addr(hCpu, *pReg_c + 0xFF00);
	*pT_cycles_count += 8;
}

void load_A_to_addr_of_C_to_internal_ram(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg_a = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t* pReg_c = get_8_bit_reg_addr(hCpu, 1);

	set_mem_for_instructions_at_addr(hCpu, *pReg_c + 0xFF00, *pReg_a);
	*pT_cycles_count += 8;
}