#pragma once

#include "memory.h"
#include "types.h"

typedef void* PPU;

PPU ppu_create(void);
void ppu_execute(PPU hPPU, int t_cycles_took);
void ppu_destroy(PPU* phPPU);
void ppu_connect_memory(PPU hPPU, MEMORY hMemory);
void ppu_fetch_all_tiles(PPU pPPU, uint32_t* frame_buffer, int size);