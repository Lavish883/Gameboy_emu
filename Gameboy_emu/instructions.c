#define _CRT_SECURE_NO_WARNINGS

#include "instructions.h"
#include <string.h>
#include <stdio.h>

char* condition_code_table[4] = { "NZ","Z","NC", "C" };
char* r16_group1_table[4] = { "BC", "DE", "HL", "SP" };
char* r16_group2_table[4] = { "BC","DE","HL+","HL-" };
char* r16_group3_table[4] = { "BC","DE","HL","AF" };
char* r8_table[8] = {"B","C","D","E","H","L","[HL]", "A"};
char* opcode_group_1_table[8] = {"RLCA", "RRCA", "RLA","RRA", "DAA", "CPL", "SCF", "CCF"};
char* opcode_group_2_table[8] = { "ADD","ADC","SUB","SBC","AND","XOR","OR","CP" };
char* opcode_group_3_table[8] = { "RLC","RRC","RL", "RR", "SLA","SRA","SWAP","SRL" };

char* misc_opcode_group_table[4] = { "RET","RETI","JP HL", "LD SP, HL" };

// inclusive on both ends
uint8_t read_bits_of_opcode(uint8_t opcode, int upper_bit, int lower_bit) {
	//76543210 8bit num and how indivudal bits are numbered
	return ((uint8_t)(opcode << (7 - upper_bit))) >> (7 - upper_bit + lower_bit);
}

