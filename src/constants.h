#pragma once

/**
 * Instruction class of the Zenon application.
 */
#define CLA 0xE0

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Transaction length (bytes).
 */
#define TRANSACTION_LEN 306

/**
 * Signature length (bytes).
 */
#define SIG_LEN 64

/**
 * Public key length (bytes).
 */
#define PUBKEY_LEN 32

/**
 * Exponent used to convert mZTS to ZTS unit (N ZTS = N * 10^3 mZTS).
 */
#define EXPONENT_SMALLEST_UNIT 3
