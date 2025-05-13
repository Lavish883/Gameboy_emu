#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include "handle_files.h"

long size_of_file(FILE* pFile);

uint8_t* return_file_as_array(char* file_name, STATUS* status) {
	STATUS local_status = FAILURE;
	uint8_t* uint8_arry = NULL;

	if (file_name == NULL) {
		goto change_reported_status;
	}
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == NULL) {
		SDL_Log("File could not be opened make sure the file named %s exists", file_name);
		goto change_reported_status;
	}
	
	long file_size = size_of_file(pFile); // in number of bytes
	if (file_size <= 0) {
		SDL_Log("Invalid file size: %ld", file_size);
		goto change_reported_status;
	}

	uint8_arry = (uint8_t*)SDL_malloc(sizeof(uint8_t) * file_size);
	if (uint8_arry == NULL) {
		goto change_reported_status;
	}

	size_t num_of_elms_read = fread(uint8_arry, sizeof(uint8_t), file_size, pFile);
	if (num_of_elms_read != file_size) {
		SDL_Log("Not everything in the rom was read uh oh");
		SDL_free(uint8_arry);
		goto change_reported_status;
	}
	
	fclose(pFile);
	local_status = SUCCESS;
	change_reported_status:
		if (status != NULL){
			*status = local_status;
		}
	
	return uint8_arry;
}
// Pre-condtion that pFile is not null
long size_of_file(FILE* pFile) {
	long current_pos = ftell(pFile);
	fseek(pFile, 0L, SEEK_END);
	long size = ftell(pFile);
	fseek(pFile,current_pos, SEEK_SET);
	return size;
}