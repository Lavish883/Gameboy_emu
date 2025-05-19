#include "types.h"

uint8_t read_bits_of_8bit_num(uint8_t num, int upper_bit, int lower_bit) {
	return ((uint8_t)(num << (7 - upper_bit))) >> (7 - upper_bit + lower_bit);
}