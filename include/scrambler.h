#ifndef SCRAMBLER_H
#define SCRAMBLER_H 0

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Utility

enum _PRINT_TYPE {
    P_C,
    P_X,
    P_I
};
typedef enum _PRINT_TYPE PRINT_TYPE;

void printHex(void *ptr, size_t size, PRINT_TYPE t);

// # Definitions

// Settings contain the algorithm constants
struct _s_settings {
    size_t SAMPLING_FREQUENCY;
    size_t PACKET_LEN;
    size_t PACKETS_IN_BLOCK;
};
typedef struct _s_settings s_settings;
// The main structure
struct _s_scrambler {
    // all private!
    s_settings _settings;
};
typedef struct _s_scrambler s_scrambler;
// Presets can be used only if you guarantee it won`t be changed
// Never use in different threads
struct _s_preset {
    uint8_t *input;
    size_t i_len;
    uint8_t *output;
    size_t o_len;
    uint16_t *key;
    uint16_t _block_offset;
};
typedef struct _s_preset s_preset;
// Typical errors
enum _S_ERROR {
    OK = 0,
    NULL_INPUT   = 0x4c554e, // = NUL
    SMALL_OUTPUT = 0x4c4d53, // = SML
    BAD_INPUT    = 0x444142  // = BAD
};
typedef enum _S_ERROR S_ERROR;
// Validation errors
enum _S_TODO_VALID {
    ALL_SET = 0,
    UNSET_ALL = 0xFFFF,
    UNSET_INPUT = 1,
    UNSET_OUTPUT = 2,
    UNSET_SAMPLING_FREQUENCY = 4,
    UNSET_PACKET_LEN = 8,
    UNSET_PACKETS_IN_BLOCK = 16,
    UNSET_KEY = 32,
    SMALL_OUTPUT_LEN = 64
};

// # Init/clear functions

// Init, with or without settings
S_ERROR s_init_scrambler (s_scrambler *to_fill);
S_ERROR s_init_scrambler_set (s_scrambler *to_fill, s_settings settings);
// Init settings
S_ERROR s_clear_settings (s_settings *s);
S_ERROR s_clear_preset (s_preset *p);

// # Main functions

// b_blocks - number of blocks to process per call
S_ERROR s_encrypt_io (size_t b_blocks, uint16_t *key, s_scrambler *s, uint8_t *input, size_t i_len, uint8_t *output, size_t o_len, uint8_t *buffer);
S_ERROR s_encrypt_preset (size_t b_blocks, s_preset *presets, s_scrambler *s, uint8_t *buffer);

// b_blocks - number of blocks to process per call
S_ERROR s_decrypt_io (size_t b_blocks, uint16_t *key, s_scrambler *s, uint8_t *input, size_t i_len, uint8_t *output, size_t o_len, uint8_t *buffer);
S_ERROR s_decrypt_preset (size_t b_blocks, s_preset *presets, s_scrambler *s, uint8_t *buffer);

// Call this if you are not sure
int s_is_preset_finished (s_preset *p, s_scrambler *s);

// # Validators

uint16_t s_is_invalid (s_scrambler *s);
uint16_t s_is_invalid_setting (s_settings *s);
uint16_t s_is_invalid_preset (s_preset *f, s_scrambler *s);

#endif
