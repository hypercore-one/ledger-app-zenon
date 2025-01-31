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
#include "buffer.h"

#include "deserialize.h"
#include "types.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    if (buf->size != TX_LEN) {
        return WRONG_LENGTH_ERROR;
    }

    // version
    if (!buffer_read_u64(buf, &tx->version, BE)) {
        return PARSING_ERROR;
    }

    // chainIdentifier
    if (!buffer_read_u64(buf, &tx->chainIdentifier, BE)) {
        return PARSING_ERROR;
    }

    // blockType
    if (!buffer_read_u64(buf, &tx->blockType, BE)) {
        return PARSING_ERROR;
    }

    tx->previousHash = (uint8_t *) (buf->ptr + buf->offset);

    // previousHash
    if (!buffer_seek_cur(buf, HASH_LEN)) {
        return PARSING_ERROR;
    }

    // height
    if (!buffer_read_u64(buf, &tx->height, BE)) {
        return PARSING_ERROR;
    }

    tx->momentumAcknowledged = (uint8_t *) (buf->ptr + buf->offset);

    // momentumAcknowledged
    if (!buffer_seek_cur(buf, HASH_LEN + 8)) {
        return PARSING_ERROR;
    }

    tx->address = (uint8_t *) (buf->ptr + buf->offset);

    // address
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return PARSING_ERROR;
    }

    tx->toAddress = (uint8_t *) (buf->ptr + buf->offset);

    // toAddress
    if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
        return PARSING_ERROR;
    }

    tx->amount = (uint8_t *) (buf->ptr + buf->offset);

    // amount
    if (!buffer_seek_cur(buf, 32)) {
        return PARSING_ERROR;
    }

    tx->tokenStandard = (uint8_t *) (buf->ptr + buf->offset);

    // tokenStandard
    if (!buffer_seek_cur(buf, ZTS_LEN)) {
        return PARSING_ERROR;
    }

    tx->fromBlockHash = (uint8_t *) (buf->ptr + buf->offset);

    // fromBlockHash
    if (!buffer_seek_cur(buf, HASH_LEN)) {
        return PARSING_ERROR;
    }

    tx->descendentBlocks = (uint8_t *) (buf->ptr + buf->offset);

    // descendentBlocks
    if (!buffer_seek_cur(buf, HASH_LEN)) {
        return PARSING_ERROR;
    }

    tx->data = (uint8_t *) (buf->ptr + buf->offset);

    // data
    if (!buffer_seek_cur(buf, HASH_LEN)) {
        return PARSING_ERROR;
    }

    // fusedPlasma
    if (!buffer_read_u64(buf, &tx->fusedPlasma, BE)) {
        return PARSING_ERROR;
    }

    // difficulty
    if (!buffer_read_u64(buf, &tx->difficulty, BE)) {
        return PARSING_ERROR;
    }

    // nonce
    if (!buffer_read_u64(buf, &tx->nonce, BE)) {
        return PARSING_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
