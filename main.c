#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "catastrophic-chip8/vm.h"

#define ROM_PATH "/home/roland/CLionProjects/catastrophic-chip8/roms/"


/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void
quit(int rc)
{
    SDL_Quit();
    exit(rc);
}


int
main(int argc, char *argv[])
{
    SDL_Window *window;

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    /* Create the window */
    window = SDL_CreateWindow("Keypad input test",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              640, 480, 0);

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't create 640x480 window: %s\n", SDL_GetError());
        quit(2);
    }

    SDL_PumpEvents();

    /* Beginning of emulation ******************************************************/

    CH8_VM *vm = CH8VM_init(500);

    while (CH8VM_SDL_set_keys(vm) >= 0);

    CH8VM_kill(vm);

    /* End of emulation ************************************************************/

    SDL_Quit();
    return 0;
}
