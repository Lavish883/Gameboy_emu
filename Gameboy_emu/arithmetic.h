#pragma once

#include <stdint.h>
#include "cpu_instr_dec.h"

void increment_r16(CPU hCpu, int reg_index, long* pT_cycles_count); 
void decrement_r16(CPU hCpu, int reg_index, long* pT_cycles_count);

void increment_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void decrement_r8(CPU hCpu, int reg_index, long* pT_cycles_count);