#include "load_instr.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define reg_A 7
#define reg_AT_HL 6

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

// LD [r16], A
// store the data of A into the addr that r16 is
void load_8bit_data_from_A_to_mem_addr(CPU hCpu, int reg_index, long* pT_cycles_count) {
	// FIXs reg_indx for get_16_bit_reg_value as HL- is in 3rd indx but should be 2nd indx in table_all
	int corrected_indx = reg_index == 3 ? 2 : reg_index;
	uint16_t addr = get_16_bit_reg_value(hCpu, corrected_indx);
	set_mem_for_instructions_at_addr(hCpu, addr, *(get_8_bit_reg_addr(hCpu, reg_A)));
	
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