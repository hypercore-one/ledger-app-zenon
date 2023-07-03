#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define TX_LEN      306
#define ADDRESS_LEN 20
#define HASH_LEN    32
#define ZTS_LEN     10

typedef enum {
    PARSING_OK = 1,          /// transaction parsing successful
    PARSING_ERROR = -1,      /// transaction parsing error
    WRONG_LENGTH_ERROR = -7  /// invalid transaction length
} parser_status_e;

typedef struct {
    uint64_t version;               /// version (8 bytes)
    uint64_t chainIdentifier;       /// chainIdentifier (8 bytes)
    uint64_t blockType;             /// blockType (8 bytes)
    uint8_t *previousHash;          /// pointer to previousHash (32 bytes)
    uint64_t height;                /// height (8 bytes)
    uint8_t *momentumAcknowledged;  /// pointer to momentumAcknowledged (32 + 8 bytes)
    uint8_t *address;               /// pointer to address (20 bytes)
    uint8_t *toAddress;             /// pointer to address (20 bytes)
    uint8_t *amount;                /// pointer to amount (32 bytes)
    uint8_t *tokenStandard;         /// pointer to tokenStandard value (10 bytes)
    uint8_t *fromBlockHash;         /// pointer to fromBlockHash (32 bytes)
    uint8_t *descendentBlocks;      /// pointer to descendentBlocks (32 bytes)
    uint8_t *data;                  /// pointer to data (32 bytes)
    uint64_t fusedPlasma;           /// fusedPlasma (8 bytes)
    uint64_t difficulty;            /// difficulty (8 bytes)
    uint64_t nonce;                 /// nonce (8 bytes)
} transaction_t;
