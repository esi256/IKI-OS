#ifndef MPU_H
#define MPU_H

#include <cmsis.h>

#define MPU_MIN_SEG_SIZE 32
#define MPU_MIN_SEG_SIZE_LOG 5
#define MPU_MAX_SEGMENTS 8
#define MPU_MAX_SEGMENTS_UPROC MPU_MAX_SEGMENTS - 1
#define MPU_MAX_SEGMENT_NUM 8

enum mpu_mem_type {
    MPU_MEM_STRONGLY_ORDERED     = 0x00,  /* TEX=000 S=0 C=0 B=0 */
    MPU_MEM_DEVICE_SHARED        = 0x01,  /* TEX=000 S=0 C=0 B=1 */
    MPU_MEM_DEVICE_NONSHARED     = 0x10,  /* TEX=010 S=0 C=0 B=0 */

    MPU_MEM_NORMAL_WT            = 0x02,  /* TEX=000 S=0 C=1 B=0 */
    MPU_MEM_NORMAL_WT_SHARED     = 0x06,  /* TEX=000 S=1 C=1 B=0 */

    MPU_MEM_NORMAL_WB_NWA        = 0x03,  /* TEX=000 S=0 C=1 B=1 */
    MPU_MEM_NORMAL_WB_NWA_SHARED = 0x07,  /* TEX=000 S=1 C=1 B=1 */

    MPU_MEM_NORMAL_NC            = 0x08,  /* TEX=001 S=0 C=0 B=0 */
    MPU_MEM_NORMAL_NC_SHARED     = 0x0C,  /* TEX=001 S=1 C=0 B=0 */

    MPU_MEM_NORMAL_WB_WRA        = 0x0B,  /* TEX=001 S=0 C=1 B=1 */
    MPU_MEM_NORMAL_WB_WRA_SHARED = 0x0F   /* TEX=001 S=1 C=1 B=1 */
};

enum mpu_cache_policy {
    MPU_CACHE_NONE        = 0x0,    /* Non-cacheable */
    MPU_CACHE_WB_WA_RA    = 0x1,    /* Write back, write and read allocate */
    MPU_CACHE_WT_NO_WA    = 0x2,    /* Write through, no write allocate */
    MPU_CACHE_WB_NO_WA    = 0x3     /* Write back, no write allocate */
};

enum mpu_ap {
    MPU_AP_NO_ACCESS          = 0x0,    /* priv: none,  unpriv: none */
    MPU_AP_PRIV_RW            = 0x1,    /* priv: RW,    unpriv: none */
    MPU_AP_PRIV_RW_UNPRIV_RO  = 0x2,    /* priv: RW,    unpriv: RO   */
    MPU_AP_FULL_ACCESS        = 0x3,    /* priv: RW,    unpriv: RW   */
    MPU_AP_PRIV_RO            = 0x5,    /* priv: RO,    unpriv: none */
    MPU_AP_READ_ONLY          = 0x6     /* priv: RO,    unpriv: RO   */
};

struct mpu_op {
    uint32_t addr;
    uint8_t nseg;
    uint8_t size_in_log2;
    uint8_t mem_type;
    uint8_t cache_policy;
    uint8_t ap_permission;
    uint8_t exe_permission;
};

enum kernel_error mpu_init(void);
enum kernel_error mpu_create_segment(struct mpu_op *ops);

#endif