#include <cmsis.h>
#include <syscalls.h>
#include <mpu.h>

extern uint32_t flash_size;
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
/* Fix this shit */
uint8_t memory_map_init(void)
{
    uint32_t seg_addr;
    uint32_t lefsz;

    if (mpu_init() < 0)
        return 0;

    lefsz = ((uint32_t)&total_memory) - (((uint32_t)&kernel_end) - SRAM_BASE);
    kernel_heap_size = (lefsz - 2*32) / 3;
    stack_size = user_heap_size = kernel_heap_size;
    if (kernel_stack_min > lefsz)
        return 0;
    
    kernel_heap_addr = (uint32_t)&kernel_end;
    kernel_heap_end = kernel_heap_addr + kernel_heap_size;

    // Align the addresses with 32 bytes
    seg_addr = make_address_align(kernel_heap_end, 0x1f);
    // mpu_create_segment(1, seg_addr, MPU_MIN_SEG_SIZE_LOG, 0, 0);
    seg_addr += MPU_MIN_SEG_SIZE;
    user_heap_addr = seg_addr;
    seg_addr += user_heap_size;
    user_heap_end = seg_addr;
    seg_addr = make_address_align(seg_addr, 0x1f);
    // mpu_create_segment(1, seg_addr, MPU_MIN_SEG_SIZE_LOG, 0, 0);
    seg_addr += MPU_MIN_SEG_SIZE;
    kernel_stack_end = seg_addr;
    return 1;
}

void *kalloc(uint32_t *size)
{
    uint32_t *tmp;

    *size = make_address_align(*size, 0x1f);
    if (kernel_heap_addr + *size >= kernel_heap_end)
        return 0;

    tmp = (uint32_t *)kernel_heap_addr;
    kernel_heap_addr += *size;
    return tmp;
}

void *ualloc(uint32_t *size)
{
    uint32_t *tmp;

    *size = make_address_align(*size, 0x1f);
    if (user_heap_addr + *size >= user_heap_end)
        return 0;
    tmp = (uint32_t *)user_heap_addr;
    user_heap_addr += *size;
    return tmp;
}

uint32_t *uproc_stack_allocate(uint8_t id ,uint32_t *size)
{
    uint32_t *tmp;
    uint32_t t;

    *size = make_address_align(*size, 0x02);
    if ((user_heap_addr % (*size)) && (*size) < (user_heap_end-user_heap_addr)) {
        t = make_address_align(user_heap_addr, *size);
        if (t >= user_heap_end)
            return NULL;
        user_heap_addr = t;
    }
    /* FIX THIS GARBAGE */
    if (!mpu_create_segment(id, user_heap_addr, 11, 0, 1))
        return NULL;
    tmp = (uint32_t *)user_heap_addr;
    user_heap_addr += *size;
    return tmp;
}