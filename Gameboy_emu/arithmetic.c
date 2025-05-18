#include "arithmetic.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define reg_16_HL 2
#define reg_AT_HL 6
#define reg_A 7

//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
//char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };
// char* r16_group1_table[4] = { "BC", "DE", "HL", "SP" };
//char* r16_group2_table[4] = { "BC","DE","HL+","HL-" };

// INC r16, increases the 16 bit register by 1
void increment_r16(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint16_t r16_value = get_16_bit_reg_value(hCpu, reg_index);
	r16_value += 1;
	set_16_bit_reg_value(hCpu, reg_index, r16_value);
	*pT_cycles_count += 8;
}

// DEC r16, decreases register by 1
void decrement_r16(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint16_t r16_value = get_16_bit_reg_value(hCpu, reg_index);
	r16_value -= 1;
	set_16_bit_reg_value(hCpu, reg_index, r16_value);
	*pT_cycles_count += 8;
}

// ADD HL, r16
void add_r16_to_HL(CPU hCpu, int reg_indx, long* pT_cycles_count) {
	uint16_t reg_HL_value = get_16_bit_reg_value(hCpu, reg_16_HL);
	uint16_t adding_reg_value = get_16_bit_reg_value(hCpu, reg_indx);

	set_16_bit_reg_value(hCpu, reg_16_HL, reg_HL_value + adding_reg_value);
	// SET FLAGS
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((reg_HL_value & 0x0FFF) + (adding_reg_value & 0x0FFF)) > 0x0FFF);
	set_flag(hCpu, 'C', (reg_HL_value + adding_reg_value) > 0xFFFF);

	*pT_cycles_count += 8;
}

// INC r8, increases the 8 bit register by 1
void increment_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old = *pReg;

	*pReg = *pReg + 1;
	// SET FLAGS
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((old & 0x0F) + 1) > 0x0F);

	*pT_cycles_count += reg_index == reg_AT_HL ? 12 : 4;
}

// DEC r8, decrease the 8 but regisster by 1
void decrement_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old = *pReg;

	*pReg = *pReg - 1;
	// SET FLAGS
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 1);
	set_flag(hCpu, 'H', (old & 0x0F) == 0);

	*pT_cycles_count += reg_index == reg_AT_HL ? 12 : 4;
}


// OR adding is_reg allows us to pass false and then it checks immdeatie 8 and does stuff with that
void r8_OR_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	*pReg = *pReg | reg_value;

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', 0);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// CP adding is_reg allows us to pass false and then it checks immdeatie 8 and does stuff with that
// Subs A - B and sets falg doenst impact A's value
void r8_CP_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	uint8_t value = *pReg - reg_value;

	set_flag(hCpu, 'Z', value == 0);
	set_flag(hCpu, 'N', 1);
	set_flag(hCpu, 'H', ((*pReg & 0xF) < (reg_value & 0xF)));
	set_flag(hCpu, 'C', *pReg < reg_value);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// AND 
void r8_AND_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	*pReg = *pReg & reg_value;

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 1);
	set_flag(hCpu, 'C', 0);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// XOR
void r8_XOR_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);

	*pReg = *pReg ^ reg_value;

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', 0);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// ADD
void r8_ADD_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint16_t old_value = *pReg;

	*pReg = *pReg + reg_value;

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((old_value & 0xF) + (reg_value & 0xF)) > 0xF);
	set_flag(hCpu, 'C', (old_value + reg_value) > 0xFF);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// SUB
void r8_SUB_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint16_t old_value = *pReg;

	*pReg = *pReg - reg_value;

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 1);
	set_flag(hCpu, 'H', ((old_value & 0xF) < (reg_value & 0xF)));
	set_flag(hCpu, 'C', old_value < reg_value);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// ADC
void r8_ADC_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t flag_value = get_flag(hCpu, 'C');

	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint16_t old_value = *pReg;


	*pReg = *pReg + reg_value + flag_value;


	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((old_value & 0xF) + (reg_value & 0xF) + flag_value) > 0xF);
	set_flag(hCpu, 'C', (old_value + reg_value + flag_value) > 0xFF);

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// SBC
void r8_SBC_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count) {
	uint8_t reg_value = is_reg ? *get_8_bit_reg_addr(hCpu, reg_index) : read_immediate_mem_for_instructions(hCpu);
	uint8_t flag_value = get_flag(hCpu, 'C');

	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint16_t old_value = *pReg;


	*pReg = *pReg - reg_value - flag_value;


	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 1);
	set_flag(hCpu, 'H', (old_value & 0xF) < ((reg_value & 0xF) + flag_value));
	set_flag(hCpu, 'C', old_value < (reg_value + flag_value));

	if (is_reg == false || reg_index == reg_AT_HL) {
		pT_cycles_count += 8;
	}
	else {
		pT_cycles_count += 4;
	}
}

// ADD SP, e8
void add_signed_immediate_8_to_SP(CPU hCpu, long* pT_cycles_count) {
	int8_t e8 = (int8_t)read_immediate_mem_for_instructions(hCpu);
	uint16_t sp_val = get_16_bit_reg_value(hCpu, 3);

	// Set flags
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', ((sp_val & 0xF) + (e8 & 0xF)) > 0xF);
	set_flag(hCpu, 'C', ((sp_val & 0xFF) + (e8 & 0xFF)) > 0xFF);

	set_16_bit_reg_value(hCpu, 3, sp_val + e8);
	*pT_cycles_count += 16;
}

void CPL(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg_A = get_8_bit_reg_addr(hCpu, reg_A);
	*pReg_A = ~(*pReg_A);
	
	// Set flags
	set_flag(hCpu, 'N', 1);
	set_flag(hCpu, 'H', 1);

	*pT_cycles_count += 4;
}

// RRA
void RRA(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t old_value = *pReg;

	*pReg = (*pReg >> 1) + (get_flag(hCpu, 'C') << 7);

	// Set flags
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += 4;
}

// RLA
void RLA(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t old_value = *pReg;

	*pReg = (*pReg << 1) + get_flag(hCpu, 'C');

	// Set flags
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', (old_value & 0x80) >> 7);

	*pT_cycles_count += 4;
}

// RRCA
void RRCA(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t old_value = *pReg;

	*pReg = (*pReg >> 1) | (*pReg << 7);

	// Set flags
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += 4;
}
// RLCA
void RLCA(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t old_value = *pReg;

	*pReg = (*pReg << 1) | (*pReg >> 7);

	// Set flags
	set_flag(hCpu, 'Z', 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', (old_value & 0x80) >> 7);

	*pT_cycles_count += 4;
}

// DAA
void DAA(CPU hCpu, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_A);
	uint8_t adjustment = 0;

	bool n_flag = get_flag(hCpu, 'N');
	bool h_flag = get_flag(hCpu, 'H');
	bool c_flag = get_flag(hCpu, 'C');

	if (n_flag == true) {
		if (h_flag == true) adjustment += 0x06;
		if (c_flag == true) adjustment += 0x60;
		*pReg -= adjustment;
	}
	else {
		if (h_flag == true || (*pReg & 0x0F) > 0x09) adjustment += 0x06;
		if (c_flag == true || *pReg > 0x99) {
			adjustment += 0x60;
			set_flag(hCpu, 'C', 1);
		}
		*pReg += adjustment;
	}

	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'H', 0);

	*pT_cycles_count += 4;
}