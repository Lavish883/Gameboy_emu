#pragma once
#include <stdint.h>

typedef void* MEMORY;

MEMORY memory_create(uint8_t* rom_bank);
void memory_destroy(MEMORY* phMemory);