#include "ppu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

struct ppu {
	unsigned int t_cycles_count;
	MEMORY hMemory;
};

uint32_t colors[4] = { 
	0xFF000000 | (255 << 16) | (255 << 8) | 255, // White
	0xFF000000 | (169 << 16) | (169 << 8) | 169, // ligt gray
	0xFF000000 | (84 << 16) | (84 << 8) | 84, // dark gray
	0xFF000000 | (0 << 16) | (0 << 8) | 0 // black
};

uint8_t tile[16] = {
	0xFF,
	0x00,
	0x7E,
	0xFF,
	0x85,
	0x81,
	0x89,
	0x83,
	0x93,
	0x85,
	0xA5,
	0x8B,
	0xC9,
	0x97,
	0x7E,
	0xFF
};
uint8_t tile_2[16] = {
	0x3C, 
	0x7E, 
	0x42, 
	0x42, 
	0x42, 
	0x42, 
	0x42, 
	0x42, 
	0x7E, 
	0x5E, 
	0x7E, 
	0x0A, 
	0x7C, 
	0x56, 
	0x38, 
	0x7C,
};

typedef struct ppu Ppu;

void handle_LY_register_update(Ppu* pPPU);

PPU ppu_create(void) {
	// Not gonnaa do pPpu looks odd as hell so gonna do pPPU
	Ppu* pPPU = (Ppu*)SDL_malloc(sizeof(Ppu));

	if (pPPU != NULL) {
		pPPU->t_cycles_count = 0;
	}

	return pPPU;
}

void ppu_execute(PPU hPPU, int t_cycles_took) {
	Ppu* pPPU = (Ppu*)hPPU;
	pPPU->t_cycles_count += t_cycles_took;
	handle_LY_register_update(pPPU);
}

void handle_LY_register_update(Ppu* pPPU) {
	if (pPPU->t_cycles_count >= 456) {
		uint8_t LY = memory_read(pPPU->hMemory, 0xFF44);
		if (LY == 153) {
			memory_set(pPPU->hMemory, 0xFF44, 0);
		}
		else {
			memory_set(pPPU->hMemory, 0xFF44, LY + 1);
		}
		
		if (LY + 1 == 144) { // SET Interrupt
			memory_set(pPPU->hMemory, 0xFF0F, memory_read(pPPU->hMemory, 0xFF0F) | 0x01);
		}
		pPPU->t_cycles_count -= 456;
	}
}

void ppu_destroy(PPU* phPPU) {
	SDL_free(*phPPU);
	*phPPU = NULL;
}

void ppu_fetch_all_tiles(PPU hPPU, uint32_t* frame_buffer, int size) {
	// Each tile occupies 16 bytes, 8*8 64 pixels per tile
	Ppu* pPPU = (Ppu*)hPPU;

	uint8_t bg_color_palette_info = memory_read(pPPU->hMemory, 0xFF47); // bg and window palette info
	uint32_t bg_color_palette[4] = {
		colors[read_bits_of_8bit_num(bg_color_palette_info, 1, 0)], // ID 0
		colors[read_bits_of_8bit_num(bg_color_palette_info, 3, 2)], // ID 1
		colors[read_bits_of_8bit_num(bg_color_palette_info, 5, 4)], // ID 2
		colors[read_bits_of_8bit_num(bg_color_palette_info, 7, 6)]  // ID 3
	};

	// each row takes 2 bytes, each tile takes 16 bytes, each pixel takes 2 bits
	//uint8_t tile[16];
	// 16 * k for next tile
	// (c + (o * 8)) to move to the right
	// r + 8 to move one down

	uint8_t pixels[64];

	for (int p = 0; p < 360; p++) {
		//// get all tile bytes
		for (int i = 0; i < 16; i++) {
			tile[i] = memory_read(pPPU->hMemory, (0x8000 + i) + 16 * p);
		}

		for (int r = 0; r < 8; r++) {
				uint8_t high_byte = tile[2 * r + 1];
				uint8_t low_byte = tile[2 * r];

				for (int c = 0; c < 8; c++) { // Get the pixel data
					uint8_t high_bit = read_bits_of_8bit_num(high_byte, 7 - c, 7 - c);
					uint8_t low_bit = read_bits_of_8bit_num(low_byte, 7 - c, 7 - c);

					pixels[r * 8 + c] = (high_bit << 1) + low_bit;
				}
			}

		int o = p % 20;
		int k = p / 20;

		// display one tile hopefully, set the framebuffer
		for (int r = 0; r < 8; r++) { // r is the row
			for (int c = 0; c < 8; c++) { // c is the col
				frame_buffer[(r + k * 8) * 160 + c + o * 8] = colors[pixels[r * 8 + c]];
			}
		}
	}
}

void ppu_connect_memory(PPU hPPU, MEMORY hMemory) {
	Ppu* pPPU = (Ppu*)hPPU;
	pPPU->hMemory = hMemory;
	// Set some registers now
	memory_set(hMemory, 0xFF44, 0);
}