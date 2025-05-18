#include "cb_instr.h"
#include <stdint.h>
#include <stdio.h>

//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
int get_bit(uint8_t num, int bit);
void set_bit_for_r8(uint8_t* pNum, int bit, int value);

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

void RL_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg << 1) + get_flag(hCpu, 'C');

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', (old_value & 0x80) >> 7);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void RLC_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg << 1) | (*pReg >> 7);

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', (old_value & 0x80) >> 7);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

// RRC //	*pReg = (*pReg >> 1) | (*pReg << 7);
void RRC_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg >> 1) | (*pReg << 7);

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

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

void SRA_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg >> 1);
	*pReg += old_value & 0x80; // As the 7th bit is preserverd in SRA

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', old_value & 0x01);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void SLA_r8(CPU hCpu, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	uint8_t old_value = *pReg;

	*pReg = (*pReg << 1);

	// Set flags
	set_flag(hCpu, 'Z', *pReg == 0);
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 0);
	set_flag(hCpu, 'C', (old_value & 0x80) >> 7);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void COPY_CPL_BIT_TO_Z_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	
	// SET Flags
	set_flag(hCpu, 'Z', !get_bit(*pReg, bit));
	set_flag(hCpu, 'N', 0);
	set_flag(hCpu, 'H', 1);

	*pT_cycles_count += reg_index != 6 ? 8 : 12; // [HL] takes 12 cycles
}

void RES_BIT_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	set_bit_for_r8(pReg, bit, 0);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void SET_BIT_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count) {
	uint8_t* pReg = get_8_bit_reg_addr(hCpu, reg_index);
	set_bit_for_r8(pReg, bit, 1);

	*pT_cycles_count += reg_index != 6 ? 8 : 16; // [HL] takes 16 cycles
}

void set_bit_for_r8(uint8_t* pNum, int bit, int value) {
	if (value == 1) {
		*pNum |= (0x01 << bit);
	}
	else if (value == 0) {
		*pNum &= ~(0x01 << bit);
	}
	else {
		printf("NOT VALID VALUE  to set bit value is %d", value);
	}
}