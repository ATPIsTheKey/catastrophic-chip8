/*******************************************************************************
 *
 * MIT License
 * Copyright (c) 2019 Roland Fuhrmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sysexits.h>
#include <sys/utsname.h>

#include <SDL2/SDL.h>

#include "src/vm.h"
#include "src/debug.h"
#include "libs/argtable3.h"


#define PROGNAME "catastrophic-chip8"
#define VERSION "1.0.0"

#define NSECPERSEC    1000000000
#define REGDECR_RATE  60 // rate in Hz at which timers should be decremented


static const int AUDIO_SAMPLE_RATE = 44100;
static int AUDIO_FREQ;
static int AUDIO_AMPLITUDE;


//> Used in timing chip8 cpu cycles and timer decrements
struct timespec
time_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;

    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return temp;
}


//> Draws a framebuffer
static void
draw_framebuffer(
        uint32_t *framebuffer, SDL_Texture *texture, SDL_Renderer *renderer)
{
    SDL_UpdateTexture(texture, NULL, framebuffer,
                      sizeof(framebuffer[0]) * CH8_VM_SCR_W);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


//> Callback function used to refill SDL audio buffer
void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*) raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int n_samples = (*(int*)user_data); // looks evil but works ...

    for (int i = 0; i < length; i++, n_samples++)
    {
        double time = (double)n_samples / (double) AUDIO_SAMPLE_RATE;
        buffer[i] = (Sint16)(AUDIO_AMPLITUDE * sin(2.0f * M_PI * (double) AUDIO_FREQ * time));
    }
}


//> Main emulation loop of chip8. Its pretty heavy on the CPU thread as th thread is
//  not put into sleep mode while timing chip8 CPU cycles and timer decrements.
static int
CH8_emulation_loop(
        const char *rom_fpath, uint32_t vm_opts, int32_t video_scale,
        size_t clock_freq, int audio_freq, int audio_ampl)
{
    int main_rc = 0; // return code to main loop
    int temp_rc = 0; // temporary variable to hold return code of any function

    /*** Set up SDL */

    temp_rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // todo: debug memory leaks
    if (temp_rc < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        main_rc = EX_TEMPFAIL;
        goto QUIT;
    }

    SDL_Window *window = SDL_CreateWindow(
            PROGNAME, 0, 0,
            CH8_VM_SCR_W * video_scale, CH8_VM_SCR_H * video_scale,
            SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(
            window, -1,
            SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            CH8_VM_SCR_W, CH8_VM_SCR_H);

    AUDIO_FREQ        = audio_freq;
    AUDIO_AMPLITUDE   = audio_ampl;

    int n_samples = 0;
    SDL_AudioSpec want_spec = {
            .freq     = AUDIO_SAMPLE_RATE,
            .format   = AUDIO_S16SYS,   // sample type (signed short i.e. 16 bit)
            .channels = 1,              // only one channel
            .samples  = 5000,           // buffer-size
            .callback = audio_callback, // function SDL calls periodically to refill the buffer
            .userdata = &n_samples      // counter keeping track of current sample number
    };

    SDL_AudioSpec have_spec;
    temp_rc = SDL_OpenAudio(&want_spec, &have_spec);
    if (temp_rc != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
        main_rc = EX_TEMPFAIL;
        goto QUIT;
    }

    if(want_spec.format != have_spec.format) {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec.");
        main_rc = EX_TEMPFAIL;
        goto QUIT;
    }

    /*** Beginning of emulation */

    CH8_VM *vm = CH8_VM_init(vm_opts);
    temp_rc = CH8_VM_load_rom(vm, rom_fpath);

    if (temp_rc == CH8_VM_ROMSIZE_OUTOFBOUNDS) {
        main_rc = EX_DATAERR;
        goto QUIT;
    } else if (temp_rc == CH8_VM_ROM_NOTFOUND) {
        main_rc = EX_NOINPUT;
        goto QUIT;
    }

    struct timespec t1_clockfreq = {.tv_sec = 0, .tv_nsec = 0};
    struct timespec t2_clockfreq = {.tv_sec = 0, .tv_nsec = 0};

    struct timespec t1_regdecr   = {.tv_sec = 0, .tv_nsec = 0};
    struct timespec t2_regdecr   = {.tv_sec = 0, .tv_nsec = 0};

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1_clockfreq);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1_regdecr);

    while (1)
    {
        if (vm->cpu->sound_timer > 0) { SDL_PauseAudio(0); } // ugly but it works
        else if (vm->cpu->sound_timer == 0) { SDL_PauseAudio(1); }

        if (time_diff(t1_regdecr, t2_regdecr).tv_nsec > NSECPERSEC / REGDECR_RATE)
        {
            CH8_VM_decrement_timers(vm);
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1_regdecr);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2_regdecr);

        if (time_diff(t1_clockfreq, t2_clockfreq).tv_nsec > NSECPERSEC / clock_freq)
        {
            temp_rc = CH8_VM_emulate_cycle(vm);
            if (temp_rc == CH8_VM_UNSUPPORTED_OPCODE) {
                main_rc = EX_SOFTWARE;
                goto QUIT;
            }

            if (CH8_VM_is_drawflag_set(vm))
            {
                draw_framebuffer(vm->framebuffer, texture, renderer);
                CH8_VM_unset_drawflag(vm);
            }

            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1_clockfreq);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2_clockfreq);

        temp_rc = CH8_VM_SDL_set_keys(vm);
        switch (temp_rc) {
            case CH8_VM_QUIT:
                if (vm->opt_flags & CH8_VM_VERBOSE_MODE)
                    CH8_VM_DBG_log(__func__, "quit\n");
                goto QUIT;

            case CH8_VM_RELOAD:
                CH8_VM_kill(vm);
                vm = CH8_VM_init(vm_opts);
                CH8_VM_load_rom(vm, rom_fpath);

                if (vm->opt_flags & CH8_VM_VERBOSE_MODE)
                    CH8_VM_DBG_log(__func__, "vm reloaded\n");
                break;

            case CH8_VM_CPU_DUMP:
                if (vm->opt_flags & CH8_VM_VERBOSE_MODE)
                    CH8_VM_DBG_output_cpu_dump(__func__, vm, "CPU dump requested\n");
                break;

            default:
                break;
        }
    }

    QUIT:
    CH8_VM_kill(vm);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_CloseAudio();

    SDL_Quit();

    return main_rc;
}


