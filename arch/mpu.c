#include "cmsis.h"
#include "syscalls.h"

uint8_t is_mpu_enabled(void)
{
    return ((MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos);
}

uint8_t mpu_create_segment(uint32_t nseg,uint32_t addr, uint32_t sizelog2, uint32_t memtype, uint8_t perm)
{
    sizelog2--;
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 0x0;
    MPU->RNR = nseg & 0xf;
    MPU->RASR |= (sizelog2 & 0x1f) << MPU_RASR_SIZE_Pos;
    MPU->RBAR |= (addr >> sizelog2) << sizelog2;
    MPU->RBAR |= (nseg & 0xf) << MPU_RBAR_REGION_Pos;
    MPU->RASR = 0x0;
    MPU->RASR &= ~(MPU_RASR_XN_Msk);
    if (perm > 0)
        MPU->RASR |= (3 << MPU_RASR_AP_Pos); // Full Access
    else
        MPU->RASR &= ~(MPU_RASR_AP_Msk); // Zero Access

    // Normal mode with write back cache
    MPU->RASR &= ~MPU_RASR_TEX_Msk;
    MPU->RASR |= MPU_RASR_C_Msk;
    MPU->RASR |= MPU_RASR_B_Msk;
    MPU->RASR |= MPU_RASR_S_Msk;

    MPU->RBAR |= MPU_RBAR_VALID_Msk;
    MPU->RASR |= MPU_RASR_ENABLE_Msk;
    MPU->CTRL |= MPU_CTRL_ENABLE_Msk;  
    return 1;  
}

uint8_t mpu_init(void)
{
    if (is_mpu_enabled() < 8)
        return -1;
    
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    MPU->CTRL = 0x00;
    MPU->CTRL |= MPU_CTRL_PRIVDEFENA_Msk;
    MPU->CTRL &= ~MPU_CTRL_HFNMIENA_Msk;
}