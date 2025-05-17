#define _CRT_SECURE_NO_WARNINGS

#include "instructions.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef void(*opcode_group3_fp)(CPU, int,long*);
typedef void(*opcode_group2_fp)(CPU, int, bool, long*);

char* condition_code_table[4] = { "NZ","Z","NC", "C" };
char* r16_group1_table[4] = { "BC", "DE", "HL", "SP" };

char* r16_group2_table[4] = { "BC","DE","HL+","HL-" };
char* r16_group3_table[4] = { "BC","DE","HL","AF" };
char* opcode_group_1_table[8] = {"RLCA", "RRCA", "RLA","RRA", "DAA", "CPL", "SCF", "CCF"};

char* opcode_group_2_table[8] = { "ADD","ADC","SUB","SBC","AND","XOR","OR","CP" };
opcode_group2_fp opcode_group2_all_fp[8] = {
	&r8_ADD_r8,
	&r8_ADC_r8,
	&r8_SUB_r8,
	&r8_SBC_r8,
	&r8_AND_r8,
	&r8_XOR_r8,
	&r8_OR_r8,
	&r8_CP_r8
};

char* opcode_group_3_table[8] = { "RLC","RRC","RL", "RR", "SLA","SRA","SWAP","SRL" };
opcode_group3_fp opcode_group3_all_fp[8] = {
	NULL,
	NULL,
	NULL,
	&RR_r8,
	NULL,
	NULL,
	&SWAP_r8,
	&SRL_r8,
};

char* misc_opcode_group_table[4] = { "RET","RETI","JP HL", "LD SP, HL" };

// inclusive on both ends
uint8_t read_bits_of_opcode(uint8_t opcode, int upper_bit, int lower_bit) {
	//76543210 8bit num and how indivudal bits are numbered
	return ((uint8_t)(opcode << (7 - upper_bit))) >> (7 - upper_bit + lower_bit);
}

