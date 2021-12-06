#ifndef BITS_H
#define BITS_H

typedef struct bits_info
{
    uint8_t mask;
    uint8_t offset;
} bits_info_t;

uint8_t bits_get(uint8_t raw_byte, uint8_t mask, uint8_t offset);

#endif