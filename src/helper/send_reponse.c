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

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove

#include "buffer.h"

#include "send_response.h"
#include "../constants.h"
#include "../globals.h"
#include "../sw.h"

int helper_send_response_pubkey() {
    uint8_t resp[1 + PUBKEY_LEN] = {0};
    size_t offset = 0;

    resp[offset++] = PUBKEY_LEN;
    memmove(resp + offset, G_context.public_key, PUBKEY_LEN);
    offset += PUBKEY_LEN;

    return io_send_response_pointer(resp, offset, SW_OK);
}

int helper_send_response_sig() {
    uint8_t resp[1 + SIG_LEN] = {0};
    size_t offset = 0;

    resp[offset++] = G_context.tx_info.signature_len;
    memmove(resp + offset, G_context.tx_info.signature, G_context.tx_info.signature_len);
    offset += G_context.tx_info.signature_len;

    return io_send_response_pointer(resp, offset, SW_OK);
}
