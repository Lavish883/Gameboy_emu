#pragma once
#include <stdint.h>

typedef void* MEMORY;

MEMORY memory_create(uint8_t* rom_bank);
void memory_destroy(MEMORY* phMemory);
uint8_t memory_read(MEMORY hMemory, uint16_t addr);
void memory_set(MEMORY hMemory, uint16_t addr, uint8_t value);
uint8_t* memory_read_get_pointer(MEMORY hMemory, uint16_t addr);
void handle_serial_transfer(MEMORY hMemory);