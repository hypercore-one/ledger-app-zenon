# project information
project(TxParser
        VERSION 1.0
        DESCRIPTION "Transaction parser of Zenon app"
        LANGUAGES C)

# specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wno-unused-function -DFUZZ -pedantic -g -O0"
)

add_library(txparser
    ${BOLOS_SDK}/lib_standard_app/format.c
    ${BOLOS_SDK}/lib_standard_app/buffer.c
    ${BOLOS_SDK}/lib_standard_app/read.c
    ${BOLOS_SDK}/lib_standard_app/write.c
    ${BOLOS_SDK}/lib_standard_app/varint.c
    ${BOLOS_SDK}/lib_standard_app/bip32.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../src/transaction/deserialize.c
    #${CMAKE_CURRENT_SOURCE_DIR}/../src/address.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/segwit_addr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/token_standard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/bigint.c
)

set_target_properties(txparser PROPERTIES SOVERSION 1)

target_include_directories(txparser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${BOLOS_SDK}/lib_standard_app
    ${BOLOS_SDK}/include
    ${BOLOS_SDK}/lib_cxng/src
    ${BOLOS_SDK}/lib_cxng/include
    ${BOLOS_SDK}/target/nanos/include
)