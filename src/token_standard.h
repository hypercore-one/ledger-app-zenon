#pragma once

#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

typedef enum {
    ZNN,  // ZNN Token Standard
    QSR,  // QSR Token Standard
    ZTS
} zts_t;

/**
 * Get the zenon token standard (zts) type.
 *
 * @param[in]  prog
 *   Pointer to byte buffer for the witness program (10 bytes).
 *
 * @return zts_t.
 *
 */
zts_t token_standard_type(const uint8_t *prog);

/**
 * Encode a zenon token standard (zts) from witness program using bech32.
 *
 * address = Bech32("zts", prog)[0:26]
 *
 * @param[in]  prog
 *   Pointer to byte buffer for the witness program (10 bytes).
 * @param[out] out
 *   Pointer to output char buffer for address.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool token_standard_encode(const uint8_t *prog, char *out, size_t out_len);
