#pragma once

#include <stdint.h>
#include "cpu_instr_dec.h"

void SRL_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void RR_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void SWAP_r8(CPU hCpu, int reg_index, long* pT_cycles_count);