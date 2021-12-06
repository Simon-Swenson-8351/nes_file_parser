#include "nes_mapper_00000.h"

#include <string.h>
#include <stdbool.h>

typedef struct nes_mapper_00000
{
    nes_mapper_t super;
    bool prg_rom_mirrored;
} nes_mapper_00000_t;

static uint16_t handle_proc_mirrors(uint16_t addr)
{
    if(addr < 0x2000)
    {
        // RAM
        addr = addr & 0x07FF;
    }
    else if(addr >= 0x2000 && addr < 0x4000)
    {
        // PPU
        addr = addr & 0x07;
        addr += 0x2000;
    }
    return addr;
}

// the address passed into this should already have its mirrors handled.
static bool proc_addr_is_read_only(uint16_t addr)
{
    return addr == 0x2002
        || addr == 0x4018
        || addr == 0x4019
        || addr >= 0x8000;
}

static bool proc_addr_is_unmapped(uint16_t addr)
{
    return addr >= 0x4020
        && addr <  0x8000;
}

static nes_mapper_result_t procw8(nes_mapper_t *self, uint16_t addr, uint8_t in)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
    addr = handle_proc_mirrors(addr);
    if(proc_addr_is_read_only(addr)) return NES_MAPPER_RESULT_WRITE_TO_READ_ONLY;
    if(proc_addr_is_unmapped(addr)) return NES_MAPPER_RESULT_ADDR_UNMAPPED;
    ts->proc_mem[addr] = in;
    return NES_MAPPER_RESULT_SUCCESS;
}

static nes_mapper_result_t procr8(nes_mapper_t *self, uint16_t addr, uint8_t *out)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
    addr = handle_proc_mirrors(addr);
    *out = ts->proc_mem[addr];
}

static nes_mapper_result_t procr16(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
    if(out_buf_size < 2) return NES_MAPPER_RESULT_BUFFER_TOO_SMALL;
    addr = handle_proc_mirrors(addr);
    memcpy(out_buf, ts->proc_mem + addr, 2);
}

static nes_mapper_result_t procr24(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
    if(out_buf_size < 3) return NES_MAPPER_RESULT_BUFFER_TOO_SMALL;
    addr = handle_proc_mirrors(addr);
    memcpy(out_buf, ts->proc_mem + addr, 3);
}

static nes_mapper_result_t procgf(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
}

static nes_mapper_result_t ppuw8(nes_mapper_t *self, uint16_t addr, uint8_t in)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
}

static nes_mapper_result_t ppur8(nes_mapper_t *self, uint16_t addr, uint8_t *out)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
}

static nes_mapper_result_t ppugf(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
}

static nes_mapper_result_t clr(nes_mapper_t *self)
{
    nes_mapper_00000_t *ts = (nes_mapper_00000_t *)((char *)self - offsetof(nes_mapper_00000_t, vt));
    free(ts);
}

static const nes_mapper_iface_t NES_MAPPER_VT = 
{
    .proc_write_8 =   procw8,
    .proc_read_8 =    procr8,
    .proc_read_16 =   procr16,
    .proc_read_24 =   procr24,
    .proc_get_flags = procgf,
    .ppu_write_8 =    ppuw8,
    .ppu_read_8 =     ppur8,
    .ppu_get_flags =  ppugf,
    .clear =          clr
};

nes_mapper_result_t nes_mapper_00000_create(
    uint16_t mapper_id,
    uint8_t submapper_id,
    char *prg_rom_array,
    size_t prg_rom_size,
    char *chr_rom_array,
    size_t chr_rom_size,
    nes_mapper_t **result)
{
    if(prg_rom_size > 2 || prg_rom_size == 0 || chr_rom_size > 1 || chr_rom_size == 0)
        return NES_MAPPER_RESULT_ROM_SIZE_MAPPER_INCOMPATIBILITY;
    nes_mapper_00000_t *r = malloc(sizeof(nes_mapper_00000_t));
    r->vt = &NES_MAPPER_VT;
    memcpy(r->proc_mem + 0x8000, prg_rom_array, 0x4000 * prg_rom_size);
    memcpy(r->ppu_mem, chr_rom_array, 0x2000);
    if(prg_rom_size == 1) r->prg_rom_mirrored = true;
    else r->prg_rom_mirrored = false;
    *result = &(r->vt);
    return NES_MAPPER_RESULT_SUCCESS;
}