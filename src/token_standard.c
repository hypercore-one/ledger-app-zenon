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

#include "token_standard.h"
#include "segwit_addr.h"

#include "transaction/types.h"

zts_t token_standard_type(const uint8_t *prog) {
    const uint8_t znnZtsBytes[] = {0x14, 0xe6, 0x63, 0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6};
    const uint8_t znnQsrBytes[] = {0x04, 0x06, 0x63, 0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6};
    if (memcmp(prog, znnZtsBytes, sizeof(znnZtsBytes)) == 0) {
        return ZNN;
    } else if (memcmp(prog, znnQsrBytes, sizeof(znnQsrBytes)) == 0) {
        return QSR;
    } else {
        return ZTS;
    }
}

bool token_standard_encode(const uint8_t *prog, char *out, size_t out_len) {
    char bech32[73 + 1] = {0};

    if (27 > out_len) {
        return false;
    }

    if (bech32_addr_encode(bech32, "zts", prog, ZTS_LEN) != 1) {
        return false;
    }

    memmove(out, bech32, out_len - 1);

    out[27] = '\0';  // null terminated

    return true;
}