# Fuzzing on transaction parser

## Fuzzing

Fuzzing allows us to test how a program behaves when provided with invalid, unexpected, or random data as input.

Note: Usually we want to write a separate fuzz target for each functionality.

## Compilation

In `fuzzing` folder

```
cmake -DBOLOS_SDK=/path/to/sdk -DCMAKE_C_COMPILER=/usr/bin/clang -Bbuild -H.
```

then

```
make -C build
```

## Run

```
./build/fuzz_tx_parser
```