char* opcode_deconstructor_and_run(CPU hCpu, uint8_t opcode, char* name, int name_length, long* pT_cycles_count, int* exectued) {
    switch (opcode) {  
		case 0x00: {
			*exectued = 1;
			*pT_cycles_count += 4;
			return strncpy(name, "NOP", name_length);
		}
		case 0x08: {
			*exectued = 1;
			load_SP_to_immediate16(hCpu, pT_cycles_count);
			return strncpy(name, "LD [a16], SP", name_length);
		}
		case 0x10: return strncpy(name, "STOP n8", name_length);  
		case 0x1F: {
			*exectued = 1;
			RRA(hCpu, pT_cycles_count);
			return strncpy(name, "RRA", name_length);
		}
		case 0x18: {
			*exectued = 1;
			uncond_jump_immediate_signed8(hCpu, pT_cycles_count);
			return strncpy(name, "JR e8", name_length);
		}
		case 0x76: return strncpy(name, "HALT", name_length);  
		case 0xE0: {
			*exectued = 1;
			ldh_8bit_data_to_immediate8_from_A(hCpu, pT_cycles_count);
			return strncpy(name, "LDH [a8], A", name_length);
		}
		case 0xE8: return strncpy(name, "ADD SP, e8", name_length);  
		case 0xF0: { 
			*exectued = 1;
			ldh_8bit_data_from_immediate_to_A(hCpu, pT_cycles_count);
			return strncpy(name, "LDH A, [a8]", name_length); 
		}
		case 0xF8: return strncpy(name, "LD HL, SP + e8", name_length);  
		case 0xE2: return strncpy(name, "LDH [C], A", name_length);  
		case 0xEA: { 
			*exectued = 1;
			load_8bit_data_to_immediate16_addr_from_A(hCpu, pT_cycles_count);
			return strncpy(name, "LD [a16], A", name_length);
		}
		case 0xF2: return strncpy(name, "LDH A, [C]", name_length);  
		case 0xFA: {
			*exectued = 1;
			load_8bit_data_to_A_from_immediate16_addr(hCpu, pT_cycles_count);
			return strncpy(name, "LD A, [a16]", name_length);
		}
		case 0xC3: { 
			*exectued = 1;
			uncond_jump_immediate16(hCpu, pT_cycles_count);
			return strncpy(name, "JP a16", name_length); 
		}
		case 0xD9: {
			*exectued = 1;
			uncond_return(hCpu, pT_cycles_count);
			enable_interrupt_master(hCpu);
			return strncpy(name, "RETI", name_length);
		}
		case 0xC9: {
			*exectued = 1;
			uncond_return(hCpu, pT_cycles_count);
			return strncpy(name, "RET", name_length);
		}
		case 0xCB: { 
			return opcode_cb_deconstructor(hCpu, read_immediate_mem_for_instructions(hCpu), name, name_length, pT_cycles_count, exectued);
		}
		case 0xF3: { 
			*exectued = 1;
			disable_interrupt_master(hCpu);
			return strncpy(name, "DI", name_length); 
		}
		case 0xFB: { 
			*exectued = 1;
			enable_interrupt_master(hCpu);
			return strncpy(name, "EI", name_length);
		}
		case 0xCD: { 
			*exectued = 1;
			uncond_call(hCpu, pT_cycles_count);
			return strncpy(name, "CALL a16", name_length); 
		}
		case 0xE9: {
			*exectued = 1;
			jump_to_addr_HL(hCpu, pT_cycles_count);
			return strncpy(name, "JP HL", name_length);
		}
		case 0xF9: {
			*exectued = 1;
			load_SP_from_HL(hCpu, pT_cycles_count);
			return strncpy(name, "LD SP, HL", name_length);
		}
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
				int info = read_bits_of_opcode(opcode, 4, 3);
				sprintf_s(name, sizeof(char) * name_length, "JR %s, e8", condition_code_table[info]);
				cond_jump_immediate_signed8(hCpu, info, pT_cycles_count);
				*exectued = 1;
				return name;
			}
			if (read_bits_of_opcode(opcode, 2, 2) == 0x0) {
				// Check last 4 bits op opcode to see a pattern
				int last_4_bits_of_opcode = read_bits_of_opcode(opcode, 3, 0);
				int info = read_bits_of_opcode(opcode, 5, 4);
				switch (last_4_bits_of_opcode) {
				case(0x01): { // LD r16 n16
					sprintf_s(name, sizeof(char) * name_length, "LD %s, n16", r16_group1_table[info]);
					load_16bit_reg_to_immediate16(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x09): { // ADD HL,r16
					*exectued = 1;
					add_r16_to_HL(hCpu, info, pT_cycles_count);
					sprintf_s(name, sizeof(char) * name_length, "ADD HL, %s", r16_group1_table[info]);
					break;
				}
				case(0x02): { // LD (r16),A
					sprintf_s(name, sizeof(char) * name_length, "LD [%s], A", r16_group2_table[info]);
					load_8bit_data_from_A_to_mem_addr(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x0A): { // LD A,(r16)
					sprintf_s(name, sizeof(char) * name_length, "LD A, [%s]", r16_group2_table[info]);
					load_8bit_data_from_mem_addr_to_A(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x03): { // INC r16
					sprintf_s(name, sizeof(char) * name_length, "INC %s", r16_group1_table[info]);
					increment_r16(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x0B): { // DEC r16
					sprintf_s(name, sizeof(char) * name_length, "DEC %s", r16_group1_table[info]);
					decrement_r16(hCpu, info, pT_cycles_count);
					*exectued = 1;
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
				int info = read_bits_of_opcode(opcode, 5, 3);
				switch (last_3_bits_of_opcode) {
				case(0x04): { //INC r8
					sprintf_s(name, sizeof(char) * name_length, "INC %s", r8_table[info]);
					increment_r8(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x05): { // DEC r8
					sprintf_s(name, sizeof(char) * name_length, "DEC %s", r8_table[info]);
					decrement_r8(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x06): { // LD r8, n8
					sprintf_s(name, sizeof(char) * name_length, "LD %s, n8", r8_table[info]);
					load_8bit_data_from_immediate8_to_reg(hCpu, info, pT_cycles_count);
					*exectued = 1;
					break;
				}
				case(0x07): {
					sprintf_s(name, sizeof(char) * name_length, "%s", opcode_group_1_table[info]);
				}
				default:
					break;
				}
				return name;
			}
			break;
		}
		case(0x01): {
			int r8_dest = read_bits_of_opcode(opcode, 5, 3);
			int r8_source = read_bits_of_opcode(opcode, 2, 0);
			// LD r8, r8
			sprintf_s(name, sizeof(char) * name_length, "LD %s, %s", r8_table[r8_dest], r8_table[r8_source]);
			load_8bit_reg_to_8bit_reg(get_8_bit_reg_addr(hCpu, r8_dest), get_8_bit_reg_addr(hCpu, r8_source), pT_cycles_count);
			*exectued = 1;
			return name;
		}
		case(0x02): {
			int group_info = read_bits_of_opcode(opcode, 5, 3);
			int operand_info = read_bits_of_opcode(opcode, 2, 0);

			char* opcode_group = opcode_group_2_table[group_info];
			char* r8_operand_2 = r8_table[operand_info];

			if (opcode_group2_all_fp[group_info] != NULL) {
				*exectued = 1;
				opcode_group2_all_fp[group_info](hCpu, operand_info, true, pT_cycles_count);
			}

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
					int info = read_bits_of_opcode(opcode, 4, 3);
					switch (last_3_bits_of_opcode){
						case(0x0): {
							*exectued = 1;
							cond_return(hCpu, info, pT_cycles_count);
							sprintf_s(name, sizeof(char) * name_length, "RET %s", condition_code_table[info]);
							return name;
						}
						case(0x02): {
							*exectued = 1;
							cond_jump_immediate16(hCpu, info, pT_cycles_count);
							sprintf_s(name, sizeof(char) * name_length, "JP %s, a16", condition_code_table[info]);
							return name;
						}
						case(0x04): {
							*exectued = 1;
							cond_call(hCpu, info, pT_cycles_count);
							sprintf_s(name, sizeof(char) * name_length, "CALL %s, a16", condition_code_table[info]);
							return name;
						}
						case(0x06): {
							int group_info = read_bits_of_opcode(opcode, 5, 3);
							if (opcode_group2_all_fp[group_info] != NULL) {
								*exectued = 1;
								opcode_group2_all_fp[group_info](hCpu, -1, false, pT_cycles_count);
							}
							sprintf_s(name, sizeof(char)* name_length, "%s A, n8", opcode_group_2_table[group_info]);
							return name;
						}
						default:
							break;
					}
				}
				case(0x01): {
					int last_4_bits_of_opcode = read_bits_of_opcode(opcode, 3, 0);
					int info = read_bits_of_opcode(opcode, 5, 4);
					switch (last_4_bits_of_opcode){
						case(0x01): { //POP r16
							*exectued = 1;
							pop_from_stack(hCpu, info, pT_cycles_count);
							sprintf_s(name, sizeof(char) * name_length, "POP %s", r16_group3_table[info]);
							return name;
						}
						case(0x09): {
							sprintf_s(name, sizeof(char)* name_length, "%s", misc_opcode_group_table[info]);
							return name;
						}
						case(0x05): { // PUSH r16
							*exectued = 1;
							push_on_stack(hCpu, info, pT_cycles_count);
							sprintf_s(name, sizeof(char)* name_length, "PUSH %s", r16_group3_table[info]);
							return name;
						}
						default:
							break;
					}
					// meaning RST instruction or ILLEGAL
					if (read_bits_of_opcode(opcode, 2, 0) == 7) {
						info = read_bits_of_opcode(opcode, 5, 3);
						*exectued = 1;
						RST_to_addr(hCpu, (uint8_t)info << 3, pT_cycles_count);
						sprintf_s(name, sizeof(char) * name_length, "RST $%.2x", (uint8_t)info << 3);
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

char* opcode_cb_deconstructor(CPU hCpu, uint8_t opcode, char* name, int name_length, long* pT_cycles_count, int* exectued) {
	int first_2_bits_of_opcode = read_bits_of_opcode(opcode, 7, 6);
	int input_1 = read_bits_of_opcode(opcode, 5, 3);
	int input_2 = read_bits_of_opcode(opcode, 2, 0);

	switch (first_2_bits_of_opcode) {


		case(0x0): {
			if (opcode_group3_all_fp[input_1] != NULL) {
				*exectued = 1;
				opcode_group3_all_fp[input_1](hCpu, input_2, pT_cycles_count);
			}

			sprintf_s(name, sizeof(char) * name_length, "%s %s", opcode_group_3_table[input_1], r8_table[input_2]);
			break;
		}
		case(0x01): {
			sprintf_s(name, sizeof(char) * name_length, "BIT %d, %s", input_1, r8_table[input_2]);
			break;
		}
		case(0x02): {
			sprintf_s(name, sizeof(char) * name_length, "RES %d, %s", input_1, r8_table[input_2]);
			break;
		}
		case(0x03): {
			sprintf_s(name, sizeof(char) * name_length, "SET %d, %s", input_1, r8_table[input_2]);
			break;
		}
		default: break;
	}
	return name;
}

uint8_t test(CPU hCpu) {
	return *(get_8_bit_reg_addr(hCpu, 7));
}