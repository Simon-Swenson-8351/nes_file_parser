#include "nes_mapper.h"

#include "nes_mapper_00000.h"

/*  TEMPLATE FOR MAPPER .c FILE IMPLEMENTATIONS
static nes_mapper_result_t procw8(nes_mapper_t *self, uint16_t addr, uint8_t in)
{

}

static nes_mapper_result_t procr8(nes_mapper_t *self, uint16_t addr, uint8_t *out)
{

}

static nes_mapper_result_t procr16(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size)
{

}

static nes_mapper_result_t procr24(nes_mapper_t *self, uint16_t addr, uint8_t *out_buf, size_t out_buf_size)
{

}

static nes_mapper_result_t procgf(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out)
{

}

static nes_mapper_result_t ppuw8(nes_mapper_t *self, uint16_t addr, uint8_t in)
{

}

static nes_mapper_result_t ppur8(nes_mapper_t *self, uint16_t addr, uint8_t *out)
{

}

static nes_mapper_result_t ppugf(nes_mapper_t *self, uint16_t addr, uint8_t *flags_out, uint16_t *start_addr_out, uint16_t *end_addr_out)
{

}

static nes_mapper_result_t clr(nes_mapper_t *self)
{

}
*/
/*  TEMPLATE FOR MAPPER VTABLE ASSIGNMENTS
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
*/

nes_mapper_result_t nes_mapper_create(
    uint16_t mapper_id,
    uint8_t submapper_id,
    char *prg_rom_array,
    size_t prg_rom_size,
    char *chr_rom_array,
    size_t chr_rom_size,
    nes_mapper_t **result)
{
    switch(mapper_id)
    {
    case 0:
        return nes_mapper_00000_create(submapper_id, prg_rom_array, prg_rom_size, chr_rom_array, chr_rom_size, result);
        break;
    default:
        return NES_MAPPER_RESULT_UNSUPPORTED_MAPPER_ID;
    }
    return NES_MAPPER_RESULT_SUCCESS;
}