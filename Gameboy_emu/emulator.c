#include "emulator.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdarg.h>

struct emulator
{
	CPU hCpu;
	MEMORY hMemory;
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
		
		
		
		
		// CONNECT COMPONETS AFERT EVERYTHING HAS BEEN ALLOCATED
		cpu_connect_memory(pEmulator->hCpu, pEmulator->hMemory);
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
	cpu_execute(pEmulator->hCpu);
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

void emulator_destroy(EMULATOR* phEmulator) {
	Emulator* pEmulator = (Emulator*)(*phEmulator);
	cpu_destroy(pEmulator->hCpu);
	SDL_free(pEmulator);
	*phEmulator = NULL;
}

void emulator_check_if_test_passed(EMULATOR hEmulator) {
	Emulator* pEmulator = (Emulator*)hEmulator;
	for (int i = 0; i < 16; i++) {
		char c = memory_read(pEmulator->hMemory, 0xA000 + i);
		SDL_Log("%c", c);
	}
}