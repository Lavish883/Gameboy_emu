#pragma once
#include <stdint.h>
#include "cpu_instr_dec.h"

// LD
void load_8bit_reg_to_8bit_reg(uint8_t* regA, uint8_t* regB, long* pT_cycles_count);
void load_16bit_reg_to_immediate16(CPU hCpu, int reg_index, long* pT_cycles_count);
void load_8bit_data_from_mem_addr_to_A(CPU hCpu, int reg_index, long* pT_cycles_count);
void load_8bit_data_to_immediate16_addr_from_A(CPU hCpu, long* pT_cycles_count);
void load_8bit_data_to_A_from_immediate16_addr(CPU hCpu, long* pT_cycles_count);

void ldh_8bit_data_to_immediate8_from_A(CPU hCpu, long* pT_cycles_count);
void ldh_8bit_data_from_immediate_to_A(CPU hCpu, long* pT_cycles_count);

void load_8bit_data_from_A_to_mem_addr(CPU hCpu, int reg_index, long* pT_cycles_count);
void load_8bit_data_from_immediate8_to_reg(CPU hCpu, int reg_index, long* pT_cycles_count);

void load_SP_to_immediate16(CPU hCpu, long* pT_cycles_count);
void load_SP_from_HL(CPU hCpu, long* pT_cycles_count);