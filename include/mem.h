uint8_t memory_map_init(void);
void *kalloc(uint32_t *size);
void *ualloc(uint32_t *size);
uint32_t *uproc_stack_allocate(uint8_t id ,uint32_t *size);
