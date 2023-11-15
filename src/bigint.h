#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

typedef struct {
    int32_t sign;
    uint32_t bits[8];
    uint8_t bits_len;
} bigint_t;

int bigint_from_bytes(const uint8_t *in,
                      size_t in_len,
                      bool isUnsigned,
                      bool isBigEndian,
                      bigint_t *out);

int bigint_format(const bigint_t in, int8_t decimals, char *out, size_t out_len);

uint64_t make_uint64(uint32_t hi, uint32_t lo);

void substring(char s[], char sub[], int p, int l);

void number_format(char val[], int8_t decimals, char *out, size_t out_len);
