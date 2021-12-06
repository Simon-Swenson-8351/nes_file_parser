#ifndef NES_MAPPER_00000_H
#define NES_MAPPER_00000_H

#include "nes_mapper.h"

nes_mapper_result_t nes_mapper_00000_create(
    uint8_t submapper_id,
    char *prg_rom_array,
    size_t prg_rom_size,
    char *chr_rom_array,
    size_t chr_rom_size,
    nes_mapper_t **result);

#endif