#ifndef MEM_MIRROR_H
#define MEM_MIRROR_H

#include <inttypes.h>

// handles a contiguous area of memory that is, in reality, a much smaller 
// area of memory that is mirrored repeatedly
typedef struct mem_mirror_info
{
    uint16_t start; // start of the *actual* memory as well as the mirrored block
    uint16_t len; // length of the *actual* memory
    uint16_t num_mirrors; // defines the total block of address space taken up by this mirror.
                          // the range is from start to start + len * num_mirrors
} mem_mirror_info_t;

// if the given address lies within the address space that the mirror occupies, 
// this function will collapse it into its *unique* address.
uint16_t mem_mirror_collapse(mem_mirror_info_t *mirror_info, uint16_t addr);

#endif