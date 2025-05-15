#include "load_instr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
//char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };
//char* condition_code_table[4] = { "NZ","Z","NC", "C" };


// Jumps to the addr pointed by the next two bytes (little endian)
void uncond_jump_immediate16(CPU hCpu, long* pT_cycles_count) {
	uint8_t low_byte = read_immediate_mem_for_instructions(hCpu);
	uint8_t high_byte = read_immediate_mem_for_instructions(hCpu);

	uint16_t new_addr = (((uint16_t)high_byte) << 8) + low_byte;
	set_16_bit_reg_value(hCpu, 4, new_addr);

	*pT_cycles_count += 16;
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
	
	int8_t jump_by = (int8_t)read_immediate_mem_for_instructions(hCpu);

	// Since true jump
	if (cond == true) {
		set_16_bit_reg_value(hCpu, 4, get_16_bit_reg_value(hCpu, 4) + jump_by);
		*pT_cycles_count += 4;
	}
	*pT_cycles_count += 8;
}