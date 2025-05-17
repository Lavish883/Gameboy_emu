#pragma once
#include <stdint.h>
#include "cpu_instr_dec.h"

void uncond_jump_immediate16(CPU hCpu, long* pT_cycles_count);
void cond_jump_immediate16(CPU hCpu, int cond_indx, long* pT_cycles_count);
void uncond_call(CPU hCpu, long* pT_cycles_count);
void cond_call(CPU hCpu, int cond_indx ,long* pT_cycles_count);
void cond_jump_immediate_signed8(CPU hCpu, int cond_indx, long* pT_cycles_count);
void uncond_jump_immediate_signed8(CPU hCpu, long* pT_cycles_count);
void jump_to_addr_HL(CPU hCpu, long* pT_cycles_count);
void uncond_return(CPU hCpu, long* pT_cycles_count);
void cond_return(CPU hCpu, int cond_indx, long* pT_cycles_count);
void push_on_stack(CPU hCpu, int reg_indx, long* pT_cycles_count);
void pop_from_stack(CPU hCpu, int reg_indx, long* pT_cycles_count);
void RST_to_addr(CPU hCpu, uint8_t addr, long* pT_cycles_count);