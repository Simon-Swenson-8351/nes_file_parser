#include "bits.h"

uint8_t bits_get(uint8_t raw_byte, uint8_t mask, uint8_t offset)
{
    return (raw_data & mask) >> offset;
}