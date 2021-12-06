#ifndef NES_INPUT_H
#define NES_INPUT_H

#include <inttypes.h>

#include "bits.h"

const struct
{
    uint16_t joy1;
    uint16_t joy2;
} NES_INPUT_REG_ADDR =
{
    .joy1 = 0x4016,
    .joy2 = 0x4017
};

const struct
{
    const bits_info_t d0;
    const bits_info_t d1;
    const bits_info_t d2;
    const bits_info_t d3;
    const bits_info_t d4;
} NES_INPUT_REG_JOY_READ_BITS_INFO =
{
    .d0 =
    {
        .mask = 0x01,
        .offset = 0
    },
    .d1 =
    {
        .mask = 0x02,
        .offset = 1
    },
    .d2 =
    {
        .mask = 0x04,
        .offset = 2
    },
    .d3 =
    {
        .mask = 0x08,
        .offset = 3
    },
    .d4 =
    {
        .mask = 0x10,
        .offset = 4
    }
};

const struct
{
    const bits_info_t ctrl_latch;
    const bits_info_t exp_latch;
} NES_INPUT_REG_JOY1_WRITE_BITS_INFO =
{
    .ctrl_latch =
    {
        .mask = 0x01,
        .offset = 0
    },
    .exp_latch =
    {
        .mask = 0x6,
        .offset = 1
    }
};

#endif