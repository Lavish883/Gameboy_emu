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
	bool IME;
	unsigned long t_cycles_count;
	unsigned int div_cycles_count;
	unsigned int tima_cycles_count; // counts cycles to incremnet TIMA
	bool in_low_power_mode;
	MEMORY hMemory;
	FILE* fp;
};
typedef struct cpu Cpu;

char* r8_table[8] = { "B","C","D","E","H","L","[HL]", "A" };
char* r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };
uint16_t interrupt_handler_addrs[5] = {0x0040, 0x0048, 0x0050, 0x0058, 0x0060};

void handle_clock(CPU hCpu);
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
	pCpu->IME = false;
	pCpu->t_cycles_count = 0;
	pCpu->div_cycles_count = 0;
	pCpu->tima_cycles_count = 0;
	pCpu->in_low_power_mode = false;
	pCpu->hMemory = NULL;
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
int get_bit(uint8_t num, int bit) {
	return (num & (0x01 << bit)) >> bit;
}

uint16_t get_16_bit_reg_value(CPU hCpu, int reg_index) {
	if (reg_index < 0 || reg_index > 5) {
		SDL_Log("NOT A VALID 16 BIT REGISTER asking for %d", reg_index);
		return 0x0;
	}
	Cpu* pCpu = (Cpu*)hCpu;
	switch (reg_index){
		case(3): return pCpu->regSP;
		case(4): return pCpu->regPC;
		default: {
			uint8_t high_byte = get_8_bit_reg_value(pCpu, r16_table_all[reg_index][0]);
			uint8_t low_byte = get_8_bit_reg_value(pCpu, r16_table_all[reg_index][1]);
			if (r16_table_all[reg_index][1] == 'F') { // only set up first 4 bits
				low_byte &= 0xF0;
			}
			return (((uint16_t)high_byte) << 8) + low_byte;
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
		case(3): {
			pCpu->regSP = value; 
			break;
		}
		case(4): pCpu->regPC = value; break;
		default: {
			set_8_bit_reg_value(pCpu, r16_table_all[reg_index][0], (uint8_t)(value >> 8)); // HIGH register
			if (r16_table_all[reg_index][1] == 'F') { // only set up first 4 bits
				value = (uint8_t)value;
				value &= 0xF0;
			}
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
		case(6): return memory_read_get_pointer(pCpu->hMemory, get_16_bit_reg_value(hCpu, 2));
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
uint16_t read_immediate16_mem_for_instructions(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	uint8_t low_byte = memory_read(pCpu->hMemory, pCpu->regPC++);
	uint8_t	high_byte = memory_read(pCpu->hMemory, pCpu->regPC++);

	return (((uint16_t)high_byte) << 8) + low_byte;
}

uint8_t read_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr) {
	Cpu* pCpu = (Cpu*)hCpu;
	return memory_read(pCpu->hMemory, addr);
}
void set_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr, uint8_t value) {
	Cpu* pCpu = (Cpu*)hCpu;
	if (addr == 0xFF04) { // means DIV register
		value = 0;
	}
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
		case('Z'): return (pCpu->regF >> 7) & 0x1;
		case('N'): return (pCpu->regF >> 6) & 0x1;
		case('H'): return (pCpu->regF >> 5) & 0x1;
		case('C'): return (pCpu->regF >> 4) & 0x1;
		default: break;
	}
	return false;
}

void disable_interrupt_master(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	pCpu->IME = false;
	pCpu->t_cycles_count += 4;
}
void enable_interrupt_master(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	pCpu->IME = true;
	pCpu->t_cycles_count += 4;
}
void halt_instr(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	pCpu->in_low_power_mode = true;
	pCpu->t_cycles_count += 4;
}

// HALT bug might not be supported?
void cpu_handle_interrupts(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;

	// Get IE (Interrupt enable) and IF (Interrupt flag)
	uint8_t IE = memory_read(pCpu->hMemory, 0xFFFF);
	uint8_t IF = memory_read(pCpu->hMemory, 0xFF0F);

	if ((IE & IF) != 0) { // WAKES UP GB
		pCpu->in_low_power_mode = false;
	}
	if (pCpu->IME == false) return;
	
	int t_cycles_before = pCpu->t_cycles_count;
	
	// only nedd 0,1,2,3,4 bit info
	bool IE_Bits[5] = { get_bit(IE, 0), get_bit(IE, 1), get_bit(IE, 2), get_bit(IE, 3), get_bit(IE, 4) };
	bool IF_Bits[5] = { get_bit(IF, 0), get_bit(IF, 1), get_bit(IF, 2), get_bit(IF, 3), get_bit(IF, 4) };

	// go through IE_bits and see if they are enabled then check IF form 0,1,2,3,4 as highest priotiy in that order
	for (int i = 0; i < 5; i++) {
		if (IE_Bits[i] == true && IF_Bits[i]) {
			// Handle Intrepput, takes 20 t=cycles total
			pCpu->IME = false;
			set_bit(memory_read_get_pointer(pCpu->hMemory, 0xFF0F), i, 0); // SET bit correpsonding to this intruppt in IF to 0

			push_on_stack(pCpu, 4, &(pCpu->t_cycles_count)); // takes 16
			pCpu->regPC = interrupt_handler_addrs[i];
			pCpu->t_cycles_count += 4;
			break;
		}
	}
	pCpu->div_cycles_count += pCpu->t_cycles_count - t_cycles_before;
	pCpu->tima_cycles_count += pCpu->t_cycles_count - t_cycles_before;
}
void handle_clock(CPU hCpu) {
	Cpu* pCpu = (Cpu*)hCpu;
	if (pCpu->div_cycles_count >= 256) {
		// incremnent DIV
		memory_set(pCpu->hMemory, 0xFF04, memory_read(pCpu->hMemory, 0xFF04) + 1);
		pCpu->div_cycles_count -= 256;
	}
	// Check TAC to see if want to do time
	uint8_t TAC = memory_read(pCpu->hMemory, 0xFF07);
	if (get_bit(TAC, 2) == 1) { // MEANS TAC IS ENABLED
		// IK not an opcode but useful here
		int clock_select = read_bits_of_opcode(TAC, 1, 0); // decided how mnay cycles to increment TIMA
		unsigned int number_of_t_cycles = 0;

		switch (clock_select){
			case 0: number_of_t_cycles = 1024; break;
			case 1: number_of_t_cycles = 16; break;
			case 2: number_of_t_cycles = 64; break;
			case 3: number_of_t_cycles = 256; break;
			default: {
				printf("SOMETING WENT WRONG IN TIMER");
				break;
			}
		}

		if (pCpu->tima_cycles_count >= number_of_t_cycles) {
			// Increment TIMA
			uint16_t TIMA = (uint16_t)memory_read(pCpu->hMemory, 0xFF05);
			uint8_t TMA = memory_read(pCpu->hMemory, 0xFF06);
			uint8_t* pIF = memory_read_get_pointer(pCpu->hMemory, 0xFF0F); // Interuppt flag

			// Check if incrementing will make it overflow
			if (TIMA + 1 > 0xFF) {
				memory_set(pCpu->hMemory, 0xFF05, TMA); // sets value to TMA
				set_bit(pIF, 2, 1); // request an interrupt
			} else {
				memory_set(pCpu->hMemory, 0xFF05, (uint8_t)(TIMA + 1));
			}
			pCpu->tima_cycles_count -= number_of_t_cycles;
		}
	}
}

void cpu_execute(CPU hCpu, int* pT_cycles_took) {
	Cpu* pCpu = (Cpu*)hCpu;
	char name[30];
	//SDL_Log("Before\nA: %.2X B: %.2X, C: %.2x, D: %.2x E: %.2x, F: %.2x, H: %.2x, L: %.2x SP: %.4x\n", pCpu->regA, pCpu->regB, pCpu->regC, pCpu->regD, pCpu->regE, pCpu->regF, pCpu->regH, pCpu->regL, pCpu->regSP);
	//SDL_Log("T cycles count %lu", pCpu->t_cycles_count);
	//fprintf(stdout,"A:%.2X F:%.2X B:%.2X C:%.2X D:%.2X E:%.2X H:%.2X L:%.2X SP:%.4X PC:%.4X PCMEM:%.2X,%.2X,%.2X,%.2X\n",
	//	pCpu->regA,
	//	pCpu->regF,
	//	pCpu->regB,
	//	pCpu->regC,
	//	pCpu->regD,
	//	pCpu->regE,
	//	pCpu->regH,
	//	pCpu->regL,
	//	pCpu->regSP,
	//	pCpu->regPC,
	//	memory_read(pCpu->hMemory, pCpu->regPC),
	//	memory_read(pCpu->hMemory, pCpu->regPC + 1),
	//	memory_read(pCpu->hMemory, pCpu->regPC + 2),
	//	memory_read(pCpu->hMemory, pCpu->regPC + 3)
	//	);	
	handle_serial_transfer(pCpu->hMemory);
	handle_clock(pCpu);
	cpu_handle_interrupts(pCpu);
	
	if (pCpu->in_low_power_mode == false) {
		int t_cycles_before = pCpu->t_cycles_count;
		uint8_t opcode = memory_read(pCpu->hMemory, pCpu->regPC++);
		int exectued = 0;
		opcode_deconstructor_and_run(hCpu, opcode, name, 30, &(pCpu->t_cycles_count), &exectued);
		if (exectued == 0) {
			SDL_Log("%.2x means %s", opcode, name);
			exit(1);
		}
		pCpu->div_cycles_count += pCpu->t_cycles_count - t_cycles_before;
		pCpu->tima_cycles_count += pCpu->t_cycles_count - t_cycles_before;
		// FOR PPU updates
		*pT_cycles_took = pCpu->t_cycles_count - t_cycles_before;
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