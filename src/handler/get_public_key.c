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
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"
#include "io.h"
#include "buffer.h"
#include "crypto_helpers.h"

#include "get_public_key.h"
#include "../globals.h"
#include "../types.h"
#include "../sw.h"
#include "../ui/display.h"
#include "../helper/send_response.h"

int handler_get_public_key(buffer_t *cdata, bool display) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.req_type = CONFIRM_ADDRESS;
    G_context.state = STATE_NONE;

    if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
        !buffer_read_bip32_path(cdata, G_context.bip32_path, (size_t) G_context.bip32_path_len)) {
        return io_send_sw(SW_WRONG_DATA_LENGTH);
    }

    /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t raw_public_key[1 + 32 + 32];
    /// for public key derivation
    uint8_t chain_code[32];

    cx_err_t error = bip32_derive_with_seed_get_pubkey_256(HDW_ED25519_SLIP10,
                                                           CX_CURVE_Ed25519,
                                                           G_context.bip32_path,
                                                           G_context.bip32_path_len,
                                                           raw_public_key,
                                                           chain_code,
                                                           CX_SHA512,
                                                           NULL,
                                                           0);

    if (error != CX_OK) {
        return io_send_sw(error);
    }

    for (int i = 0; i < PUBKEY_LEN; i++) {
        G_context.public_key[i] = raw_public_key[32 + 32 - i];
    }
    if ((raw_public_key[32] & 1) != 0) {
        G_context.public_key[PUBKEY_LEN - 1] |= 0x80;
    }

    if (display) {
        return ui_display_address();
    }

    return helper_send_response_pubkey();
}