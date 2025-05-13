#pragma once
#include "instructions.h"

typedef void* CPU;

CPU cpu_create(void);
void cpu_destroy(CPU* phCpu);