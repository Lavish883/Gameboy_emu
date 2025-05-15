#include "arithmetic.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define reg_AT_HL 6

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