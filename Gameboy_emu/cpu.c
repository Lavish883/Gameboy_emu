#include "cpu.h"
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

struct cpu {
	uint8_t regA;
	uint8_t regF;
	uint8_t regB;
	uint8_t regC;
	uint8_t regD;
	uint8_t regE;
	uint8_t regH;
	uint8_t regL;
	uint16_t regSP;
	uint16_t regPC;
};
typedef struct cpu Cpu;

CPU cpu_create(void) {
	Cpu* pCpu = (Cpu*)SDL_malloc(sizeof(Cpu));
	pCpu->regA = 0;
	pCpu->regF = 0;
	pCpu->regB = 0;
	pCpu->regC = 0;
	pCpu->regD = 0;
	pCpu->regE = 0;
	pCpu->regH = 0;
	pCpu->regL = 0;
	pCpu->regSP = 0;
	pCpu->regPC = 0x100;
	char name[30];
	int count = 0;
	for (int i = 0; i < 256; i++) {
		opcode_deconstructor(i, name, 30);
		if (strcmp(name, "ILLEGAL") != 0) {
			SDL_Log("Opcode %.2x means instruction %s", i, name);
			count++;
		}
	}
	SDL_Log("count %d", count);
	opcode_cb_deconstructor(0xE8, name, 30);
	SDL_Log("Opcode cb means %s", name);
	return pCpu;
}
void cpu_destroy(CPU* phCpu) {
	Cpu* pCpu = (Cpu*)(*phCpu);
	SDL_free(pCpu);
	*phCpu = NULL;
}