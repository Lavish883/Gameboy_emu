#pragma once
#include <stdint.h>

enum status {
	FAILURE,
	SUCCESS
};

typedef enum status STATUS;

uint8_t read_bits_of_8bit_num(uint8_t num, int upper_bit, int lower_bit);