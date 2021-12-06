#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include "color.h"

// palette documentation available at:
// http://wiki.nesdev.com/w/index.php/PPU_palettes#Palettes

typedef struct nes_palette
{
    color_t colors[64];
} nes_palette_t;

const nes_palette_t NES_PALETTE_2C02 =
{
    .colors = 
    {
        { .r =  84, .g =  84, .b =  84 }, { .r =   0, .g =  30, .b = 116 }, { .r =   8, .g =  16, .b = 144 }, { .r =  48, .g =   0, .b = 136 }, { .r =  68, .g =   0, .b = 100 }, { .r =  92, .g =   0, .b =  48 }, { .r =  84, .g =   4, .b =   0 }, { .r =  60, .g =  24, .b =   0 }, { .r =  32, .g =  42, .b =   0 }, { .r =   8, .g =  58, .b =   0 }, { .r =   0, .g =  64, .b =   0 }, { .r =   0, .g =  60, .b =   0 }, { .r =   0, .g =  50, .b =  60 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 },
        { .r = 152, .g = 150, .b = 152 }, { .r =   8, .g =  76, .b = 196 }, { .r =  48, .g =  50, .b = 236 }, { .r =  92, .g =  30, .b = 228 }, { .r = 136, .g =  20, .b = 176 }, { .r = 160, .g =  20, .b = 100 }, { .r = 152, .g =  34, .b =  32 }, { .r = 120, .g =  60, .b =   0 }, { .r =  84, .g =  90, .b =   0 }, { .r =  40, .g = 114, .b =   0 }, { .r =   8, .g = 124, .b =   0 }, { .r =   0, .g = 118, .b =  40 }, { .r =   0, .g = 102, .b = 120 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 },
        { .r = 236, .g = 238, .b = 236 }, { .r =  76, .g = 154, .b = 236 }, { .r = 120, .g = 124, .b = 236 }, { .r = 176, .g =  98, .b = 236 }, { .r = 228, .g =  84, .b = 236 }, { .r = 236, .g =  88, .b = 180 }, { .r = 236, .g = 106, .b = 100 }, { .r = 212, .g = 136, .b =  32 }, { .r = 160, .g = 170, .b =   0 }, { .r = 116, .g = 196, .b =   0 }, { .r =  76, .g = 208, .b =  32 }, { .r =  56, .g = 204, .b = 108 }, { .r =  56, .g = 180, .b = 204 }, { .r =  60, .g =  60, .b =  60 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 },
        { .r = 236, .g = 238, .b = 236 }, { .r = 168, .g = 204, .b = 236 }, { .r = 188, .g = 188, .b = 236 }, { .r = 212, .g = 178, .b = 236 }, { .r = 236, .g = 174, .b = 236 }, { .r = 236, .g = 174, .b = 212 }, { .r = 236, .g = 180, .b = 176 }, { .r = 228, .g = 196, .b = 144 }, { .r = 204, .g = 210, .b = 120 }, { .r = 180, .g = 222, .b = 120 }, { .r = 168, .g = 226, .b = 144 }, { .r = 152, .g = 226, .b = 180 }, { .r = 160, .g = 214, .b = 228 }, { .r = 160, .g = 162, .b = 160 }, { .r =   0, .g =   0, .b =   0 }, { .r =   0, .g =   0, .b =   0 }
    }
};

// for the 3-bit DACs used in other palettes
//   0
//  36
//  73
// 109
// 146
// 182
// 219
// 255

#endif