char* opcode_deconstructor(uint8_t opcode, char* name, int name_length) {
    switch (opcode) {  
		case 0x00: return strncpy(name, "NOP", name_length);  
		case 0x08: return strncpy(name, "LD [a16], SP", name_length);  
		case 0x10: return strncpy(name, "STOP n8", name_length);  
		case 0x18: return strncpy(name, "JR e8", name_length);  
		case 0x76: return strncpy(name, "HALT", name_length);  
		case 0xE0: return strncpy(name, "LDH [a8], A", name_length);  
		case 0xE8: return strncpy(name, "ADD SP, e8", name_length);  
		case 0xF0: return strncpy(name, "LDH A, [a8]", name_length);  
		case 0xF8: return strncpy(name, "LD HL, SP + e8", name_length);  
		case 0xE2: return strncpy(name, "LDH [C], A", name_length);  
		case 0xEA: return strncpy(name, "LD [a16], A", name_length);  
		case 0xF2: return strncpy(name, "LDH A, [C]", name_length);  
		case 0xFA: return strncpy(name, "LD A, [a16]", name_length);  
		case 0xC3: return strncpy(name, "JP a16", name_length);  
		case 0xCB: return strncpy(name, "CB", name_length);  
		case 0xF3: return strncpy(name, "DI", name_length);  
		case 0xFB: return strncpy(name, "EI", name_length);  
		case 0xCD: return strncpy(name, "CALL a16", name_length);
		case 0xE4: return strncpy(name, "ILLEGAL", name_length);
		case 0xEC: return strncpy(name, "ILLEGAL", name_length);
		case 0xF4: return strncpy(name, "ILLEGAL", name_length);
		case 0xFC: return strncpy(name, "ILLEGAL", name_length);
		default: break;  
    }

	int first_2_bits_of_opcode = read_bits_of_opcode(opcode, 7, 6);
	switch (first_2_bits_of_opcode){
		// cases should return so no need for a break
		case(0x0): {
			// JR Conditional check
			if (read_bits_of_opcode(opcode, 7, 5) == 0x01 &&
				read_bits_of_opcode(opcode, 2, 0) == 0x0
				) {
				sprintf_s(name, sizeof(char) * name_length, "JR %s, e8", condition_code_table[read_bits_of_opcode(opcode, 4, 3)]);
				return name;
			}
			if (read_bits_of_opcode(opcode, 2, 2) == 0x0) {
				// Check last 4 bits op opcode to see a pattern
				int last_4_bits_of_opcode = read_bits_of_opcode(opcode, 3, 0);
				switch (last_4_bits_of_opcode) {
				case(0x01): { // LD r16 n16
					sprintf_s(name, sizeof(char) * name_length, "LD %s, n16", r16_group1_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				case(0x09): { // ADD HL,r16
					sprintf_s(name, sizeof(char) * name_length, "ADD HL, %s", r16_group1_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				case(0x02): { // LD (r16),A
					sprintf_s(name, sizeof(char) * name_length, "LD [%s], A", r16_group2_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				case(0x0A): { // LD A,(r16)
					sprintf_s(name, sizeof(char) * name_length, "LD A, [%s]", r16_group2_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				case(0x03): { // INC r16
					sprintf_s(name, sizeof(char) * name_length, "INC %s", r16_group1_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				case(0x0B): { // DEC r16
					sprintf_s(name, sizeof(char) * name_length, "DEC %s", r16_group1_table[read_bits_of_opcode(opcode, 5, 4)]);
					break;
				}
				default:
					break;
				}
				return name;
			}
			else {
				// Check last 3 bits to see if there is a pattern
				int last_3_bits_of_opcode = read_bits_of_opcode(opcode, 2, 0);
				switch (last_3_bits_of_opcode) {
				case(0x04): { //INC r8
					sprintf_s(name, sizeof(char) * name_length, "INC %s", r8_table[read_bits_of_opcode(opcode, 5, 3)]);
					break;
				}
				case(0x05): { // DEC r8
					sprintf_s(name, sizeof(char) * name_length, "DEC %s", r8_table[read_bits_of_opcode(opcode, 5, 3)]);
					break;
				}
				case(0x06): { // LD r8, n8
					sprintf_s(name, sizeof(char) * name_length, "LD %s, n8", r8_table[read_bits_of_opcode(opcode, 5, 3)]);
					break;
				}
				case(0x07): {
					sprintf_s(name, sizeof(char) * name_length, "%s", opcode_group_1_table[read_bits_of_opcode(opcode, 5, 3)]);
				}
				default:
					break;
				}
				return name;
			}
			break;
		}
		case(0x01): {
			char* r8_dest = r8_table[read_bits_of_opcode(opcode, 5, 3)];
			char* r8_source = r8_table[read_bits_of_opcode(opcode, 2, 0)];
			// LD r8, r8
			sprintf_s(name, sizeof(char) * name_length, "LD %s, %s", r8_dest, r8_source);
			return name;
		}
		case(0x02): {
			char* opcode_group = opcode_group_2_table[read_bits_of_opcode(opcode, 5, 3)];
			char* r8_operand_2 = r8_table[read_bits_of_opcode(opcode, 2, 0)];
			// LD r8, r8
			sprintf_s(name, sizeof(char) * name_length, "%s A, %s", opcode_group, r8_operand_2);
			return name;
		}
		case(0x03): {
			int first_3_bits_of_opcode = read_bits_of_opcode(opcode, 7, 5);			
			int last_bit_of_opcode = read_bits_of_opcode(opcode, 0, 0);

			switch (last_bit_of_opcode){
				case(0x0): {
					int last_3_bits_of_opcode = read_bits_of_opcode(opcode, 2, 0);
					switch (last_3_bits_of_opcode){
						case(0x0): {
							sprintf_s(name, sizeof(char) * name_length, "RET %s", condition_code_table[read_bits_of_opcode(opcode, 4, 3)]);
							return name;
						}
						case(0x02): {
							sprintf_s(name, sizeof(char) * name_length, "JP %s, a16", condition_code_table[read_bits_of_opcode(opcode, 4, 3)]);
							return name;
						}
						case(0x04): {
							sprintf_s(name, sizeof(char) * name_length, "CALL %s, a16", condition_code_table[read_bits_of_opcode(opcode, 4, 3)]);
							return name;
						}
						case(0x06): {
							sprintf_s(name, sizeof(char)* name_length, "%s A, n8", opcode_group_2_table[read_bits_of_opcode(opcode, 5, 3)]);
							return name;
						}
						default:
							break;
					}
				}
				case(0x01): {
					int last_4_bits_of_opcode = read_bits_of_opcode(opcode, 3, 0);
					switch (last_4_bits_of_opcode){
						case(0x01): { //POP r16
							sprintf_s(name, sizeof(char) * name_length, "POP %s", r16_group3_table[read_bits_of_opcode(opcode, 5, 4)]);
							return name;
						}
						case(0x09): {
							sprintf_s(name, sizeof(char)* name_length, "%s", misc_opcode_group_table[read_bits_of_opcode(opcode, 5, 4)]);
							return name;
						}
						case(0x05): {
							sprintf_s(name, sizeof(char)* name_length, "PUSH %s", r16_group3_table[read_bits_of_opcode(opcode, 5, 4)]);
							return name;
						}
						default:
							break;
					}
					// meaning RST instruction or ILLEGAL
					if (read_bits_of_opcode(opcode, 2, 0) == 7) {
						sprintf_s(name, sizeof(char) * name_length, "RST $%.2x", (uint8_t)read_bits_of_opcode(opcode, 5, 3) << 3);
						return name;
					}
				}
			default:
				break;
			}
		}
		default:
			break;
	}
	return strncpy(name, "ILLEGAL", name_length);
}	

char* opcode_cb_deconstructor(uint8_t opcode, char* name, int name_length) {
	int first_2_bits_of_opcode = read_bits_of_opcode(opcode, 7, 6);
	switch (first_2_bits_of_opcode) {
		case(0x0): {
			sprintf_s(name, sizeof(char) * name_length, "%s %s", opcode_group_3_table[read_bits_of_opcode(opcode, 5, 3)], r8_table[read_bits_of_opcode(opcode, 2, 0)]);
			break;
		}
		case(0x01): {
			sprintf_s(name, sizeof(char) * name_length, "BIT %d, %s", read_bits_of_opcode(opcode, 5, 3), r8_table[read_bits_of_opcode(opcode, 2, 0)]);
			break;
		}
		case(0x02): {
			sprintf_s(name, sizeof(char) * name_length, "RES %d, %s", read_bits_of_opcode(opcode, 5, 3), r8_table[read_bits_of_opcode(opcode, 2, 0)]);
			break;
		}
		case(0x03): {
			sprintf_s(name, sizeof(char) * name_length, "SET %d, %s", read_bits_of_opcode(opcode, 5, 3), r8_table[read_bits_of_opcode(opcode, 2, 0)]);
			break;
		}
		default: break;
	}
	return name;
}