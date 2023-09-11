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

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memmove

#include "write.h"

#include "serialize.h"

int transaction_serialize(const transaction_t *tx, uint8_t *out, size_t out_len) {
    size_t offset = 0;

    if (TX_LEN > out_len) {
        return -1;
    }

    // version
    write_u64_be(out, offset, tx->version);
    offset += 8;

    // chainIdentifier
    write_u64_be(out, offset, tx->chainIdentifier);
    offset += 8;

    // blockType
    write_u64_be(out, offset, tx->blockType);
    offset += 8;

    // previousHash
    memmove(out + offset, tx->previousHash, HASH_LEN);
    offset += HASH_LEN;

    // height
    write_u64_be(out, offset, tx->height);
    offset += 8;

    // momentumAcknowledged
    memmove(out + offset, tx->momentumAcknowledged, HASH_LEN + 8);
    offset += HASH_LEN + 8;

    // address
    memmove(out + offset, tx->address, ADDRESS_LEN);
    offset += ADDRESS_LEN;

    // toAddress
    memmove(out + offset, tx->toAddress, ADDRESS_LEN);
    offset += ADDRESS_LEN;

    // amount
    memmove(out + offset, tx->amount, 32);
    offset += 32;

    // tokenStandard
    memmove(out + offset, tx->tokenStandard, ZTS_LEN);
    offset += ZTS_LEN;

    // fromBlockHash
    memmove(out + offset, tx->fromBlockHash, HASH_LEN);
    offset += HASH_LEN;

    // descendentBlocks
    memmove(out + offset, tx->descendentBlocks, HASH_LEN);
    offset += HASH_LEN;

    // data
    memmove(out + offset, tx->data, HASH_LEN);
    offset += HASH_LEN;

    // fusedPlasma
    write_u64_be(out, offset, tx->fusedPlasma);
    offset += 8;

    // difficulty
    write_u64_be(out, offset, tx->difficulty);
    offset += 8;

    // nonce
    write_u64_be(out, offset, tx->nonce);
    offset += 8;

    return (int) offset;
}
