#include "cpu.h"
#include <ctype.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdbool.h>

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
	unsigned long t_cycles_count;
	MEMORY hMemory;
	FILE* fp;
};
typedef struct cpu Cpu;

char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };

uint8_t get_8_bit_reg_value(Cpu* pCpu, char reg);
void set_8_bit_reg_value(Cpu* pCpu, char reg, uint8_t value);
void set_bit(uint8_t* pNum, int n, int value);

CPU cpu_create(void) {
	Cpu* pCpu = (Cpu*)SDL_malloc(sizeof(Cpu));
	pCpu->regA = 0x01;
	pCpu->regF = 0xB0;
	pCpu->regB = 0x00; 
	pCpu->regC = 0x13;
	pCpu->regD = 0x00;
	pCpu->regE = 0xD8;
	pCpu->regH = 0x01;
	pCpu->regL = 0x4D;
	pCpu->regSP = 0xFFFE;
	pCpu->regPC = 0x0100;
	pCpu->hMemory = NULL;
	pCpu->t_cycles_count = 0;
	FILE* fp = fopen("log.txt", "w");

	if (fp == NULL) {
		SDL_Log("FILE COULD NOT BE OPENED TO LOG");
		exit(1);
	}
	pCpu->fp = fp;
	/*
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
	SDL_Log("A is %d", test(pCpu));
	*/
	return pCpu;
}
void cpu_connect_memory(CPU hCpu, MEMORY hMemory) {
	Cpu* pCpu = (Cpu*)hCpu;
	pCpu->hMemory = hMemory;
}

// USE ONLY IN CPU.C
uint8_t get_8_bit_reg_value(Cpu* pCpu, char reg) {
	switch (reg) {
		case('A'): return pCpu->regA;
		case('B'): return pCpu->regB;
		case('C'): return pCpu->regC;
		case('D'): return pCpu->regD;
		case('E'): return pCpu->regE;
		case('F'): return pCpu->regF;
		case('H'): return pCpu->regH;
		case('L'): return pCpu->regL;
		default:
			break;
	}
	SDL_Log("INVALID 8 BIT REGISTER VALUE REQUESTED in get_8 requested %c", reg);
	return 0;
}
void set_8_bit_reg_value(Cpu* pCpu, char reg, uint8_t value) {
	switch (reg) {
		case('A'): pCpu->regA = value; return;
		case('B'): pCpu->regB = value; return;
		case('C'): pCpu->regC = value; return;
		case('D'): pCpu->regD = value; return;
		case('E'): pCpu->regE = value; return;
		case('F'): pCpu->regF = value; return;
		case('H'): pCpu->regH = value; return;
		case('L'): pCpu->regL = value; return;
		default:
			break;
	}
	SDL_Log("INVALID 8 BIT REGISTER VALUE REQUESTED in set_8 requested %c", reg);
}
void set_bit(uint8_t* pNum, int n, int value) {
	if (value == 0) {
		*pNum = *pNum & ~(1 << n);
	} else if (value == 1) {
		*pNum = *pNum | (1 << n);
	} else {
	 SDL_Log("PASSING IN THE WRONG VALUE TO SET BIT TO");
	}
}

