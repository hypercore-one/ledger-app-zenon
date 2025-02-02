/*****************************************************************************
 *   Ledger App Zenon.
 *   (c) 2025 Zenon Community.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdint.h>   // uint*_t
#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <string.h>   // memcpy

#include "cx.h"

#include "address.h"
#include "segwit_addr.h"
#include "transaction/tx_types.h"

bool address_from_pubkey(const uint8_t *public_key, char *out, size_t out_len) {
    uint8_t hash[HASH_LEN];

    // Initialize SHA3-256 hashing context, return false if unsuccessful
    cx_sha3_t sha3256;
    if (cx_sha3_init_no_throw(&sha3256, 256) != CX_OK) {
        return false;
    }

    // Hash the public key using SHA3-256, return false if unsuccessful
    if (cx_hash_no_throw((cx_hash_t *) &sha3256, CX_LAST, public_key, 32, hash, HASH_LEN) !=
        CX_OK) {
        return false;
    }

    // Prepare address structure
    uint8_t core[ADDRESS_LEN];
    core[0] = 0;  // First byte (user address byte)
    // Copy the first 19 bytes of the hashed key into the address structure
    memcpy(core + 1, hash, ADDRESS_LEN - 1);

    // Encode the address into the out buffer
    return address_encode(core, out, out_len);
}

bool address_encode(const uint8_t *prog, char *out, size_t out_len) {
    // Ensure the output buffer is large enough
    if (41 > out_len) {
        return false;
    }

    // Encode the given prog array into a Bech32 format and store it
    // into the bech32 character array
    char bech32[73];
    if (bech32_addr_encode(bech32, "z", prog, ADDRESS_LEN) != 1) {
        return false;  // Bech32 encoding failed
    }

    // Copy the first 40 characters of the Bech32 address into the out buffer
    memcpy(out, bech32, 40);  // Copy 40 characters
    out[40] = '\0';           // Null terminate

    return true;
}