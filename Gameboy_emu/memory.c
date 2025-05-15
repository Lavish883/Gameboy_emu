#include "memory.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

#define KB_4 4096

struct memory {
	uint8_t* rom_bank;
	uint8_t* wram;
};
typedef struct memory Memory;

MEMORY memory_create(uint8_t* rom_bank) {
	Memory* pMemory = (Memory*)SDL_malloc(sizeof(Memory));
	// Not my job here to get rid of rom if malloc fails
	if (pMemory != NULL) {
		pMemory->rom_bank = rom_bank;
		pMemory->wram = NULL;
		pMemory->wram = SDL_calloc(KB_4, sizeof(uint8_t));
	}
	return pMemory;
}
uint8_t memory_read(MEMORY hMemory, uint16_t addr) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		return pMemory->rom_bank[addr];
	}
	else if (addr >= 0xC000 && addr <= 0xCFFF) {
		return pMemory->wram[addr - 0xC000];
	}
	SDL_Log("Not implnemented yet mem addr %.4X", addr);
	return 0x0;
}
void memory_set(MEMORY hMemory, uint16_t addr, uint8_t value) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		pMemory->rom_bank[addr] = value;
	}
	else if (addr >= 0xC000 && addr <= 0xCFFF) {
		pMemory->wram[addr - 0xC000] = value;
	}
	else {
		SDL_Log("Not implnemented yet mem addr %.4X", addr);
	}
}

// SHOULD ONLY BE USED FOR LAST CIRCUMSTANCE or NO BETTER WAY TO DO IT
// permisson given to only in cpu.c get_8_bit_reg_addr for now
uint8_t* memory_read_get_pointer(MEMORY hMemory, uint16_t addr) {
	Memory* pMemory = (Memory*)hMemory;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		return pMemory->rom_bank + addr;
	}
	SDL_Log("Not implnemented yet mem");
	return NULL;
}
void memory_destroy(MEMORY* phMemory) {
	Memory* pMemory = (Memory*)(*phMemory);
	SDL_free(pMemory->rom_bank);
	SDL_free(pMemory);
	*phMemory = NULL;
}