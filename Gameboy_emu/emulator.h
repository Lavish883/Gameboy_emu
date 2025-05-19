#pragma once

#include "types.h"
#include "handle_files.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"


typedef void* EMULATOR;

EMULATOR emulator_create(char* rom_name);
void emulator_run(EMULATOR hEmulator);
void emulator_destroy(EMULATOR* phEmulator);
uint32_t* emulator_get_frame_buffer(EMULATOR hEmulator);