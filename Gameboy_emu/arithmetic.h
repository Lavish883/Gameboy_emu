#pragma once

#include <stdint.h>
#include "cpu_instr_dec.h"
#include <stdbool.h>

void increment_r16(CPU hCpu, int reg_index, long* pT_cycles_count); 
void decrement_r16(CPU hCpu, int reg_index, long* pT_cycles_count);
void add_r16_to_HL(CPU hCpu, int reg_indx, long* pT_cycles_count);

void increment_r8(CPU hCpu, int reg_index, long* pT_cycles_count);
void decrement_r8(CPU hCpu, int reg_index, long* pT_cycles_count);

void r8_OR_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_CP_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_AND_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_XOR_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_ADD_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_SUB_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_ADC_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);
void r8_SBC_r8(CPU hCpu, int reg_index, bool is_reg, long* pT_cycles_count);

void RRA(CPU hCpu, long* pT_cycles_count);