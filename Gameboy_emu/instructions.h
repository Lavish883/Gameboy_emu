#pragma once
#include <stdint.h>
#include <stdbool.h>

char* opcode_deconstructor(uint8_t opcode,char* name, int name_length);
char* opcode_cb_deconstructor(uint8_t opcode, char* name, int name_length);
uint8_t read_bits_of_opcode(uint8_t opcode, int upper_bit, int lower_bit);