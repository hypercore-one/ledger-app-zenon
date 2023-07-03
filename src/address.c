/*****************************************************************************
 *   Ledger App Zenon.
 *   (c) 2023 Zenon Community.
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

#include <stdint.h>  // uint*_t
#include <stddef.h>  // size_t
#include <string.h>  // memmove

#include "os.h"
#include "cx.h"

#include "address.h"
#include "segwit_addr.h"

#include "transaction/types.h"

bool address_from_pubkey(const uint8_t *public_key, char *out, size_t out_len) {
    uint8_t hash[HASH_LEN] = {0};
    cx_sha3_t sha3256;

    if (cx_sha3_init_no_throw(&sha3256, 256) != CX_OK) {
        return false;
    }

    if (cx_hash_no_throw((cx_hash_t *) &sha3256, CX_LAST, public_key, 32, hash, HASH_LEN) !=
        CX_OK) {
        return false;
    }

    uint8_t core[ADDRESS_LEN] = {0};
    core[0] = 0;  // user byte
    memmove(core + 1, hash, ADDRESS_LEN - 1);

    return address_encode(core, out, out_len);
}

bool address_encode(const uint8_t *prog, char *out, size_t out_len) {
    char bech32[73 + 1] = {0};

    if (41 > out_len) {
        return false;
    }

    if (bech32_addr_encode(bech32, "z", prog, ADDRESS_LEN) != 1) {
        return false;
    }

    memmove(out, bech32, out_len - 1);

    out[41] = '\0';  // null terminated

    return true;
}