#define _CRT_SECURE_NO_WARNINGS

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <stdbool.h>
#include "emulator.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define PIXEL_SCALE 4

int main(int argc, char* argv[]) {
    // SET UP EMULATOR
    EMULATOR hEmulator = NULL;
    hEmulator = emulator_create("02-blargg.gb");
    //hEmulator = emulator_create("test.gb");
    if (hEmulator == NULL) {
        SDL_Log("EMULATOR WAS NOT ABLE TO BE MADE");
        return 1;       
    }
    //
    for (int i = 0; i < 161058; i++) {
        //SDL_Log("I %d: ", i);
        emulator_run(hEmulator);
        // 224
    }
    //emulator_check_if_test_passed(hEmulator);;
    /*
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

    // Test framebuffer (just a grayscale gradient)
    uint32_t* framebuffer = (uint32_t*)SDL_malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    
    if (framebuffer == NULL) {
        SDL_Log("Unable to malloc space");
        return 1;
    }

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            uint8_t shade = (x * 255) / SCREEN_WIDTH;
            framebuffer[y * SCREEN_WIDTH + x] = (shade << 16) | (shade << 8) | shade;
        }
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60 FPS
    }

    SDL_free(framebuffer);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    */
    return 0;
}
