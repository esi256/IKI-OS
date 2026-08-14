#include <cmsis.h>
#include <syscalls.h>
#include <mpu.h>

extern uint32_t kernel_base;
extern uint32_t kernel_end;
extern uint32_t total_memory;
uint32_t stack_size;
uint32_t heap_size;
uint32_t kernel_heap_size;
uint32_t user_heap_size;
uint32_t kernel_heap_addr;
uint32_t user_heap_addr;
uint32_t kernel_heap_end;
uint32_t kernel_stack_end;
uint32_t user_heap_end;
uint32_t kernel_stack_min = 0x8000;

int get_prev_pow2size(int32_t arg)
{
    int32_t tmp[4];
    uint8_t idx = 0;
    uint32_t min = -1;

    for (uint32_t i = 8192; i <= 65536; i *= 2) {
        tmp[idx++] = arg - i;
    }
    for (uint32_t i = 0; i < 4; i++) {
            if (tmp[i] > 0 && tmp[i] < min) {
                min = tmp[i];
                idx = i;
            }
    }
    if (min == -1)
        return -1;
    else
        return (1 << idx+13);
}

uint32_t make_address_align(uint32_t addr, uint32_t alg)
{
    uint32_t t;

    return (addr + (alg-1)) & ~(alg-1);
}

/* There are 2 Rules for assiging addresses to MPU :
    1- address should be a power of 2
    2- addreess should be a multiple of it's size 
*/
int heap_region_allocate()
{
    uint32_t seg_addr;
    uint32_t lefsz;

    if (mpu_init() < 0)
        return -1;

    lefsz = ((uint32_t)&total_memory) - (((uint32_t)&kernel_end) - SRAM_BASE);
    kernel_heap_size = (lefsz - 2*32) / 3;
    user_heap_size = kernel_heap_size;
    stack_size = user_heap_size;
    if (kernel_stack_min > lefsz)
        return -1;
    
    kernel_heap_addr = (uint32_t)&kernel_end;
    kernel_heap_end = kernel_heap_addr + kernel_heap_size;

    // Align the addresses with 32 bytes
    seg_addr = make_address_align(kernel_heap_end, 0x1f);
    mpu_create_segment(1, seg_addr, 5, 0, 0);
    seg_addr += 32;
    user_heap_addr = seg_addr;
    seg_addr += user_heap_size;
    user_heap_end = seg_addr;
    seg_addr = make_address_align(seg_addr, 0x1f);
    mpu_create_segment(1, seg_addr, 5, 0, 0);
    seg_addr += 32;
    kernel_stack_end = seg_addr;
    return 0;
}

void *kalloc(uint32_t size)
{
    uint32_t *tmp;

    if (kernel_heap_addr+size >= kernel_heap_end)
        return 0;

    tmp = (uint32_t *)kernel_heap_addr;
    kernel_heap_addr += size;
    return tmp;
}

void *ualloc(uint32_t size)
{
    uint32_t *tmp;

    if (user_heap_addr + size > user_heap_end)
        return 0;
    tmp = (uint32_t *)user_heap_addr;
    user_heap_addr += size;
    return tmp;
}