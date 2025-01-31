#pragma once

#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool

/**
 * Encodes a zenon address from public key using bech32.
 *
 * address = Bech32("z", 0 + SHA3256(public_key)[1:19])[0:40]
 *
 * @param[in]  public_key
 *   Pointer to byte buffer with public key (32 bytes).
 * @param[out] out
 *   Pointer to output char buffer for address.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool address_from_pubkey(const uint8_t *public_key, char *out, size_t out_len);

/**
 * Encode a zenon address from witness program using bech32.
 *
 * address = Bech32("z", prog)[0:40]
 *
 * @param[in]  prog
 *   Pointer to byte buffer for the witness program (20 bytes).
 * @param[out] out
 *   Pointer to output char buffer for address.
 * @param[in]  out_len
 *   Length of output byte buffer.
 *
 * @return true if success, false otherwise.
 *
 */
bool address_encode(const uint8_t *prog, char *out, size_t out_len);
