#ifndef NES_PPU_H
#define NES_PPU_H

#include <inttypes.h>

#include "bits.h"
#include "mem_mirror.h"

// documentation for registers can be found at:
// https://wiki.nesdev.com/w/index.php/CPU_memory_map

const mem_mirror_info_t NES_PPU_MEM_MIRROR_INFO =
{
    .start = 0x2000,
    .len = 8,
    .num_mirrors = 1024
};

// I'm going to put these in structs so there aren't so many consts floating 
// around in the global namespace.
const struct
{
    uint16_t ctrl;
    uint16_t mask;
    uint16_t status;
    uint16_t oam_addr;
    uint16_t oam_data;
    uint16_t scroll;
    uint16_t addr;
    uint16_t data;
    uint16_t oam_dma;
} NES_PPU_REG_ADDR =
{
    .ctrl = 0x2000,
    .mask = 0x2001,
    .status = 0x2002,
    .oam_addr = 0x2003,
    .oam_data = 0x2004,
    .scroll = 0x2005,
    .addr = 0x2006,
    .data = 0x2007,
    .oam_dma = 0x4014
};

const struct
{
    const bits_info_t base_nametable_addr;
    const bits_info_t vram_addr_inc;
    const bits_info_t spr_pattern_table_addr;
    const bits_info_t bg_pattern_table_addr;
    const bits_info_t spr_size;
    const bits_info_t ppu_master_slave;
    const bits_info_t nmi_at_vblank;
} NES_PPU_REG_CTRL_BITS_INFO = 
{
    .base_nametable_addr =
    {
        .mask = 0x03,
        .offset = 0
    },
    .vram_addr_inc =
    {
        .mask = 0x04,
        .offset = 2
    },
    .spr_pattern_table_addr =
    {
        .mask = 0x08,
        .offset = 3
    },
    .bg_pattern_table_addr =
    {
        .mask = 0x10,
        .offset = 4
    },
    .spr_size =
    {
        .mask = 0x20,
        .offset = 5
    },
    .ppu_master_slave =
    {
        .mask = 0x40,
        .offset = 6
    },
    .nmi_at_vblank =
    {
        .mask = 0x80,
        .offset = 7
    }
};

const struct
{
    const bits_info_t grayscale;
    const bits_info_t show_left_bg;
    const bits_info_t show_left_spr;
    const bits_info_t show_bg;
    const bits_info_t show_spr;
    const bits_info_t emph_r;
    const bits_info_t emph_g;
    const bits_info_t emph_b;
} NES_PPU_REG_MASK_BITS_INFO = //name is a bit wonky because the register is also called "PPUMASK"
{
    .grayscale =
    {
        .mask = 0x01,
        .offset = 0
    },
    .show_left_bg =
    {
        .mask = 0x02,
        .offset = 1
    },
    .show_left_spr =
    {
        .mask = 0x04,
        .offset = 2
    },
    .show_bg =
    {
        .mask = 0x08,
        .offset = 3
    },
    .show_spr =
    {
        .mask = 0x10,
        .offset = 4
    },
    .emph_r =
    {
        .mask = 0x20,
        .offset = 5
    },
    .emph_g =
    {
        .mask = 0x40,
        .offset = 6
    },
    .emph_b =
    {
        .mask = 0x80,
        .offset = 7
    }
};

struct
{
    const bits_info_t prev_write;
    const bits_info_t spr_overflow;
    const bits_info_t spr_0_hit;
    const bits_info_t vblank_started;
} NES_PPU_REG_STATUS_BITS_INFO =
{
    .prev_write =
    {
        .mask = 0x1F,
        .offset = 0
    },
    .spr_overflow =
    {
        .mask = 0x20,
        .offset = 5
    },
    .spr_0_hit =
    {
        .mask = 0x40,
        .offset = 6
    },
    .vblank_started =
    {
        .mask = 0x80,
        .offset = 7
    }
};

#endif