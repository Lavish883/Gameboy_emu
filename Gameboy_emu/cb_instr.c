#include "cb_instr.h"
#include <stdint.h>
#include <stdio.h>

//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };

void SRL_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;
	
	*pReg = *pReg >> 1;
	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void RR_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg >> 1) + (get_flag(hCpu, 'C') << 7);

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

// RRC //	*pReg = (*pReg >> 1) | (*pReg << 7);

// Basically swap the high bits (7-4) with (3-0) low bits
void SWAP_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t high_nibble = *pReg >> 4;
	uint8_t low_nibble = *pReg & 0xF;

	*pReg = ((low_nibble) << 4) + high_nibble;

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', 0);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}