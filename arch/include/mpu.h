#define MPU_MIN_SEG_SIZE 32
#define MPU_MIN_SEG_SIZE_LOG 5

uint8_t mpu_init(void);
void mpu_create_segment(uint32_t nseg,uint32_t addr, uint32_t sizelog2, uint32_t memtype, uint8_t perm);
