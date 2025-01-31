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

#ifdef HAVE_NBGL

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
#include "globals.h"
#include "sw.h"
#include "address.h"
#include "token_standard.h"
#include "bigint.h"
#include "validate.h"
#include "tx_types.h"
#include "menu.h"

// Buffer where the transaction hash string is written
static char g_hash[67];
// Buffer where the transaction amount string is written
static char g_amount[84];
// Buffer where the transaction address string is written
static char g_address[41];

static nbgl_contentTagValue_t pairs[2];
static nbgl_contentTagValueList_t pairList;

// called when long press button on 3rd page is long-touched or when reject footer is touched
static void review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_transaction(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_address buffers
// - Display the first screen of the transaction review
// - Display a warning if the transaction is blind-signed
int ui_display_transaction_bs_choice(bool is_blind_signed) {
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
        char zts_symbol[4];
        char zts_name[7];
        int8_t decimals = 8;
        if (zts == ZNN) {
            strcpy(zts_symbol, "ZNN");
            strcpy(zts_name, "Zenon");
        } else if (zts == QSR) {
            strcpy(zts_symbol, "QSR");
            strcpy(zts_name, "Quasar");
        } else {
            strcpy(zts_symbol, "ZTS");
            strcpy(zts_name, "token");
            decimals = 0;
        }

        memset(g_amount, 0, sizeof(g_amount));
        char amount[81] = {0};
        if (bigint_format(val, decimals, amount, sizeof(amount)) < 0) {
            return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
        }

        snprintf(g_amount, sizeof(g_amount), "%.*s %s", strlen(amount) - 1, amount, zts_symbol);
        PRINTF("Amount: %s\n", g_amount);

        memset(g_address, 0, sizeof(g_address));
        if (!address_encode(G_context.tx_info.transaction.toAddress,
                            g_address,
                            sizeof(g_address))) {
            return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
        }
        PRINTF("Address: %s\n", g_address);

		// Setup data to display
	    pairs[0].item = "Amount";
	    pairs[0].value = g_amount;
	    pairs[1].item = "Address";
	    pairs[1].value = g_address;

	    // Setup list
	    pairList.nbMaxLinesForValue = 0;
	    pairList.nbPairs = 2;
	    pairList.pairs = pairs;
	    
        char reviewTitle[35];
        char signTitle[33];
        snprintf(reviewTitle, sizeof(reviewTitle), "Review transaction\nto send %s\n", zts_name);
        snprintf(signTitle, sizeof(signTitle), "Sign transaction\nto send %s\n", zts_name);

		if (is_blind_signed) {
	        // Start blind-signing send flow
	        nbgl_useCaseReviewBlindSigning(TYPE_TRANSACTION,
	                                       &pairList,
	                                       &ICON_APP_ZENON,
	                                       reviewTitle,
	                                       NULL,
	                                       signTitle,
	                                       NULL,
	                                       review_choice);
	    } else {
	        // Start send flow
	        nbgl_useCaseReview(TYPE_TRANSACTION,
	                           &pairList,
	                           &ICON_APP_ZENON,
	                           reviewTitle,
	                           NULL,
	                           signTitle,
	                           review_choice);
	    }                   
    } else if (G_context.tx_info.transaction.blockType == 1 ||
               G_context.tx_info.transaction.blockType == 3 ||
               G_context.tx_info.transaction.blockType == 5) {
        // RECEIVE TX
        memset(g_hash, 0, sizeof(g_hash));
        if (format_hex(G_context.tx_info.m_hash, HASH_LEN, g_hash, sizeof(g_hash)) ==
            -1) {
            return io_send_sw(SW_DISPLAY_HASH_FAIL);
        };
        
        // Setup data to display
	    pairs[0].item = "Hash";
	    pairs[0].value = g_hash;

	    // Setup list
	    pairList.nbMaxLinesForValue = 0;
	    pairList.nbPairs = 1;
	    pairList.pairs = pairs;
    
        if (is_blind_signed) {
	        // Start blind-signing review flow
	        nbgl_useCaseReviewBlindSigning(TYPE_TRANSACTION,
	                                       &pairList,
	                                       &ICON_APP_ZENON,
	                                       "Review transaction\nto receive",
	                                       NULL,
	                                       "Sign transaction\nto receive",
	                                       NULL,
	                                       review_choice);
	    } else {
	        // Start review flow
	        nbgl_useCaseReview(TYPE_TRANSACTION,
	                           &pairList,
	                           &ICON_APP_ZENON,
	                           "Review transaction\nto receive",
	                           NULL,
	                           "Sign transaction\nto receive",
	                           review_choice);
	    }
    } else {
        // UNKNOWN TX
        return io_send_sw(SW_BAD_STATE);
    }
    return 0;
}

// Flow used to display a blind-signed transaction
int ui_display_blind_signed_transaction(void) {
    return ui_display_transaction_bs_choice(true);
}

// Flow used to display a clear-signed transaction
int ui_display_transaction() {
    return ui_display_transaction_bs_choice(false);
}

#endif
