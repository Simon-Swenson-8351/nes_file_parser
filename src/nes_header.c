#include "nes_header.h"

#include <string.h>

// the following functions use the tests recommended at:
// https://wiki.nesdev.com/w/index.php/NES_2.0#Identification
static bool header_is_ines(char *header_buf)
{
    return !memcmp(NES_HEADER_MAGIC, header_buf, sizeof(NES_HEADER_MAGIC));
}

static bool header_is_nes_2(char *header_buf)
{
    char header_type_indicator = header_buf[7] & 0x0c;
    return header_is_ines(header_buf) && (header_type_indicator == 0x08);
}

// the following function uses documentation available at:
// https://wiki.nesdev.com/w/index.php/INES
static nes_header_result_t parse_ines_header(char *header_buf, nes_header_t *out)
{
    memcpy(&(out->parser_version), PARSER_VERSION, sizeof(PARSER_VERSION));
    out->type = IHT_INES;
    out->prg_rom_size = *((uint8_t *)(header_buf + 4));
    out->char_rom_size = *((uint8_t *)(header_buf + 5));
    uint8_t byte6 = *((uint8_t *)(header_buf + 6));
    uint8_t byte7 = *((uint8_t *)(header_buf + 7));
    out->mapper_id = ((byte6 & 0xf0) >> 4) | (byte7 & 0xf0);
    if(byte6 & 0x08) out->ntmt = NMT_NO_MIRRORING;
    else if(byte6 & 0x01) out->ntmt = NMT_VERTICAL;
    else out->ntmt = NMT_HORIZONTAL;
    out->persistent_memory = byte6 & 0x02;
    out->trainer = byte6 & 0x04;
    out->ct = (console_type_t)(byte7 & 0x03);
    return PR_SUCCESS;
}

static uint16_t handle_shift(uint8_t shift)
{
    if(!shift)
    {
        return 0;
    }
    else
    {
        return 1 << (shift - 1);
    }
}

// If the highest nibble is $f, exponent-multiplier notation is used. See:
// https://wiki.nesdev.com/w/index.php/NES_2.0#PRG-ROM_Area
// note that the highest possible value of the exponent is 63, so there is 
// a chance that, with a value of 63 and a multiplier > 1, we will overflow.
// Not sure how to handle that without using a bignum library, but it seems 
// unlikely to overflow in practice.
// In fact, the mmap will fail before this overflows because of the large file 
// size.
static uint64_t handle_rom_size_encoding(uint64_t input_rom_size)
{
    uint16_t irs_u16 = (uint16_t)input_rom_size;
    if(irs_u16 & 0x0f00 == 0x0f00)
    {
        uint8_t exponent = (irs_u16 & 0x00fc) >> 2;
        uint8_t multiplier = irs_u16 & 0x0003;
        return (1 << exponent) * (multiplier * 2 + 1);
    }
    else return input_rom_size;
}

// the following function uses documentation available at:
// https://wiki.nesdev.com/w/index.php/INES
// https://wiki.nesdev.com/w/index.php/NES_2.0
static parse_result_t parse_nes_2_header(char *header_buf, nes_header_t *out)
{
    parse_result_t result = parse_ines_header(header_buf, out);
    if(result) return result; // if non-zero (iNES parser had an error), propagate error.
    out->type = IHT_NES_2;
    uint8_t *header_buf_uint8 = (uint8_t *)header_buf;
    out->nes_2.submapper_id = (header_buf_uint8[8] & 0xf0) >> 4;
    out->mapper_id |= ((uint16_t)(header_buf_uint8[8] & 0x0f)) << 8;
    out->prg_rom_size |= ((uint64_t)(header_buf_uint8[9] & 0x0f)) << 8;
    out->prg_rom_size = handle_rom_size_encoding(out->prg_rom_size);
    out->char_rom_size |= ((uint64_t)(header_buf_uint8[9] & 0xf0)) << 4;
    out->char_rom_size = handle_rom_size_encoding(out->char_rom_size);

    out->nes_2.prg_ram_size = handle_shift((uint8_t)(header_buf_uint8[10] & 0x0f));
    out->nes_2.prg_eeprom_size = handle_shift((uint8_t)((header_buf_uint8[10] & 0xf0) >> 4));
    out->nes_2.char_ram_size = handle_shift((uint8_t)(header_buf_uint8[11] & 0x0f));
    out->nes_2.char_eeprom_size = handle_shift((uint8_t)((header_buf_uint8[11] & 0xf0) >> 4));

    out->nes_2.tt = (timing_type_t)(header_buf_uint8[12] & 0x03);

    if(out->ct == CT_VS_SYSTEM)
    {
        out->nes_2.console_type_info.vs_system.ppu_type = header_buf_uint8[13] & 0x0f;
        out->nes_2.console_type_info.vs_system.hardware_type = (header_buf_uint8[13] & 0xf0) >> 4;
    }
    else if(out->ct == CT_EXTENDED)
    {
        out->nes_2.console_type_info.extended_console.type = header_buf_uint8[13] & 0x0f;
    }
    out->nes_2.misc_roms_size = header_buf_uint8[14] & 0x03;
    out->nes_2.default_expansion_device = header_buf_uint8[15] & 0x3f;
    return PR_SUCCESS;
}

nes_header_result_t nes_header_parse(char *header_buf, size_t header_buf_size, nes_header_t *out)
{
    if(header_buf_size != NES_HEADER_SIZE) return NES_HEADER_RESULT_WRONG_BUF_SIZE;

    if(header_is_nes_2(header_buf)) return parse_nes_2_header(header_buf, out);
    else if(header_is_ines(header_buf)) return parse_ines_header(header_buf, out);
    else return NES_HEADER_RESULT_INVALID_HEADER;
}