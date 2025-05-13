#include "memory.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

struct memory {
	uint8_t* rom_bank;
};
typedef struct memory Memory;

MEMORY memory_create(uint8_t* rom_bank) {
	Memory* pMemory = (Memory*)SDL_malloc(sizeof(Memory));
	// Not my job here to get rid of rom if malloc fails
	if (pMemory != NULL) {
		pMemory->rom_bank = rom_bank;
	}
	return pMemory;
}
void memory_destroy(MEMORY* phMemory) {
	Memory* pMemory = (Memory*)(*phMemory);
	SDL_free(pMemory->rom_bank);
	SDL_free(pMemory);
	*phMemory = NULL;
}