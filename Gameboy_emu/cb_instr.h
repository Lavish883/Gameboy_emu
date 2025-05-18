#pragma once

#include <stdint.h>
#include "cpu_instr_dec.h"

void SRL_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void RR_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void RL_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void SWAP_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void RLC_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void RRC_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void SRA_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void SLA_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void COPY_CPL_BIT_TO_Z_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count);
void RES_BIT_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count);
void SET_BIT_r8(CPU hCpu, int bit, int reg_index, long* pT_cycles_count);