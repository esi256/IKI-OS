arm-none-eabi-gcc -I"include" -I"arch/include" -I"user_procs" -g -mcpu=cortex-m4 -mthumb  -T linker-script.ld src/*.c src/*.s  arch/*.c user_procs/*.c -nostdlib
