#ifndef NES_HEADER_H
#define NES_HEADER_H

const uint64_t NES_HEADER_PARSER_VERSION[] = {0, 1, 0};

const size_t NES_HEADER_SIZE = 16;

const char NES_HEADER_MAGIC[] = {'\x4e', '\x45', '\x53', '\x1a'};

const uint16_t NES_HEADER_PROG_ROM_BLOCK_SIZE = 0x4000;
const uint16_t NES_HEADER_CHAR_ROM_BLOCK_SIZE = 0x2000;
const uint8_t NES_HEADER_PROG_RAM_BLOCK_SIZE = 0x40;
const uint8_t NES_HEADER_PROG_EEPRON_BLOCK_SIZE = 0x40;
const uint8_t NES_HEADER_CHAR_RAM_BLOCK_SIZE = 0x40;
const uint8_t NES_HEADER_CHAR_EEPRON_BLOCK_SIZE = 0x40;

typedef enum nes_header_type
{
    NES_HEADER_TYPE_INES = 1,
    NES_HEADER_TYPE_NES_2 = 2
} nes_header_type_t;
const char *NES_HEADER_TYPE_STR[] = {"", "iNES", "NES 2.0"};

typedef enum nes_header_nametable_mirroring_type
{
    NES_HEADER_NAMETABLE_MIRRORING_TYPE_HORIZONTAL = 0,
    NES_HEADER_NAMETABLE_MIRRORING_TYPE_VERTICAL = 1,
    NES_HEADER_NAMETABLE_MIRRORING_TYPE_NO_MIRRORING = 2
} nes_header_nametable_mirroring_type_t;
const char *NES_HEADER_NAMETABLE_MIRRORING_TYPE_STR[] = {"horizontal", "vertical", "no mirroring"};

typedef enum nes_header_console_type
{
    NES_HEADER_CONSOLE_TYPE_NES = 0,
    NES_HEADER_CONSOLE_TYPE_VS_SYSTEM = 1,
    NES_HEADER_CONSOLE_TYPE_PLAYCHOICE_10 = 2,
    NES_HEADER_CONSOLE_TYPE_EXTENDED = 3
} nes_header_console_type_t;
const char *NES_HEADER_CONSOLE_TYPE_STR[] = {"NES", "Vs. System", "Playchoice-10", "extended"};

typedef enum nes_header_timing_type
{
    NES_HEADER_TIMING_TYPE_NTSC = 0,
    NES_HEADER_TIMING_TYPE_PAL = 1,
    NES_HEADER_TIMING_TYPE_MULTI = 2,
    NES_HEADER_TIMING_TYPE_DENDY = 3
} nes_header_timing_type_t;
const char *NES_HEADER_TIMING_TYPE_STR[] = {"NTSC", "PAL", "multi-region", "Dendy"};

typedef struct nes_header
{
    uint64_t parser_version[3];
    ines_header_type_t type;
    uint64_t prg_rom_size;  // number of 16K PRG-ROM banks
    uint64_t char_rom_size; // number of 8K CHR-ROM banks
    uint16_t mapper_id;
    nametable_mirroring_type_t ntmt;
    bool persistent_memory;
    bool trainer;
    console_type_t ct;
    struct
    {
        uint8_t submapper_id;
        uint16_t prg_ram_size;     // the total PRG-RAM    size is 64 * prg_ram_size
        uint16_t prg_eeprom_size;  // the total PRG-EEPROM size is 64 * prg_eeprom_size
        uint16_t char_ram_size;    // the total CHR-RAM    size is 64 * char_ram_size
        uint16_t char_eeprom_size; // the total CHR-EEPROM size is 64 * char_eeprom_size
        timing_type_t tt;
        union
        {
            struct
            {
                uint8_t ppu_type;
                uint8_t hardware_type;
            } vs_system;
            struct
            {
                uint8_t type;
            } extended_console;
        } console_type_info;
        uint8_t misc_roms_size;
        uint8_t default_expansion_device;
    } nes_2;
} nes_header_t;

typedef enum nes_header_result
{
    NES_HEADER_RESULT_SUCCESS = 0,
    NES_HEADER_RESULT_WRONG_BUF_SIZE,
    NES_HEADER_RESULT_INVALID_HEADER
} nes_header_result_t;
char *NES_HEADER_RESULT_STR[] = {"success", "Wrong buffer size given. iNES headers must be 16 bytes long.", "invalid header"};

nes_header_result_t nes_header_parse(char *header_buf, size_t header_buf_size, nes_header_t *out);
nes_header_result_t nes_header_serialize(nes_header_t *in, char *out_buf, size_t out_buf_size);

#endif
