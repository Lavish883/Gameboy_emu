#include "memory.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdarg.h>
#include <stdio.h>

#define KB_4 4096
#define KB_8 8192

struct memory {
	uint8_t* rom_bank;
	uint8_t* wram;
	uint8_t* vram;
	uint8_t* external_ram;
	uint8_t* io_registers_and_hram;
};
typedef struct memory Memory;

void clean_up_memory_on_fail(Memory** pMemory, int argc, ...);

MEMORY memory_create(uint8_t* rom_bank) {
	Memory* pMemory = (Memory*)SDL_malloc(sizeof(Memory));
	// Not my job here to get rid of rom if malloc fails
	if (pMemory != NULL) {
		pMemory->rom_bank = rom_bank;
		pMemory->wram = SDL_calloc(KB_8, sizeof(uint8_t));
		if (pMemory->wram == NULL) {
			clean_up_memory_on_fail(&pMemory, 0);
			goto exit;
		}
		pMemory->io_registers_and_hram = SDL_calloc(0xFF, sizeof(uint8_t));
		if (pMemory->io_registers_and_hram == NULL) {
			clean_up_memory_on_fail(&pMemory, 1, pMemory->wram);
			goto exit;
		}
		pMemory->vram = SDL_calloc(KB_8, sizeof(uint8_t));
		if (pMemory->vram == NULL) {
			clean_up_memory_on_fail(&pMemory, 2, pMemory->wram, pMemory->io_registers_and_hram);
			goto exit;
		}
		pMemory->external_ram = SDL_calloc(KB_8, sizeof(uint8_t));
		if (pMemory->external_ram == NULL) {
			clean_up_memory_on_fail(&pMemory, 3, pMemory->wram, pMemory->io_registers_and_hram, pMemory->external_ram);
			goto exit;	
		}
	}

	exit:
		return pMemory;
}
uint8_t memory_read(MEMORY hMemory, uint16_t addr) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr == 0xFF44) return 0x90;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		return pMemory->rom_bank[addr];
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return pMemory->vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return pMemory->external_ram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return pMemory->wram[addr - 0xC000];
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		return pMemory->io_registers_and_hram[addr - 0xFF00];
	}
	SDL_Log("Not implnemented yet mem addr %.4X", addr);
	return 0x0;
}
void memory_set(MEMORY hMemory, uint16_t addr, uint8_t value) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		pMemory->rom_bank[addr] = value;
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		pMemory->vram[addr - 0x8000] = value;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		pMemory->external_ram[addr - 0xA000] = value;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		pMemory->wram[addr - 0xC000] = value;
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		pMemory->io_registers_and_hram[addr - 0xFF00] = value;
	}
	else {
		SDL_Log("Not implnemented yet mem addr %.4X", addr);
	}
}

void clean_up_memory_on_fail(Memory** pMemory, int argc, ...){
	va_list args;
	va_start(args, argc);
	for (int i = 0; i < argc; i++) {
		uint8_t* data = va_arg(args, uint8_t*);
		SDL_free(data);
	}
	SDL_free(*pMemory);
	*pMemory = NULL;
	va_end(args);
}

// SHOULD ONLY BE USED FOR LAST CIRCUMSTANCE or NO BETTER WAY TO DO IT
// permisson given to only in cpu.c get_8_bit_reg_addr for now
uint8_t* memory_read_get_pointer(MEMORY hMemory, uint16_t addr) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		return pMemory->rom_bank + addr;
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return &(pMemory->vram[addr - 0x8000]);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return &(pMemory->wram[addr - 0xC000]);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return &(pMemory->external_ram[addr - 0xA000]);
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		return &(pMemory->io_registers_and_hram[addr - 0xFF00]);
	}
	SDL_Log("Not implnemented yet mem %.4X", addr);
	return NULL;
}
void memory_destroy(MEMORY* phMemory) {
	Memory* pMemory = (Memory*)(*phMemory);
	SDL_free(pMemory->rom_bank);
	SDL_free(pMemory);
	*phMemory = NULL;
}

// Called when 0xFF02 is written with 0x81
void handle_serial_transfer(MEMORY hMemory) {
	Memory* pMemory = (Memory*)hMemory;
	if (memory_read(pMemory, 0xFF02) == 0x81) {
		char out = memory_read(pMemory, 0xFF01);
		printf("%c", out);  // or buffer it
		// Mark transfer as done
		memory_set(pMemory, 0xFF02, 0x00);
	}
}