uint16_t get_16_bit_reg_value(CPU hCpu, int reg_index) {
	if (reg_index < 0 || reg_index > 5) {
		SDL_Log("NOT A VALID 16 BIT REGISTER");
		return 0x0;
	}
	Cpu* pCpu = (Cpu*)hCpu;
	switch (reg_index){
		case(3): return pCpu->regSP;
		case(4): return pCpu->regPC;
		default: {
			uint8_t high_byte = get_8_bit_reg_value(pCpu, r16_table_all[reg_index][0]);
			uint8_t low_byte = get_8_bit_reg_value(pCpu, r16_table_all[reg_index][1]);
			return (((uint16_t)high_byte) << 8)+ low_byte;
		}
	}
}
void set_16_bit_reg_value(CPU hCpu, int reg_index, uint16_t value) {
	if (reg_index < 0 || reg_index > 5) {
		SDL_Log("NOT A VALID 16 BIT REGISTER");
		return;
	}
	
	Cpu* pCpu = (Cpu*)hCpu;
	switch (reg_index){
		case(3): pCpu->regSP = value; break;
		case(4): pCpu->regPC = value; break;
		default: {
			set_8_bit_reg_value(pCpu, r16_table_all[reg_index][0], (uint8_t)(value >> 8)); // HIGH register
			set_8_bit_reg_value(pCpu, r16_table_all[reg_index][1], (uint8_t)(value)); // LOW register
			break;
		}
	}
}
uint8_t* get_8_bit_reg_addr(CPU hCpu, int reg_indx) {
	Cpu* pCpu = (Cpu*)hCpu;
	switch (reg_indx){
		case(0): return &(pCpu->regB);
		case(1): return &(pCpu->regC);
		case(2): return &(pCpu->regD);
		case(3): return &(pCpu->regE);
		case(4): return &(pCpu->regH);
		case(5): return &(pCpu->regL);
		case(6): return memory_read_get_pointer(pCpu->hMemory, get_16_bit_reg_value(hCpu, 3));
		case(7): return &(pCpu->regA);
		default:
			break;
	}
	SDL_Log("ASKED FOR INVALID REGISTER");
	return NULL;
}
uint8_t read_immediate_mem_for_instructions(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	return memory_read(pCpu->hMemory, pCpu->regPC++);
}
uint8_t read_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr) {
	Cpu* pCpu = (Cpu*)hCpu;
	return memory_read(pCpu->hMemory, addr);
}
void set_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr, uint8_t value) {
	Cpu* pCpu = (Cpu*)hCpu;
	memory_set(pCpu->hMemory, addr, value);
}
void set_flag(CPU hCpu, char flag, int value) {
	Cpu* pCpu = (Cpu*)hCpu;
	flag = toupper(flag);
	switch (flag){
		case('Z'):{
			set_bit(&(pCpu->regF), 7, value);
			break;
		}
		case('N'): {
			set_bit(&(pCpu->regF), 6, value);
			break;
		}
		case('H'): {
			set_bit(&(pCpu->regF), 5, value);
			break;
		}
		case('C'): {
			set_bit(&(pCpu->regF), 4, value);
			break;
		}
		default:
			break;
	}
}
bool get_flag(CPU hCpu, char flag) {
	Cpu* pCpu = (Cpu*)hCpu;
	flag = toupper(flag);
	switch (flag) {
		case('Z'): return pCpu->regF >> 7;
		case('N'): return pCpu->regF >> 6;
		case('H'): return pCpu->regF >> 5;
		case('C'): return pCpu->regF >> 4;
		default: break;
	}
	return false;
}


void cpu_execute(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	char name[30];
	//SDL_Log("Before\nA: %.2X B: %.2X, C: %.2x, D: %.2x E: %.2x, F: %.2x, H: %.2x, L: %.2x SP: %.4x\n", pCpu->regA, pCpu->regB, pCpu->regC, pCpu->regD, pCpu->regE, pCpu->regF, pCpu->regH, pCpu->regL, pCpu->regSP);
	//SDL_Log("T cycles count %lu", pCpu->t_cycles_count);
	fprintf(pCpu->fp,"A:%.2X F:%.2X B:%.2X C:%.2X D:%.2X E:%.2X H:%.2X L:%.2X SP:%.4X PC:%.4X PCMEM:%.2X,%.2X,%.2X,%.2X\n",
		pCpu->regA,
		pCpu->regF,
		pCpu->regB,
		pCpu->regC,
		pCpu->regD,
		pCpu->regE,
		pCpu->regH,
		pCpu->regL,
		pCpu->regSP,
		pCpu->regPC,
		memory_read(pCpu->hMemory, pCpu->regPC),
		memory_read(pCpu->hMemory, pCpu->regPC + 1),
		memory_read(pCpu->hMemory, pCpu->regPC + 2),
		memory_read(pCpu->hMemory, pCpu->regPC + 3)
		);
	uint8_t opcode = memory_read(pCpu->hMemory, pCpu->regPC++);
	int exectued = 0;
	opcode_deconstructor_and_run(hCpu, opcode, name, 30, &(pCpu->t_cycles_count), &exectued);
	if (exectued == 0) {
		SDL_Log("%.2x means %s", opcode, name);
		exit(1);
	}
	//SDL_Log("After\nA: %.2x B: %.2x, C: %.2x, D: %.2x E: %.2x, F: %.2x, H: %.2x, L: %.2x SP: %.4x\n", pCpu->regA, pCpu->regB, pCpu->regC, pCpu->regD, pCpu->regE, pCpu->regF, pCpu->regH, pCpu->regL, pCpu->regSP);
	//SDL_Log("T cycles count %lu", pCpu->t_cycles_count);
	//SDL_Log("value at 0x110 is %.2x", memory_read(pCpu->hMemory, 0x111));
}
void cpu_destroy(CPU* phCpu) {
	Cpu* pCpu = (Cpu*)(*phCpu);
	fclose(pCpu->fp);
	SDL_free(pCpu);
	*phCpu = NULL;
}