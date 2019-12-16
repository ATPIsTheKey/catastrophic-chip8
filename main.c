#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "catastrophic-chip8/chip8.h"

// clock speed should be chosen proportionally to 60 Hz
#define CLOCK_SPEED 1

// Small test program to make sure SDL2 library is working
void
test_sdl(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
            "SDL2Test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    test_sdl();
    return 0;
}