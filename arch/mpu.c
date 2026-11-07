#include <syscalls.h>
#include <mpu.h>
#include <mem.h>
#include <kernel_error.h>

uint8_t mpu_count_regions(void)
{
    return ((MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos);
}

enum kernel_error mpu_create_segment(struct mpu_op *ops)
{
    if (ops->size_in_log2 < 5 || ops->size_in_log2 > 32)
        return ERR_MPU_BAD_SIZE;
    uint8_t size = ops->size_in_log2 - 1;
    if (ops->nseg > (MPU_MAX_SEGMENTS-1))
        return ERR_MPU_NO_FREE_REGION;
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
    MPU->RNR = ops->nseg;  
    MPU->RASR = 0x00;
    MPU->RBAR = 0x00; 
    MPU->RASR |= (size & 0x1f) << MPU_RASR_SIZE_Pos;
    MPU->RBAR |= (ops->addr >> ops->size_in_log2) << ops->size_in_log2;
    MPU->RBAR |= (ops->nseg) << MPU_RBAR_REGION_Pos;
    MPU->RBAR &= ~(MPU_RBAR_VALID_Msk);

    MPU->RASR |= ((ops->exe_permission ? 0 : 1) << MPU_RASR_XN_Pos);
    MPU->RASR |= (ops->ap_permission << MPU_RASR_AP_Pos);
    MPU->RASR |= (ops->mem_type << MPU_RASR_B_Pos);

    MPU->RBAR |= MPU_RBAR_VALID_Msk;
    MPU->RASR |= MPU_RASR_ENABLE_Msk;
    MPU->CTRL |= MPU_CTRL_ENABLE_Msk;  
    return ERR_OK;
}

enum kernel_error mpu_init(void)
{
    enum kernel_error err;

    if (mpu_count_regions() < MPU_MAX_SEGMENTS)
        return ERR_MPU_NOT_SUPPORTED;  
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    MPU->CTRL = 0x00;
    MPU->CTRL |= MPU_CTRL_PRIVDEFENA_Msk;
    MPU->CTRL &= ~MPU_CTRL_HFNMIENA_Msk;

    struct mpu_op code_seg_op = {
        .addr = 0x08000000,
        .nseg = 0,
        .size_in_log2 = 20,
        .mem_type = MPU_MEM_NORMAL_WT,
        .exe_permission = 1,
        .ap_permission = MPU_AP_READ_ONLY
    };
    if ((err = mpu_create_segment(&code_seg_op)) != ERR_OK)
        return err;
    MPU->CTRL |= MPU_CTRL_ENABLE_Msk;
    return ERR_OK;
}