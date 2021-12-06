#ifndef NES_MAPPER_H
#define NES_MAPPER_H

#include <inttypes.h>

#include "../mem_mirror.h"

typedef struct nes_mapper_result
{
    NES_MAPPER_RESULT_SUCCESS = 0,
    NES_MAPPER_RESULT_UNSUPPORTED_MAPPER_ID,
    NES_MAPPER_RESULT_ROM_SIZE_MAPPER_INCOMPATIBILITY,
    NES_MAPPER_RESULT_WRITE_TO_READ_ONLY,
    NES_MAPPER_RESULT_ADDR_UNMAPPED,
    NES_MAPPER_RESULT_BUFFER_TOO_SMALL
} nes_mapper_result_t;

typedef struct nes_mapper_mem_flags
{
    uint8_t read: 1;
    uint8_t write: 1;
    uint8_t persistent: 1;
} nes_mapper_mem_flags_t;

typedef struct nes_mapper_iface nes_mapper_iface_t;
typedef struct nes_mapper nes_mapper_t;
struct nes_mapper_iface
{
    (nes_mapper_result_t)(*cpu_write_8)(nes_mapper_t *self, uint16_t addr, uint8_t in);
    (nes_mapper_result_t)(*cpu_read_8)(nes_mapper_t *self, uint16_t addr, uint8_t *out);
    (nes_mapper_result_t)(*cpu_read_16)(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size);
    (nes_mapper_result_t)(*cpu_read_24)(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size);
    // gets the flags (read, write, persistent) for the given address as well as the start of
    // the block of address space for which these flags persist and 
    (nes_mapper_result_t)(*cpu_get_flags)(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out);

    (nes_mapper_result_t)(*ppu_write_8)(nes_mapper_t *self, uint16_t addr, uint8_t in);
    (nes_mapper_result_t)(*ppu_read_8)(nes_mapper_t *self, uint16_t addr, uint8_t *out);
    // gets the flags (read, write, persistent) for the given address as well as the start of
    // the block of address space for which these flags persist and 
    (nes_mapper_result_t)(*ppu_get_flags)(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out);

    // releases all resources that this object allocated
    (nes_mapper_result_t)(*clear)(nes_mapper_t *self);
};

const mem_mirror_info_t NES_MAPPER_RAM_MIRROR_INFO = 
{
    .start = 0,
    .len = 0x0800,
    .num_mirrors = 4
};

struct nes_mapper
{
    nes_mapper_iface_t *vtable;
    uint8_t cpu_addr_space[0x10000];
    uint16_t cpu_last_addr;
    uint8_t ppu_addr_space[0x4000];
    uint16_t ppu_last_addr;
    uint8_t oam_mem[0x100];
    uint8_t oam_last_addr;
};


// Attempts to create a new NES memory mapper object which handles bank switching 
// correctly for the given mapper_id and submapper_id
nes_mapper_result_t nes_mapper_create(
    uint16_t mapper_id,
    uint8_t submapper_id,
    char *prg_rom_array,
    size_t prg_rom_size, // number of banks as specified in the iNES header, not the total size of the prg_rom_array buffer
    char *chr_rom_array,
    size_t chr_rom_size, // number of banks as specified in the iNES header, not the total size of the chr_rom_array buffer
    nes_mapper_t **result);

nes_mapper_result_t nes_mapper_

#endif