#include "lfs_HAL.h"

int pico_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
    if (c == NULL || buffer == NULL)
        return PICO_ERROR_GENERIC;

    uint32_t fs_start = XIP_BASE + HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * c->block_size) + off;

    memcpy(buffer, (unsigned char *)addr, size);
    return 0;
}

int pico_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    if (c == NULL || buffer == NULL)
        return PICO_ERROR_GENERIC;

    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * c->block_size) + off;

    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(addr, (const uint8_t *)buffer, size);
    restore_interrupts(ints);

    return 0;
}

int pico_erase(const struct lfs_config* c, lfs_block_t block)
{
    if (c == NULL)
        return PICO_ERROR_GENERIC;

    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t offset = fs_start + (block * c->block_size);

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(offset, c->block_size);
    restore_interrupts(ints);

    return 0;
}

int pico_sync()
{
    return 0;
}

