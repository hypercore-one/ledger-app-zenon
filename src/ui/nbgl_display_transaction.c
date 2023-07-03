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

#ifdef HAVE_NBGL

#pragma GCC diagnostic ignored "-Wformat-invalid-specifier"  // snprintf
#pragma GCC diagnostic ignored "-Wformat-extra-args"         // snprintf

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "glyphs.h"
#include "os_io_seproxyhal.h"
#include "nbgl_use_case.h"
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

// Buffer where the transaction hash string is written
static char g_hash[65];
// Buffer where the transaction amount string is written
static char g_amount[81];
// Buffer where the transaction address string is written
static char g_address[41];

static nbgl_layoutTagValue_t pairs[3];
static nbgl_layoutTagValueList_t pairList;
static nbgl_pageInfoLongPress_t infoLongPress;

static void confirm_transaction_rejection(void) {
    // display a status page and go back to main
    validate_transaction(false);
    nbgl_useCaseStatus("Transaction rejected", false, ui_menu_main);
}

static void ask_transaction_rejection_confirmation(void) {
    // display a choice to confirm/cancel rejection
    nbgl_useCaseConfirm("Reject transaction?",
                        NULL,
                        "Yes, Reject",
                        "Go back to transaction",
                        confirm_transaction_rejection);
}

// called when long press button on 3rd page is long-touched or when reject footer is touched
static void review_choice(bool confirm) {
    if (confirm) {
        // display a status page and go back to main
        validate_transaction(true);
        nbgl_useCaseStatus("TRANSACTION\nSIGNED", true, ui_menu_main);
    } else {
        ask_transaction_rejection_confirmation();
    }
}

static void review_send_continue(void) {
    // Setup data to display
    pairs[0].item = "Type";
    pairs[0].value = "Send";
    pairs[1].item = "Amount";
    pairs[1].value = g_amount;
    pairs[2].item = "Address";
    pairs[2].value = g_address;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 3;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_zenon_64px;
    infoLongPress.text = "Sign transaction\nto send";
    infoLongPress.longPressText = "Hold to sign";

    nbgl_useCaseStaticReview(&pairList, &infoLongPress, "Reject transaction", review_choice);
}

static void review_receive_continue(void) {
    // Setup data to display
    pairs[0].item = "Type";
    pairs[0].value = "Receive";
    pairs[1].item = "Hash";
    pairs[1].value = g_hash;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 2;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_zenon_64px;
    infoLongPress.text = "Sign transaction\nto receive";
    infoLongPress.longPressText = "Hold to sign";

    nbgl_useCaseStaticReview(&pairList, &infoLongPress, "Reject transaction", review_choice);
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
int ui_display_transaction() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (G_context.tx_info.transaction.blockType == 2 ||
        G_context.tx_info.transaction.blockType == 4) {
        // SEND TX
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
        char amount[82] = {0};
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

        // Start review
        nbgl_useCaseReviewStart(&C_app_zenon_64px,
                                "Review transaction\nto send",
                                NULL,
                                "Reject transaction",
                                review_send_continue,
                                ask_transaction_rejection_confirmation);
    } else if (G_context.tx_info.transaction.blockType == 1 ||
               G_context.tx_info.transaction.blockType == 3 ||
               G_context.tx_info.transaction.blockType == 5) {
        // RECEIVE TX
        memset(g_hash, 0, sizeof(g_hash));
        snprintf(g_hash, sizeof(g_hash), "0x%.*H", HASH_LEN, G_context.tx_info.m_hash);

        // Start review
        nbgl_useCaseReviewStart(&C_app_zenon_64px,
                                "Review transaction\nto receive",
                                NULL,
                                "Reject transaction",
                                review_receive_continue,
                                ask_transaction_rejection_confirmation);
    } else {
        // UNKNOWN TX
        return io_send_sw(SW_BAD_STATE);
    }

    return 0;
}

#endif
