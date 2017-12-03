#include "scrambler.h"

#include <string.h>

// # Utility

inline size_t get_block_size (s_scrambler *s) {
    return s->_settings.PACKETS_IN_BLOCK * s->_settings.PACKET_LEN;
}

// # Validators

uint16_t s_is_invalid (s_scrambler *s) {
    if (NULL == s)
        return UNSET_ALL;
    return s_is_invalid_setting(&s->_settings);
}

uint16_t s_is_invalid_setting (s_settings *s) {
    if (NULL == s)
        return UNSET_ALL;
    uint16_t error = ALL_SET;
    if (0 == s->SAMPLING_FREQUENCY)
        error |= UNSET_SAMPLING_FREQUENCY;
    if (0 == s->PACKET_LEN)
        error |= UNSET_PACKET_LEN;
    if (0 == s->PACKETS_IN_BLOCK)
        error |= UNSET_PACKETS_IN_BLOCK;
    return error;
}

uint16_t s_is_invalid_s_preset (s_preset *f, s_scrambler *s) {
    if (NULL == f)
        return UNSET_ALL;
    uint16_t error = ALL_SET;
    if (NULL == f->input || 0 == f->i_len)
        error |= UNSET_INPUT;
    if (NULL == f->output || 0 == f->o_len)
        error |= UNSET_OUTPUT;
    if (NULL == f->key)
        error |= UNSET_KEY;
    size_t d_i = f->i_len / get_block_size(s);
    size_t f_i = f->i_len % get_block_size(s);
    size_t d_o = f->o_len / get_block_size(s);
    if ((d_i > d_o)
            || (d_i == d_o && f_i != 0))
        error |= SMALL_OUTPUT_LEN;
    return error;
}

// # Clear

S_ERROR s_clear_settings (s_settings *s) {
    if (NULL == s)
        return NULL_INPUT;
    s->SAMPLING_FREQUENCY = 0;
    s->PACKET_LEN = 0;
    s->PACKETS_IN_BLOCK = 0;
    return OK;
}

S_ERROR s_clear_preset (s_preset *p) {
    if (NULL == p)
        return NULL_INPUT;
    p->input = p->output = NULL;
    p->i_len = p->o_len = p->_block_offset = 0;
    return OK;
}

int s_is_preset_finished (s_preset *p, s_scrambler *s) {
    size_t position = p->_block_offset * get_block_size(s);
    if (position + get_block_size(s) > p->o_len && position < p->o_len) {
        for (size_t i = position; i < p->o_len; i++)
            p->output[i] = 0;
        return 1;
    }
    return 0;
}

// # Main - scrambler-only

S_ERROR s_init_scrambler (s_scrambler *to_fill) {
    if (NULL == to_fill)
        return NULL_INPUT;
    s_clear_settings(&to_fill->_settings);
    return OK;
}

S_ERROR s_init_scrambler_set (s_scrambler *to_fill, s_settings settings) {
    if (NULL == to_fill)
        return NULL_INPUT;
    s_init_scrambler(to_fill);
    uint16_t error_validate = s_is_invalid_setting(&settings);
    if (ALL_SET != error_validate)
        return BAD_INPUT;
    to_fill->_settings = settings;
    return OK;
}

S_ERROR s_encrypt_io (size_t b_blocks, uint16_t *key, s_scrambler *s, uint8_t *input, size_t i_len, uint8_t *output, size_t o_len) {
    if (0 == b_blocks || NULL == key || NULL == s || NULL == input || NULL == output)
        return NULL_INPUT;
    s_preset conf;
    s_clear_preset(&conf);
    conf.input = input, conf.i_len = i_len;
    conf.output = output, conf.o_len = o_len;
    conf.key = key;
    return s_encrypt_preset(b_blocks, &conf, s);
}

static void encrypt (size_t packet_len, uint8_t *input, uint8_t *output, uint16_t *key, size_t packets_in_block) {
    for (size_t i = 0; i < packets_in_block; i++) {
        memcpy(output + key[i]*packet_len, input + i*packet_len, packet_len);
    }
}

static void decrypt (size_t packet_len, uint8_t *input, uint8_t *output, uint16_t *key, size_t packets_in_block) {
    for (size_t i = 0; i < packets_in_block; i++) {
        memcpy(output + i*packet_len, input + key[i]*packet_len, packet_len);
    }
}

static S_ERROR process_all (size_t b_blocks, s_preset *presets, s_scrambler *s, void(*decrypt)(size_t,uint8_t*,uint8_t*,uint16_t*,size_t)) {
    if (0 == b_blocks || NULL == presets || NULL == s)
        return NULL_INPUT;
    size_t block_size = get_block_size(s);
    size_t i = presets->_block_offset * block_size;
    for (size_t step = 0; step < b_blocks; step++) {
        if (i < presets->i_len && i + block_size > presets->o_len)
            if (i < presets->o_len)
                memset(presets->output + i, 0, presets->o_len - i);
            return SMALL_OUTPUT;
            encrypt(s->_settings.PACKET_LEN, presets->input, presets->output, presets->key, s->_settings.PACKETS_IN_BLOCK);
        i += block_size;
    }
    return OK;
}

S_ERROR s_encrypt_preset (size_t b_blocks, s_preset *presets, s_scrambler *s) {
    return process_all(b_blocks, presets, s, encrypt);
}

S_ERROR s_decrypt_preset (size_t b_blocks, s_preset *presets, s_scrambler *s) {
    return process_all(b_blocks, presets, s, decrypt);
}
