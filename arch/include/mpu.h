#define MPU_MIN_SEG_SIZE 32
#define MPU_MIN_SEG_SIZE_LOG 5
#define MPU_MAX_SEGMENTS 8
#define MPU_MAX_SEGMENTS_UPROC MPU_MAX_SEGMENTS - 1
#define MPU_MAX_SEGMENT_NUM 8

uint8_t mpu_init(void);
uint8_t mpu_create_segment(uint32_t nseg,uint32_t addr, uint32_t sizelog2, uint32_t memtype, uint8_t perm);
