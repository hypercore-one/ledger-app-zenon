#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include <cmocka.h>

#include "bigint.h"

static void test_bigint_from_bytes(void **state) {
    (void) state;

    // clang-format off
    uint8_t in[] = { 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0xCB, 0xCC, 0xCC, 0x8C
    };
    
    // Setup
    bigint_t expected = { 
        .sign = 1,
        .bits = { 3419196556u, 1u },
        .bits_len = 2
    };
    
    // Execute
    bigint_t result;
    int status = bigint_from_bytes(in, 32, true, true, &result);
    
    // Validate
    assert_int_equal(status, 0);
    assert_int_equal(result.sign, expected.sign);
    assert_int_equal(result.bits_len, expected.bits_len);
    assert_memory_equal(result.bits, expected.bits, sizeof(uint32_t) * expected.bits_len);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bigint_from_bytes)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
