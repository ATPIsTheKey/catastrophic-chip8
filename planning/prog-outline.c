// File outside of project target!!

// Outline of main emulation loop
int main(int argc, char *argv[])
{
    setup_graphics()
    setup_input()

    CHIP8 *vm = CHIP8_init_vm(CLOCKSPEED);

    for(; <some signal to quit>; )
    {
       CHIP8_emulate_cycle(*vm);

       if (CHIP8_is_drawflag_set())
          draw_framebuffer(vm->framebuffer);

       CHIP8_set_keys();
    }

    CHIP8_kill_vm();
    return 0;
}