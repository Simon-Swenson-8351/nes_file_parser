#ifndef NES_PRG_ROM_BANK
#define NES_PRG_ROM_BANK

#include <inttypes.h>

const uint16_t NES_PRG_ROM_BANK_SIZE = 0x4000;

typedef struct nes_prg_rom_bank
{
    char contents[NES_PRG_ROM_BANK_SIZE];
} nes_prg_rom_bank_t;

#endif