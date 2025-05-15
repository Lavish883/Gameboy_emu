#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "load_instr.h"
#include "arithmetic.h"
#include "con_trans_instr.h"

#include "cpu_instr_dec.h"

uint8_t test(CPU hCpu);
char* opcode_deconstructor_and_run(CPU hCpu, uint8_t opcode, char* name, int name_length, long* pT_cycles_count, int* exectued);
char* opcode_cb_deconstructor(uint8_t opcode, char* name, int name_length);
uint8_t read_bits_of_opcode(uint8_t opcode, int upper_bit, int lower_bit);