#pragma once

#include "types.h"
#include "handle_files.h"
#include "cpu.h"
#include "memory.h"


typedef void* EMULATOR;

EMULATOR emulator_create(char* rom_name);
void emulator_run(EMULATOR hEmulator);
void emulator_destroy(EMULATOR* phEmulator);