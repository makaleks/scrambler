#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

const size_t SAMPLING_FREQUENCY = 8000;
const size_t PACKET_LEN = 100;
const size_t PACKETS_IN_BLOCK = 40;
const size_t block_size = 4000;

void gen_key(uint16_t *key, size_t key_length) {
    if (NULL == key)
        return;
    size_t i = 0;
    uint16_t val = 0;
    size_t j = 0;
    int toAdd = 1;
    memset(key, 0xFFFF, sizeof(*key)*key_length);
    srand(time(NULL));
    for (i = 0; i < key_length; i++) {
        toAdd = 1;
        while (toAdd) {
            j = rand() % key_length;
            while (j < key_length && toAdd) {
                if (key[j] == 0xFFFF) {
                    toAdd = 0;
                    key[j] = i;
                    break;
                }
                j += toAdd;
            }
            if (0 == toAdd) break;
            else toAdd *= -1;
        }
        j = 0;
    }
}

#include "scrambler.h"

void print_key(uint16_t *key, size_t key_len) {
    printf("{ ");
    for (size_t i = 0; i < key_len; i++)
        printf("%hu%s ", key[i], i == key_len-1 ? "" : ",");
    printf("}");
}

int main() {
    char str[] = "The blockchain technology has become quite famous in recent years. The fame was earned by various projects like Onename, OpenBazaar, Gems, Firechat and Bitcoin. All this projects use blockchain to store data. In recent years a one more appliance of this technology is actively developed - the code execution. It is known as 'smart contracts'. This paper covers the way smart contracts work and the problems solved by modern projects, introduction to the blockchain is also provided. It may be used by developers who want to get acquainted to this new technology and create a new kind of applications.\nA smart contract is a concept of protocol that can execute the terms of a contract. It`s aimed to be self-enforced and self-executed to minimize the costs of trusted intermediaries and the occurrence of exceptions: accidental or malicious. The blockchain ideas caused a huge boost in smart contracts development, so nearly all smart contracts today use blockchain context.\n";
    size_t str_len = strlen(str);
    const size_t BUF_SIZE = 12000;
    uint8_t encrypted[BUF_SIZE];
    uint8_t decrypted[BUF_SIZE];
    memset(encrypted, 0, BUF_SIZE);
    memset(decrypted, 0, BUF_SIZE);
    s_scrambler s;
    s_settings settings;
    s_clear_settings(&settings);
    settings.SAMPLING_FREQUENCY = SAMPLING_FREQUENCY;
    settings.PACKET_LEN = PACKET_LEN;
    settings.PACKETS_IN_BLOCK = PACKETS_IN_BLOCK;

    s_preset in;
    s_preset out;
    s_clear_preset(&in);
    s_clear_preset(&out);
    in.input = str;
    in.i_len = str_len;
    in.output = encrypted;
    in.o_len = BUF_SIZE;
    out.input = encrypted;
    out.i_len = BUF_SIZE;
    out.output = decrypted;
    out.o_len = BUF_SIZE;

    uint16_t key[PACKETS_IN_BLOCK];
    gen_key(key, PACKETS_IN_BLOCK);
    in.key = key;
    out.key = key;
    puts("# key: ");
    print_key(key, PACKETS_IN_BLOCK);puts("");

    S_ERROR err = OK;
    err = s_init_scrambler_set(&s, settings);
    if (err)
        puts((char*)(err));
    size_t blocks = (size_t)(ceil(((double)str_len)/(PACKETS_IN_BLOCK*PACKET_LEN)));

    uint8_t buffer[block_size];
    printf("\n# Before:\n  %s\n", str);
    err = s_encrypt_preset(blocks, &in, &s, buffer);
    //printHex(&err, sizeof(err), P_I);puts("");
    if (err)
        puts((char*)(&err));
    err = s_decrypt_preset(blocks, &out, &s, buffer);
    printf("\n# After:\n  %s\n", decrypted);
    if (err)
        puts((char*)(err));//*/
    return 0;
}
