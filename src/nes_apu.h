#ifndef NES_APU_H
#define NES_APU_H

#include <inttypes.h>

#include "bits.h"

const struct
{
    uint16_t sq1_vol;
    uint16_t sq1_sweep;
    uint16_t sq1_lo;
    uint16_t sq1_hi;
    uint16_t sq2_vol;
    uint16_t sq2_sweep;
    uint16_t sq2_lo;
    uint16_t sq2_hi;
    uint16_t tri_linear;
    uint16_t tri_lo;
    uint16_t tri_hi;
    uint16_t noise_vol;
    uint16_t noise_lo;
    uint16_t noise_hi;
    uint16_t dmc_freq;
    uint16_t dmc_raw;
    uint16_t dmc_start;
    uint16_t dmc_len;
    uint16_t snd_chn;
    uint16_t test_dac1;
    uint16_t test_dac2;
    uint16_t test_dac3;
    uint16_t test_pits1;
    uint16_t test_pits2;
    uint16_t test_pits3;
} NES_APU_REG_ADDR =
{
    .sq1_vol = 0x4000,
    .sq1_sweep = 0x4001,
    .sq1_lo = 0x4002,
    .sq1_hi = 0x4003,
    .sq2_vol = 0x4004,
    .sq2_sweep = 0x4005,
    .sq2_lo = 0x4006,
    .sq2_hi = 0x4007,
    .tri_linear = 0x4008,
    .tri_lo = 0x400A,
    .tri_hi = 0x400B,
    .noise_vol = 0x400C,
    .noise_lo = 0x400E,
    .noise_hi = 0x400F,
    .dmc_freq = 0x4010,
    .dmc_raw = 0x4011,
    .dmc_start = 0x4012,
    .dmc_len = 0x4013,
    .snd_chn = 0x4015,
    .test_dac1 = 0x4018,
    .test_dac2 = 0x4019,
    .test_dac3 = 0x401A,
    .test_pits1 = 0x401C,
    .test_pits2 = 0x401D,
    .test_pits3 = 0x401E
};

// TODO bits info

#endif