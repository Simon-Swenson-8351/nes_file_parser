#ifndef NES_CHR_ROM_BANK
#define NES_CHR_ROM_BANK

#include <inttypes.h>

const uint16_t NES_CHR_ROM_BANK_SIZE = 0x2000;

typedef struct nes_chr_rom_bank
{
    char contents[NES_CHR_ROM_BANK_SIZE];
} nes_chr_rom_bank_t;

#endif