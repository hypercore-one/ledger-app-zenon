#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

extern "C" {
#include "common/buffer.h"
#include "common/format.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"
#include "address.h"
#include "token_standard.h"
#include "bigint.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_t tx;
    parser_status_e status;

    char version[21] = {0};
    char chainIdentifier[21] = {0};
    char blockType[21] = {0};
    char previousHash[65] = {0};
    char height[21] = {0};
    char momentumAcknowledgedHash[65] = {0};
    char momentumAcknowledgedHeight[21] = {0};
    char address[41] = {0};
    char toAddress[41] = {0};
    char amount[82] = {0};
    char tokenStandard[27] = {0};
    char fromBlockHash[65] = {0};
    char descendentBlocks[65] = {0};
    char data[65] = {0};
    char fusedPlasma[21] = {0};
    char difficulty[21] = {0};
    char nonce[21] = {0};

    memset(&tx, 0, sizeof(tx));

    status = transaction_deserialize(&buf, &tx);

    if (status == PARSING_OK) {
        /// version (8 bytes)
        format_u64(version, sizeof(version), tx.version);
        printf("version: %s\n", version);
        /// chainIdentifier (8 bytes)
        format_u64(chainIdentifier, sizeof(chainIdentifier), tx.chainIdentifier);
        printf("chainIdentifier: %s\n", chainIdentifier);
        /// blockType (8 bytes)
        format_u64(blockType, sizeof(blockType), tx.blockType);
        printf("blockType: %s\n", blockType);
        /// previousHash (32 bytes)
        format_hex(tx.previousHash, HASH_LEN, previousHash, sizeof(previousHash));
        printf("previousHash: %s\n", previousHash);
        // height (8 bytes)
        format_u64(height, sizeof(height), tx.height);
        printf("height: %s\n", height);
        // momentumAcknowledged (32 + 8 bytes)
        format_hex(tx.momentumAcknowledged, HASH_LEN, momentumAcknowledgedHash, sizeof(momentumAcknowledgedHash));
        printf("momentumAcknowledgedHash: %s\n", momentumAcknowledgedHash);
        format_u64(momentumAcknowledgedHeight, sizeof(momentumAcknowledgedHeight), tx.momentumAcknowledged + HASH_LEN);
        printf("momentumAcknowledgedHeight: %s\n", momentumAcknowledgedHeight);
        // address (20 bytes)
        address_encode(tx.address, address, sizeof(address));
        printf("address: %s\n", address);
        // toAddress (20 bytes)
        address_encode(tx.toAddress, toAddress, sizeof(toAddress));
        printf("toAddress: %s\n", address);
        // amount (32 bytes)
        bigint_t val;
        bigint_from_bytes(tx.amount, 32, true, true, &val);
        bigint_format(val, 0, amount, sizeof(amount));
        // tokenStandard value (10 bytes)
        token_standard_encode(tx.tokenStandard, tokenStandard, sizeof(tokenStandard));
        printf("tokenStandard: %s\n", tokenStandard);
        /// fromBlockHash (32 bytes)
        format_hex(tx.fromBlockHash, HASH_LEN, fromBlockHash, sizeof(fromBlockHash));
        printf("fromBlockHash: %s\n", fromBlockHash);
        /// fromBlockHash (32 bytes)
        format_hex(tx.descendentBlocks, HASH_LEN, descendentBlocks, sizeof(descendentBlocks));
        printf("descendentBlocks: %s\n", descendentBlocks);
        /// data (32 bytes)
        format_hex(tx.data, HASH_LEN, data, sizeof(data));
        printf("data: %s\n", data);
        // fusedPlasma (8 bytes)
        format_u64(fusedPlasma, sizeof(fusedPlasma), tx.fusedPlasma);
        printf("fusedPlasma: %s\n", fusedPlasma);
        // difficulty (8 bytes)
        format_u64(difficulty, sizeof(difficulty), tx.difficulty);
        printf("difficulty: %s\n", difficulty);
        // nonce (8 bytes)
        format_u64(nonce, sizeof(nonce), tx.nonce);
        printf("nonce: %s\n", nonce);
    }

    return 0;
}
