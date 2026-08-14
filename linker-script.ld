ENTRY(Reset_Handler)

MEMORY 
{
    FLASH (rx) : ORIGIN = 0x8000000,  LENGTH = 384K
    SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 100K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);
total_memory = LENGTH(SRAM);

SECTIONS
{
    kernel_base = .;
    .isr_vector :
    {
        *(.isr_vector)
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        *(.text)
        . = ALIGN(4);
    } > FLASH

    _sidata = LOADADDR(.data);
    .data :
    {
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    kernel_end = .;
}