/*** Command line parsing **********************************************************/


struct arg_lit *help, *version, *verbose_mode, *original_mode;
struct arg_int *clockfreq, *vidscale, *beepfreq, *ampl;
struct arg_file *rom_fspec;
struct arg_end *end;

int
main(int argc, char **argv)
{
    int exitcode = 0;

    void *argtable[] = {
            help          = arg_litn("h", "help",
                    0, 1, "display this help and exit"),

            version       = arg_litn(NULL, "version",
                    0, 1, "display version info and exit"),

            rom_fspec     = arg_filen(NULL, NULL, "<file>",
                    1, 1, "rom to be loaded"),

            clockfreq     = arg_intn(NULL, "cpufreq", "<int>",
                    0, 1, "clock frequency of emulator in Hz (defaults to 700)"),

            vidscale      = arg_intn(NULL, "vidscale","<int>",
                    0, 1, "video scale (defaults to 10)"),

            beepfreq      = arg_intn(NULL, "audiofreq", "<int>",
                    0, 1, "frequency of single chip8 sound in Hz (defaults to 440)"),

            ampl          = arg_intn(NULL, "ampl","<int>",
                    0, 1, "amplitude of single chip8 sound (defaults to 20000)"),

            verbose_mode  = arg_litn("v", "verbose",
                    0, 1, "verbose mode of emulator"),

            original_mode = arg_litn(NULL, "original",
                    0, 1, "emulate with orignal instruction set"),

            end           = arg_end(20)
    };

    // set clock frequency default value to 700
    clockfreq->ival[0] = 700;

    // set video scale default value to 10
    vidscale->ival[0]  = 10;

    // set amplitude default value to 20000
    ampl->ival[0]      = 20000;

    // set frequency default value to 440 Hz
    beepfreq->ival[0]      = 440;

    int nerrors;
    nerrors = arg_parse(argc, argv, argtable);

    // special case: '--help' takes precedence over error reporting
    if (help->count > 0)
    {
        printf("Usage: %s", PROGNAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Options and arguments: \n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        exitcode = 0;
        goto EXIT;
    }

    // special case: '--version' takes precedence over error reporting
    if (version->count > 0)
    {
        struct utsname name;
        if(uname(&name)) exit(-1);
        printf("%s (%s@%s) "VERSION"\n", PROGNAME, name.sysname, name.release);
        printf("This is free software. There is NO warranty; not even for MERCHANTABILITY or "
               "FITNESS FOR A PARTICULAR PURPOSE.\n");
        goto EXIT;
    }

    if (nerrors > 0)
    {
        arg_print_errors(stdout, end, PROGNAME);
        printf("Try '%s --help' for more information.\n", PROGNAME);
        exitcode = 1;
        goto EXIT;
    }

    uint32_t opts = ((verbose_mode->count == 1) ? CH8_VM_VERBOSE_MODE : 0u) |
                    ((original_mode->count == 1) ? CH8_VM_ORIGINAL_IMPL : 0u);

    exitcode = CH8_emulation_loop(
            rom_fspec->filename[0],
            opts,
            vidscale->ival[0],
            clockfreq->ival[0],
            beepfreq->ival[0],
            ampl->ival[0]);

    EXIT:
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}
