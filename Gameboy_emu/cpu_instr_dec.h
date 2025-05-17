// FUCNTIONS THAT NEED TO BE ACCESSED BEWTWEEN CPU AND INSTRS
#include <stdint.h>
#include <stdbool.h>

typedef void* CPU;

extern char* r8_table[]; // Defined in cpu.c
extern char* r16_all_table[]; // Ddefined in cpu.c

uint8_t* get_8_bit_reg_addr(CPU hCpu, int reg_indx);
uint16_t get_16_bit_reg_value(CPU hCpu, int reg_index);
void set_16_bit_reg_value(CPU hCpu, int reg_index, uint16_t value);
void set_flag(CPU hCpu, char flag, int value);
bool get_flag(CPU hCpu, char flag);
void disable_interrupt_master(CPU hCpu);
void enable_interrupt_master(CPU hCpu);

// Gives the value immediate to the position where the cpu is (PC)
uint8_t read_immediate_mem_for_instructions(CPU hCpu);
// Returns the value at the given addr in the memory
uint8_t read_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr);
void set_mem_for_instructions_at_addr(CPU hCpu, uint16_t addr, uint8_t value);

uint16_t read_immediate16_mem_for_instructions(CPU hCpu);
