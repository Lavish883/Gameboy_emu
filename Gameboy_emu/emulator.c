#include "emulator.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdarg.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

struct emulator
{
	CPU hCpu;
	MEMORY hMemory;
	PPU hPPU;
	uint32_t* frame_buffer;
};

typedef struct emulator Emulator;
typedef void* EMULATOR_COMPONENT;
typedef void(*DestroyFunc)(EMULATOR_COMPONENT*);

// n amount of total args takes in 2 per component destroy, order of destroy, function pointer
void clean_up_emulator_on_fail(Emulator** pEmulator, int argc, ...);
STATUS emulator_rom_set_up(char* rom_name, uint8_t** pRom);

EMULATOR emulator_create(char* rom_name) {
	Emulator* pEmulator = (Emulator*)SDL_malloc(sizeof(Emulator));
	// rom data
	uint8_t* rom_bank = NULL;
	uint32_t* frame_buffer = (uint32_t*)SDL_malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));

	if (frame_buffer == NULL) {
		SDL_Log("Frame buffer wasn't able to be made");
		SDL_free(pEmulator);
		pEmulator = NULL;
		goto exit;
	}

	pEmulator->frame_buffer = frame_buffer;

	STATUS rom_bank_status = emulator_rom_set_up(rom_name, &rom_bank);
	if (rom_bank_status == FAILURE) {
		SDL_Log("Rom wasn't able to be made");
		SDL_free(pEmulator);
		pEmulator = NULL;
		goto exit;
	}
	if (pEmulator != NULL) {
		pEmulator->hCpu = cpu_create();
		if (pEmulator->hCpu == NULL) {
			clean_up_emulator_on_fail(&pEmulator, 0);
			goto exit;
		}
		pEmulator->hMemory = memory_create(rom_bank);
		if (pEmulator->hMemory == NULL) {
			clean_up_emulator_on_fail(&pEmulator, 2, &(pEmulator->hCpu), &cpu_destroy);
			// usually memory would handle free memory stuff but since memory part wasnt made emualtor frees the rom part
			SDL_free(rom_bank); 
			goto exit;
		}
		pEmulator->hPPU = ppu_create();
		if (pEmulator->hPPU == NULL) {
			clean_up_emulator_on_fail(&pEmulator, 2, &(pEmulator->hCpu), &cpu_destroy, &(pEmulator->hPPU), &ppu_destroy);
			goto exit;
		}
		
		
		
		// CONNECT COMPONETS AFERT EVERYTHING HAS BEEN ALLOCATED
		cpu_connect_memory(pEmulator->hCpu, pEmulator->hMemory);
		ppu_connect_memory(pEmulator->hPPU, pEmulator->hMemory);
	}

	exit:
		return pEmulator;
}
STATUS emulator_rom_set_up(char* rom_name, uint8_t** pRom){
	STATUS local_status;
	
	*pRom = return_file_as_array(rom_name, &local_status);
	SDL_Log("Catridge type: %d", (*pRom)[0x147]);
	return local_status;
}
void emulator_run(EMULATOR hEmulator) {
	Emulator* pEmulator = (Emulator*)hEmulator;
	int t_cycles_took = 0;

	cpu_execute(pEmulator->hCpu, &t_cycles_took);
	ppu_execute(pEmulator->hPPU, t_cycles_took);
}

void clean_up_emulator_on_fail(Emulator** pEmulator, int argc,  ...) {
	va_list args;
	va_start(args, argc);	
	for (int i = 0; i < argc / 2; i++) {
		EMULATOR_COMPONENT* phEmu_comp_struct = va_arg(args, EMULATOR_COMPONENT*);
		DestroyFunc pfDestroy = va_arg(args, DestroyFunc);
		pfDestroy(phEmu_comp_struct);
	}
	SDL_free(*pEmulator);
	*pEmulator = NULL;	

	va_end(args);
}

uint32_t* emulator_get_frame_buffer(EMULATOR hEmulator) {
	Emulator* pEmulator = (Emulator*)hEmulator;
	ppu_fetch_all_tiles(pEmulator->hPPU, pEmulator->frame_buffer, SCREEN_WIDTH * SCREEN_HEIGHT);

	return pEmulator->frame_buffer;
}

void emulator_destroy(EMULATOR* phEmulator) {
	Emulator* pEmulator = (Emulator*)(*phEmulator);
	cpu_destroy(pEmulator->hCpu);
	SDL_free(pEmulator);
	*phEmulator = NULL;
}