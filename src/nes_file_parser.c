/**
 * nes_file_parser.c
 *
 * parses a .nes file and provides several outputs:
 * - A .json file representing the header contents
 * - A number of .bin files representing the contents of the ROM banks
 * - A number of .chr files representing the contents of the CHAR banks
 *
 * Makes extensive use of the documentation available at https://wiki.nesdev.com
 *
 * @author Simon Swenson
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include <sys/mman.h>

#include <json-c/json.h>

const uint64_t VERSION[] = {0, 1, 0}; // v0.1.0

const char *NES_SUFFIX = ".nes";
const char *HEADER_SUFFIX = ".json";
const char *ROM_SUFFIX = ".bin";
const char *CHAR_SUFFIX = ".chr";

const uint16_t PRG_ROM_BLOCK_SIZE = 0x4000;
const uint16_t CHAR_ROM_BLOCK_SIZE = 0x2000;

// "NES" + DOS EOF
const char NES_MAGIC[] = {'\x4e', '\x45', '\x53', '\x1a'};

typedef enum return_code
{
    RC_SUCCESS = 0,
    RC_ERR_INVALID_INPUT_FILETYPE = 1,
    RC_ERR_INFILE_OPEN_ERR = 2,
    RC_ERR_OUTFILE_OPEN_ERR = 3,
    RC_ERR_INFILE_CORRUPTED = 4,
    RC_ERR_PARSE_ERR = 5,
    RC_ERR_BANK_SAVE_ERR = 6
} return_code_t;

typedef enum ines_header_type
{
    IHT_INES = 1,
    IHT_NES_2 = 2
} ines_header_type_t;
const char *INES_HEADER_TYPE_STR[] = {"", "iNES", "NES 2.0"};

typedef enum nametable_mirroring_type
{
    NMT_HORIZONTAL = 0,
    NMT_VERTICAL = 1,
    NMT_NO_MIRRORING = 2
} nametable_mirroring_type_t;
const char *NAMETABLE_MIRRORING_TYPE_STR[] = {"horizontal", "vertical", "no mirroring"};

typedef enum console_type
{
    CT_NES = 0,
    CT_VS_SYSTEM = 1,
    CT_PLAYCHOICE_10 = 2,
    CT_EXTENDED = 3
} console_type_t;
const char *CONSOLE_TYPE_STR[] = {"NES", "Vs. System", "Playchoice-10", "extended"};

typedef enum timing_type
{
    CT_NTSC = 0,
    CT_PAL = 1,
    CT_MULTI = 2,
    CT_DENDY = 3
} timing_type_t;
const char *TIMING_TYPE_STR[] = {"NTSC", "PAL", "multi-region", "Dendy"};

typedef struct ines_header
{
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
} ines_header_t;

bool header_is_ines(char *header_buf);
bool header_is_nes_2(char *header_buf);

typedef enum parse_result
{
    PR_SUCCESS = 0,
} parse_result_t;

parse_result_t parse_ines_header(char *header_buf, ines_header_t *out);
parse_result_t parse_nes_2_header(char *header_buf, ines_header_t *out);

int save_output(ines_header_t *header, char *infile_buf, char *outfile_base_name);

int main(int argc, char *argv[])
{
    return_code_t result = RC_SUCCESS;
    char *nes_rom_file = argv[argc - 1];
    size_t basename_len = strlen(nes_rom_file) - strlen(NES_SUFFIX);
    if(strlen(nes_rom_file) < strlen(NES_SUFFIX) || strcmp(NES_SUFFIX, nes_rom_file + basename_len))
    {
        printf("Please enter a valid nes file with the .nes file extension.\n");
        result = RC_ERR_INVALID_INPUT_FILETYPE;
        goto end;
    }

    // setup basename buffer
    char *nes_rom_file_basename = malloc(basename_len + 1);
    strncpy(nes_rom_file_basename, nes_rom_file, basename_len);
    nes_rom_file_basename[basename_len] = '\0';

    // begin file parsing
    FILE *infile = fopen(nes_rom_file, "r");
    if(!infile)
    {
        printf("Error opening input file\n");
        result = RC_ERR_INFILE_OPEN_ERR;
        goto end;
    }

    // establish mmap to make reading more convenient
    fseek(infile, 0, SEEK_END);
    size_t infile_size = ftell(infile); // TODO handle a too small file given by user
    if(infile_size < 16)
    {
        printf("Given file is too short to contain an iNES header!\n");
        result = RC_ERR_INVALID_INPUT_FILETYPE;
        goto close_infile;
    }
    rewind(infile);
    // do not allow changing infile_mmap pointer, as it is used to close the 
    // mmap at the end of the program.
    const char *infile_mmap = mmap(NULL, infile_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fileno(infile), 0);
    if(infile_mmap == MAP_FAILED)
    {
        printf("Error setting infile mmap\n");
        result = RC_ERR_INFILE_OPEN_ERR;
        goto close_infile;
    }

    // discover header type
    // The order of these checks is important because every valid nes_2 header 
    // is also a valid ines header.
    ines_header_t header;
    parse_result_t parse_result;
    if(header_is_nes_2(infile_mmap))
    {
        if(parse_result = parse_nes_2_header(infile_mmap, &header))
        {
            printf("Error parsing NES 2.0 header: %d\n", parse_result);
            result = RC_ERR_PARSE_ERR;
            goto close_infile_mmap;
        }
    }
    else if(header_is_ines(infile_mmap))
    {
        if(parse_result = parse_ines_header(infile_mmap, &header))
        {
            printf("Error parsing iNES header: %d\n", parse_result);
            result = RC_ERR_PARSE_ERR;
            goto close_infile_mmap;
        }
    }
    else
    {
        printf("NES magic number was not found. This is not a valid .nes file or has been corrupted.\n");
        result = RC_ERR_INFILE_CORRUPTED;
        goto close_infile_mmap;
    }

    if(save_output(&header, infile_mmap, nes_rom_file_basename)) result = RC_ERR_BANK_SAVE_ERR;

close_infile_mmap:
    munmap(infile_mmap, infile_size);
close_infile:
    fclose(infile);
end:
    free(nes_rom_file_basename);
    return result;
}

// the following functions use the tests recommended at:
// https://wiki.nesdev.com/w/index.php/NES_2.0#Identification
bool header_is_ines(char *header_buf)
{
    return !strncmp(NES_MAGIC, header_buf, sizeof(NES_MAGIC));
}

bool header_is_nes_2(char *header_buf)
{
    char header_type_indicator = header_buf[7] & 0x0c;
    return header_is_ines(header_buf) && (header_type_indicator == 0x08);
}

// the following function uses documentation available at:
// https://wiki.nesdev.com/w/index.php/INES
parse_result_t parse_ines_header(char *header_buf, ines_header_t *out)
{
    out->type = IHT_INES;
    out->prg_rom_size = *((uint8_t *)(header_buf + 4));
    out->char_rom_size = *((uint8_t *)(header_buf + 5));
    uint8_t byte6 = *((uint8_t *)(header_buf + 6));
    uint8_t byte7 = *((uint8_t *)(header_buf + 7));
    out->mapper_id = ((byte6 & 0xf0) >> 4) | (byte7 & 0xf0);
    if(byte6 & 0x08) out->ntmt = NMT_NO_MIRRORING;
    else if(byte6 & 0x01) out->ntmt = NMT_VERTICAL;
    else out->ntmt = NMT_HORIZONTAL;
    out->persistent_memory = byte6 & 0x02;
    out->trainer = byte6 & 0x04;
    out->ct = (console_type_t)(byte7 & 0x03);
    return PR_SUCCESS;
}

static uint16_t handle_shift(uint8_t shift)
{
    if(!shift)
    {
        return 0;
    }
    else
    {
        return 1 << (shift - 1);
    }
}

// If the highest nibble is $f, exponent-multiplier notation is used. See:
// https://wiki.nesdev.com/w/index.php/NES_2.0#PRG-ROM_Area
// note that the highest possible value of the exponent is 63, so there is 
// a chance that, with a value of 63 and a multiplier > 1, we will overflow.
// Not sure how to handle that without using a bignum library, but it seems 
// unlikely to overflow in practice.
// In fact, the mmap will fail before this overflows because of the large file 
// size.
static uint64_t handle_rom_size_encoding(uint64_t input_rom_size)
{
    uint16_t irs_u16 = (uint16_t)input_rom_size;
    if(irs_u16 & 0x0f00 == 0x0f00)
    {
        uint8_t exponent = (irs_u16 & 0x00fc) >> 2;
        uint8_t multiplier = irs_u16 & 0x0003;
        return (1 << exponent) * (multiplier * 2 + 1);
    }
    else return input_rom_size;
}

// the following function uses documentation available at:
// https://wiki.nesdev.com/w/index.php/INES
// https://wiki.nesdev.com/w/index.php/NES_2.0
parse_result_t parse_nes_2_header(char *header_buf, ines_header_t *out)
{
    parse_result_t result = parse_ines_header(header_buf, out);
    if(result) return result; // if non-zero (iNES parser had an error), propagate error.
    out->type = IHT_NES_2;
    uint8_t *header_buf_uint8 = (uint8_t *)header_buf;
    out->nes_2.submapper_id = (header_buf_uint8[8] & 0xf0) >> 4;
    out->mapper_id |= ((uint16_t)(header_buf_uint8[8] & 0x0f)) << 8;
    out->prg_rom_size |= ((uint64_t)(header_buf_uint8[9] & 0x0f)) << 8;
    out->prg_rom_size = handle_rom_size_encoding(out->prg_rom_size);
    out->char_rom_size |= ((uint64_t)(header_buf_uint8[9] & 0xf0)) << 4;
    out->char_rom_size = handle_rom_size_encoding(out->char_rom_size);

    out->nes_2.prg_ram_size = handle_shift((uint8_t)(header_buf_uint8[10] & 0x0f));
    out->nes_2.prg_eeprom_size = handle_shift((uint8_t)((header_buf_uint8[10] & 0xf0) >> 4));
    out->nes_2.char_ram_size = handle_shift((uint8_t)(header_buf_uint8[11] & 0x0f));
    out->nes_2.char_eeprom_size = handle_shift((uint8_t)((header_buf_uint8[11] & 0xf0) >> 4));

    out->nes_2.tt = (timing_type_t)(header_buf_uint8[12] & 0x03);

    if(out->ct == CT_VS_SYSTEM)
    {
        out->nes_2.console_type_info.vs_system.ppu_type = header_buf_uint8[13] & 0x0f;
        out->nes_2.console_type_info.vs_system.hardware_type = (header_buf_uint8[13] & 0xf0) >> 4;
    }
    else if(out->ct == CT_EXTENDED)
    {
        out->nes_2.console_type_info.extended_console.type = header_buf_uint8[13] & 0x0f;
    }
    out->nes_2.misc_roms_size = header_buf_uint8[14] & 0x03;
    out->nes_2.default_expansion_device = header_buf_uint8[15] & 0x3f;
    return PR_SUCCESS;
}

static int save_header(char *infile_buf, char *outfile_name)
{
    FILE *outfile = fopen(outfile_name, "w");
    if(!outfile) return 1;
    fwrite(infile_buf, 16, 1, outfile);
    fclose(outfile);
    return 0;
}

// assumes filename_buf contains the base filename and is null-terminated.
static int save_prg_roms(char *prg_roms_start, size_t num_blocks, char *filename_buf)
{
    size_t filename_basename_len = strlen(filename_buf);
    size_t i = 0;
    for(i = 0; i < num_blocks; i++)
    {
        char block_num_str[125];
        sprintf(block_num_str, "%llu", i);
        strcat(filename_buf, block_num_str);
        strcat(filename_buf, ".bin");

        FILE *outfile = fopen(filename_buf, "w");
        if(!outfile) return 1;
        fwrite(prg_roms_start, (size_t)PRG_ROM_BLOCK_SIZE, 1, outfile);
        fclose(outfile);

        prg_roms_start += (size_t)PRG_ROM_BLOCK_SIZE;
        filename_buf[filename_basename_len] = '\0';
    }
    return 0;
}

static int save_char_roms(char *char_roms_start, size_t num_blocks, char *filename_buf)
{
    size_t filename_basename_len = strlen(filename_buf);
    size_t i = 0;
    for(i = 0; i < num_blocks; i++)
    {
        char block_num_str[125];
        sprintf(block_num_str, "%llu", i);
        strcat(filename_buf, block_num_str);
        strcat(filename_buf, ".chr");

        FILE *outfile = fopen(filename_buf, "w");
        if(!outfile) return 1;
        fwrite(char_roms_start, (size_t)CHAR_ROM_BLOCK_SIZE, 1, outfile);
        fclose(outfile);

        char_roms_start += (size_t)CHAR_ROM_BLOCK_SIZE;
        filename_buf[filename_basename_len] = '\0';
    }
    return 0;
}

int save_output(ines_header_t *header, char *infile_buf, char *outfile_base_name)
{
    int result;
    char *outfile_name = malloc(strlen(outfile_base_name) + 129);
    size_t outfile_base_name_len = strlen(outfile_base_name);
    strcpy(outfile_name, outfile_base_name);

    // save header
    strcat(outfile_name, ".ineshdr");
    if(result = save_header(infile_buf, outfile_name)) return result;
    outfile_name[outfile_base_name_len] = '\0'; // ensure next strcat encounters this null character first.

    // save PRG-ROM blocks
    if(result = save_prg_roms(infile_buf + 16, header->prg_rom_size, outfile_name)) return result;

    // save CHR-ROM blocks
    if(result = save_char_roms(infile_buf + 16 + ((size_t)PRG_ROM_BLOCK_SIZE * header->prg_rom_size), header->char_rom_size, outfile_name)) return result;

    return 0;
    // TODO handle the "miscellaneous rom area" described at:
    // https://wiki.nesdev.com/w/index.php/NES_2.0#Miscellaneous_ROM_Area
}