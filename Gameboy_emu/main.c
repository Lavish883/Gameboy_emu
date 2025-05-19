#define _CRT_SECURE_NO_WARNINGS

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdbool.h>
#include "emulator.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define PIXEL_SCALE 7


int main(int argc, char* argv[]) {
    // SET UP EMULATOR
    EMULATOR hEmulator = NULL;
    hEmulator = emulator_create("drMario.gb");
    //hEmulator = emulator_create("test.gb");
    if (hEmulator == NULL) {
        SDL_Log("EMULATOR WAS NOT ABLE TO BE MADE");
        return 1;       
    }
    //
    for (int i = 0; i < 100000; i++) {
        //SDL_Log("I %d: ", i);
        emulator_run(hEmulator);
        // 224
    }
    //emulator_check_if_test_passed(hEmulator);;
    
    // SDL SETUP
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init fai2led: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game Boy Emulator",
        SCREEN_WIDTH * PIXEL_SCALE,
        SCREEN_HEIGHT * PIXEL_SCALE,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);
   
    if (!texture) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    uint32_t* framebuffer = emulator_get_frame_buffer(hEmulator);
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        framebuffer = emulator_get_frame_buffer(hEmulator);

        SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60 FPS
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
