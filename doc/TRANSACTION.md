# Zenon Transaction Serialization

## Overview

[Zenon Network](https://github.com/zenon-network/go-zenon), the Network of Momentum (NoM), serialises transactions into raw byte streams which are then digested via SHA3-256 to produce a signing message hash. In the case of the Zenon Ledger App, serialised transactions are first sent via APDU as the `CData`, then deserialised for validation and user approval before construction and signing of the message hash. The sections below provide a brief overview of how transactions and their associated values are constructed; for more in-depth information, please visit [Zenon's website](https://zenon.network).

## Amount units

The Network of Momentum has a dual-coin architecture which consists of two coins: [$ZNN](https://www.zenon.org/phases/1/tokens/znn?ref=zenon.info) and [$QSR](https://www.zenon.org/phases/1/tokens/qsr?ref=zenon.info). ZNN and QSR are both part of the Zenon Token Standard (ZTS), which is a programmable token standard. Both coins have a precision of 8 decimals. Zenon transaction make use of raw 256 bit currency values obtained by multiplying the amount by `10^8` as exampled in the table below.

| Amount | ZNN | QSR | ZTS |
|--|--|--|--|
| 1 | 0.00000001 | 0.00000001 | 1 |
| 100000000 | 1 | 1 | 100000000 |
| 999999999 | 9.99999999 | 9.99999999 | 999999999 |

> The decimals precision and symbol of ZTS tokens is programmable. The token information is currently not available in the Zenon Ledger App. Therefore the raw amount and the ZTS symbol is displayed when signing a transaction containing a ZTS token.

## Address format

Zenon addresses are Bech32 encoded with human readable part `z`, identifiers are derived from the first 19 bytes of the SHA3-256 hash of the Ed25519 public key and are prefixed with a user account `0x00` or an embedded contract `0x01` byte.

## Structure

### Transaction

| Field | Size (bytes) | Description |
| --- | :---: | --- |
| `version` | 8 | A protocol version number |
| `chainIdentifier` | 8 | The chain identifier |
| `blockType` | 8 | A number specifying the block type |
| `previousHash` | 32 | The hash of the previous transaction |
| `height` | 8 | A number specifying the account block height |
| `momentumAcknowledged` | 32 + 8 | The hash of the acknowledgement momentum followed by the momentum height |
| `address` | 20 | The source address |
| `toAddress` | 20 | The destination address in case of a send transaction; otherwise empty |
| `amount` | 32 | The amount to send to the destination address in case of a send transaction; otherwise empty |
| `tokenStandard` | 10 | The address of the token standard |
| `fromBlockHash` | 32 | The hash of the from block in case of a receive transaction |
| `descendentBlocks` | 32 | The hash of the descendent blocks |
| `data` | 32 | The hash of the data |
| `fusedPlasma` | 8 | The fused plasma used to process the transaction |
| `difficulty` | 8 | The difficulty used to calculate the nonce |
| `nonce` | 8 | A number that meets the difficulty |

### Signature

Deterministic signature scheme [EdDSA](https://en.wikipedia.org/wiki/EdDSA) is used to sign a transaction on the [Curve25519](https://en.wikipedia.org/wiki/Curve25519) curve using [SHA-512](https://en.wikipedia.org/wiki/SHA-2). A Zenon transaction is signed by constructing a SHA3-256 hash of the serialized transaction `sha3-256(serialized_tx)`; this hash is subsequently signed producing a 64-byte signature.

### Feeless

Zenon Token Standard or short ZTS is a specification for creating tokens. ZTS tokens are native to the protocol level. ZTS tokens inherit the feeless properties of ZNN and QSR: transferring tokens instantly without fees.

## Links

- Learn | [Zenon.org](https://zenon.org/?ref=zenon.info) & [Zenon.Network](https://zenon.network/?ref=zenon.info)
- White Paper | [Zenon Network White Paper](https://github.com/zenon-network/zenon.network/releases/download/whitepaper/whitepaper.pdf?ref=zenon.info)