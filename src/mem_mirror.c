#include "mem_mirror.h"

uint16_t mem_mirror_collapse(mem_mirror_info_t *mirror_info, uint16_t addr)
{
    if(addr >= mirror_info->start && addr < mirror_info->start + mirror_info->len * num_mirrors)
    {
        return ((addr - mirror_info->start) % mirror_info->len) + mirror_info->start;
    }
    else return addr;
}