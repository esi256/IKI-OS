arm-none-eabi-gcc -I"include" -I"arch/include" -I"arch/cmsis" -I"user_procs" -I"lib/include" -g -mcpu=cortex-m4 -mthumb  -T linker-script.ld src/*.c src/*.s lib/*.s arch/*.c user_procs/*.c -nostdlib
