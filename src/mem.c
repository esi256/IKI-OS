#include <cmsis.h>
#include <syscalls.h>
#include <mpu.h>
#include <kernel_error.h>
#include <bit_ops.h>

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
enum kernel_error memory_map_init(void)
{
    uint32_t seg_addr;
    uint32_t lefsz;
    enum kernel_error err;

    err = mpu_init();
    if (err != ERR_OK)
        return err;

    lefsz = ((uint32_t)&total_memory) - (((uint32_t)&kernel_end) - SRAM_BASE);
    kernel_heap_size = (lefsz - 2*32) / 3;
    stack_size = user_heap_size = kernel_heap_size;
    if (kernel_stack_min > lefsz)
        return ERR_MEM_NO_SPACE_FOR_KERNEL;
    
    kernel_heap_addr = (uint32_t)&kernel_end;
    kernel_heap_end = kernel_heap_addr + kernel_heap_size;

    seg_addr = make_address_align(kernel_heap_end, 32);
    struct mpu_op seg_ops = {
        .addr = seg_addr,
        .nseg = 1,
        .size_in_log2 = MPU_MIN_SEG_SIZE_LOG,
        .mem_type = MPU_MEM_STRONGLY_ORDERED,
        .exe_permission = 0,
        .ap_permission = MPU_AP_NO_ACCESS
    };
    if ((err = mpu_create_segment(&seg_ops)) != ERR_OK)
        return err;
    seg_addr += MPU_MIN_SEG_SIZE;
    user_heap_addr = seg_addr;
    seg_addr += user_heap_size;
    user_heap_end = seg_addr;
    seg_addr = make_address_align(seg_addr, 32);
    
    seg_ops.nseg = 2;
    seg_ops.addr = seg_addr;
    if ((err = mpu_create_segment(&seg_ops)) != ERR_OK)
        return err;
    seg_addr += MPU_MIN_SEG_SIZE;
    kernel_stack_end = seg_addr;
    return ERR_OK;
}

void *kalloc(uint32_t *size)
{
    uint32_t *tmp;

    *size = make_address_align(*size, 32);
    if (kernel_heap_addr + *size >= kernel_heap_end)
        return NULL;

    tmp = (uint32_t *)kernel_heap_addr;
    kernel_heap_addr += *size;
    return tmp;
}

void *ualloc(uint32_t *size)
{
    uint32_t *tmp;

    *size = make_address_align(*size, 32);
    if (user_heap_addr + *size >= user_heap_end)
        return NULL;
    tmp = (uint32_t *)user_heap_addr;
    user_heap_addr += *size;
    return tmp;
}

uint32_t *uproc_stack_allocate(uint8_t id ,uint32_t *size)
{
    uint32_t *tmp;
    uint32_t t, t1 = user_heap_addr, sizet = *size;

    sizet = round_up_pow2(*size);
    if (!sizet)
        return NULL;
    if ((user_heap_addr % (sizet))) {
        t = make_address_align(user_heap_addr, sizet);
        if (t+sizet > user_heap_end)
            return NULL;
        user_heap_addr = t;
    }

    struct mpu_op up_stack_ops = {
        .addr = user_heap_addr,
        .nseg = id,
        .size_in_log2 = 31 -__CLZ(sizet),
        .mem_type = MPU_MEM_NORMAL_WB_WRA,
        .exe_permission = 0,
        .ap_permission = MPU_AP_FULL_ACCESS
    };
    if (mpu_create_segment(&up_stack_ops) != ERR_OK) {
        user_heap_addr = t1;
        return NULL;
    }
    tmp = (uint32_t *)user_heap_addr;
    user_heap_addr += sizet;
    *size = sizet;
    return tmp;
}