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

#ifdef HAVE_BAGL

#pragma GCC diagnostic ignored "-Wformat-invalid-specifier"  // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"         // snprintf

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"
#include "io.h"
#include "bip32.h"
#include "format.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "../sw.h"
#include "../address.h"
#include "../token_standard.h"
#include "../bigint.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../menu.h"

static action_validate_cb g_validate_callback;
static char g_type[8];
static char g_hash[65];
static char g_amount[84];
static char g_bip32_path[20];
static char g_address[41];

// Validate/Invalidate public key and go back to home
static void ui_action_validate_pubkey(bool choice) {
    validate_pubkey(choice);
    ui_menu_main();
}

// Validate/Invalidate transaction and go back to home
static void ui_action_validate_transaction(bool choice) {
    validate_transaction(choice);
    ui_menu_main();
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm Address"});
// Step with title/text for BIP32 path
UX_STEP_NOCB(ux_display_path_step,
             bnnn_paging,
             {
                 .title = "Path",
                 .text = g_bip32_path,
             });
// Step with title/text for address
UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = g_address,
             });
// Step with approve button
UX_STEP_CB(ux_display_approve_step,
           pb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_reject_step,
           pb,
           (*g_validate_callback)(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

// FLOW to display address and BIP32 path:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display BIP32 Path
// #3 screen: display address
// #4 screen: approve button
// #5 screen: reject button
UX_FLOW(ux_display_pubkey_flow,
        &ux_display_confirm_addr_step,
        &ux_display_path_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_address() {
    if (G_context.req_type != CONFIRM_ADDRESS || G_context.state != STATE_NONE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_bip32_path, 0, sizeof(g_bip32_path));
    if (!bip32_path_format(G_context.bip32_path,
                           G_context.bip32_path_len,
                           g_bip32_path,
                           sizeof(g_bip32_path))) {
        return io_send_sw(SW_DISPLAY_BIP32_PATH_FAIL);
    }

    memset(g_address, 0, sizeof(g_address));
    if (!address_from_pubkey(G_context.public_key, g_address, sizeof(g_address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_pubkey;

    ux_flow_init(0, ux_display_pubkey_flow, NULL);
    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "Transaction",
             });
// Step with title/text for type
UX_STEP_NOCB(ux_display_type_step,
             bn,
             {
                 "Type",
                 g_type,
             });
// Step with title/text for amount
UX_STEP_NOCB(ux_display_amount_step,
             bnnn_paging,
             {
                 .title = "Amount",
                 .text = g_amount,
             });
// Step with title/text for hash
UX_STEP_NOCB(ux_display_hash_step,
             bnnn_paging,
             {
                 .title = "Hash",
                 .text = g_hash,
             });

// FLOW to display send transaction information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display type
// #3 screen : display amount
// #4 screen : display destination address
// #5 screen : approve button
// #6 screen : reject button
UX_FLOW(ux_display_send_transaction_flow,
        &ux_display_review_step,
        &ux_display_type_step,
        &ux_display_amount_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

// FLOW to display receive transaction information:
// #1 screen : eye icon + "Review Transaction"
// #2 screen : display type
// #3 screen : display hash
// #5 screen : approve button
// #6 screen : reject button
UX_FLOW(ux_display_receive_transaction_flow,
        &ux_display_review_step,
        &ux_display_type_step,
        &ux_display_hash_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_transaction() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (G_context.tx_info.transaction.blockType == 2 ||
        G_context.tx_info.transaction.blockType == 4) {
        // SEND TX
        snprintf(g_type, sizeof(g_type), "Send");
        PRINTF("Type: %s\n", g_type);

        bigint_t val;
        if (bigint_from_bytes(G_context.tx_info.transaction.amount, 32, true, true, &val) != 0) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
        }

        zts_t zts = token_standard_type(G_context.tx_info.transaction.tokenStandard);
        char symbol[4];
        int8_t decimals = 8;
        if (zts == ZNN) {
            strcpy(symbol, "ZNN");
        } else if (zts == QSR) {
            strcpy(symbol, "QSR");
        } else {
            strcpy(symbol, "ZTS");
            decimals = 0;
        }

        memset(g_amount, 0, sizeof(g_amount));
        char amount[81] = {0};
        if (bigint_format(val, decimals, amount, sizeof(amount)) < 0) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
        }

        snprintf(g_amount, sizeof(g_amount), "%.*s %s", strlen(amount) - 1, amount, symbol);
        PRINTF("Amount: %s\n", g_amount);

        memset(g_address, 0, sizeof(g_address));
        if (!address_encode(G_context.tx_info.transaction.toAddress,
                            g_address,
                            sizeof(g_address))) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
        PRINTF("Address: %s\n", g_address);

        g_validate_callback = &ui_action_validate_transaction;

        ux_flow_init(0, ux_display_send_transaction_flow, NULL);
    } else if (G_context.tx_info.transaction.blockType == 1 ||
               G_context.tx_info.transaction.blockType == 3 ||
               G_context.tx_info.transaction.blockType == 5) {
        // RECEIVE TX
        snprintf(g_type, sizeof(g_type), "Receive");
        PRINTF("Type: %s\n", g_type);

        memset(g_hash, 0, sizeof(g_hash));
        snprintf(g_hash, sizeof(g_hash), "0x%.*H", HASH_LEN, G_context.tx_info.m_hash);

        g_validate_callback = &ui_action_validate_transaction;

        ux_flow_init(0, ux_display_receive_transaction_flow, NULL);
    } else {
        // UNKNOWN TX
        return io_send_sw(SW_BAD_STATE);
    }

    return 0;
}

#endif
