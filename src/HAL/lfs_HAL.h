#include "littlefs/lfs.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define BLOCK_SIZE_BYTES (FLASH_SECTOR_SIZE)
#define HW_FLASH_STORAGE_BYTES (1024 * 1024)
#define HW_FLASH_STORAGE_BASE (PICO_FLASH_SIZE_BYTES - HW_FLASH_STORAGE_BYTES)

#define LFS_BUFFER_SIZE 64

int pico_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int pico_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int pico_erase(const struct lfs_config *c, lfs_block_t block);
int pico_sync();



static const struct lfs_config PICO_FLASH_CONFIG =
    {
        .read = pico_read,
        .prog = pico_prog,
        .erase = pico_erase,
        .sync = pico_sync,

        .read_size = FLASH_PAGE_SIZE,
        .prog_size = FLASH_PAGE_SIZE,

        .block_size = BLOCK_SIZE_BYTES,
        .block_count = HW_FLASH_STORAGE_BYTES / BLOCK_SIZE_BYTES,
        .block_cycles = 16,

        .cache_size = FLASH_PAGE_SIZE,
        .lookahead_size = FLASH_PAGE_SIZE,
};