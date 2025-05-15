#pragma once

#include "cpu_instr_dec.h"
#include "instructions.h"
#include "memory.h"

CPU cpu_create(void);
void cpu_execute(CPU hCpu);
void cpu_connect_memory(CPU hCpu, MEMORY hMemory);
void cpu_destroy(CPU* phCpu);