#pragma once
#include <stdint.h>
#include "cpu_instr_dec.h"

void uncond_jump_immediate16(CPU hCpu, long* pT_cycles_count);
void cond_jump_immediate_signed8(CPU hCpu, int cond_indx, long* pT_cycles_count);