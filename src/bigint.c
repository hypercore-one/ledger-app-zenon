#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#include "bigint.h"

int bigint_from_bytes(const uint8_t *in,
                      size_t in_len,
                      bool isUnsigned,
                      bool isBigEndian,
                      bigint_t *out) {
    bool isNegative = false;
    uint8_t offset = 0;
    int8_t byteCount = in_len >= 32 ? 32 : in_len;

    memset(out->bits, 0, sizeof(out->bits));
    out->bits_len = 0;
    out->sign = 0;

    if (byteCount > 0) {
        uint8_t mostSignificantByte = isBigEndian ? in[0] : in[byteCount - 1];
        isNegative = (mostSignificantByte & 0x80) != 0 && !isUnsigned;

        if (mostSignificantByte == 0) {
            if (isBigEndian) {
                // Try to conserve space as much as possible by checking for wasted leading byte[]
                // entries
                offset = 1;

                while (offset < byteCount && in[offset] == 0) {
                    offset++;
                }

                byteCount = byteCount - offset;
            } else {
                byteCount -= 2;

                while (byteCount >= 0 && in[byteCount] == 0) {
                    byteCount--;
                }

                byteCount++;
            }
        }
    } else {
        isNegative = false;
    }

    if (byteCount == 0) {
        return 0;
    }

    if (byteCount <= 4) {
        uint32_t sign = isNegative ? 0xffffffff : 0;

        if (isBigEndian) {
            for (int8_t i = 0; i < byteCount; i++) {
                sign = (sign << 8) | in[offset + i];
            }
        } else {
            for (int8_t i = byteCount - 1; i >= 0; i--) {
                sign = (sign << 8) | in[offset + i];
            }
        }

        out->sign = (int32_t) sign;

        if (out->sign < 0 && !isNegative) {
            // Int32 overflow
            // Example: Int64 value 2362232011 (0xCB, 0xCC, 0xCC, 0x8C, 0x0)
            // can be naively packed into 4 bytes (due to the leading 0x0)
            // it overflows into the int32 sign bit
            out->bits[0] = (uint32_t) out->sign;
            out->bits_len = 1;
            out->sign = +1;
        }
        if (out->sign == INT_MIN) {
            out->bits[0] = (uint32_t) INT_MIN;
            out->bits_len = 1;
            out->sign = -1;
        }
    } else {
        uint8_t unalignedBytes = byteCount % 4;
        uint8_t dwordCount = byteCount / 4 + (unalignedBytes == 0 ? 0 : 1);
        uint8_t byteCountMinus1 = byteCount - 1;

        if (dwordCount > 32) {
            return -1;
        }

        // Copy all dwords, except don't do the last one if it's not a full four bytes
        int8_t curDword, curByte;

        if (isBigEndian) {
            curByte = byteCount - 4;
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++) {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++) {
                    uint8_t curByteValue = in[curByte + offset];
                    out->bits[curDword] = (out->bits[curDword] << 8) | curByteValue;
                    curByte++;
                }

                curByte -= 8;
            }
        } else {
            curByte = 4 - 1;
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++) {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++) {
                    uint8_t curByteValue = in[curByte + offset];
                    out->bits[curDword] = (out->bits[curDword] << 8) | curByteValue;
                    curByte--;
                }

                curByte += 8;
            }
        }

        // Copy the last dword specially if it's not aligned
        if (unalignedBytes != 0) {
            if (isNegative) {
                out->bits[dwordCount - 1] = 0xffffffff;
            }

            if (isBigEndian) {
                for (curByte = 0; curByte < unalignedBytes; curByte++) {
                    uint8_t curByteValue = in[curByte + offset];
                    out->bits[curDword] = (out->bits[curDword] << 8) | curByteValue;
                }
            } else {
                for (curByte = byteCountMinus1; curByte >= byteCount - unalignedBytes; curByte--) {
                    uint8_t curByteValue = in[curByte + offset];
                    out->bits[curDword] = (out->bits[curDword] << 8) | curByteValue;
                }
            }
        }

        if (isNegative) {
            // TODO: implement negative bigint.
            return -1;
        } else {
            out->bits_len = dwordCount;
            out->sign = +1;
        }
    }

    return 0;
}

int bigint_format(const bigint_t in, int8_t decimals, char *out, size_t out_len) {
    int32_t digits = 0;

    if (in.bits_len == 0) {
        if (decimals > 0) {
            char tmp[12];
            snprintf(tmp, sizeof(tmp), "%d", in.sign);
            number_format(tmp, decimals, out, out_len);
        } else {
            snprintf(out, out_len, "%d\n", in.sign);
        }
        return 0;
    }

    const uint32_t kuBase = 1000000000;  // 10^9
    const uint8_t kcchBase = 9;

    int8_t cuSrc = in.bits_len > 8 ? 8 : in.bits_len;
    uint32_t rguDst[10];
    memset(rguDst, 0, sizeof(rguDst));
    int8_t cuDst = 0;

    for (int8_t iuSrc = cuSrc; --iuSrc >= 0;) {
        uint32_t uCarry = in.bits[iuSrc];
        for (int8_t iuDst = 0; iuDst < cuDst; iuDst++) {
            uint64_t uuRes = make_uint64(rguDst[iuDst], uCarry);
            rguDst[iuDst] = (uint32_t)(uuRes % kuBase);
            uCarry = (uint32_t)(uuRes / kuBase);
        }
        if (uCarry != 0) {
            rguDst[cuDst++] = uCarry % kuBase;
            uCarry /= kuBase;
            if (uCarry != 0) rguDst[cuDst++] = uCarry;
        }
    }

    int32_t cchMax = cuDst * kcchBase;

    bool decimalFmt = decimals <= 0;
    if (decimalFmt) {
        if (digits > 0 && digits > cchMax) cchMax = digits;
        if (in.sign < 0) {
            cchMax = cchMax + 1;
        }
    }

    char rgch[83];
    memset(rgch, 0, sizeof(rgch));

    int32_t ichDst = (int32_t) out_len >= cchMax ? cchMax : out_len;

    for (int8_t iuDst = 0; iuDst < cuDst - 1; iuDst++) {
        uint32_t uDig = rguDst[iuDst];

        for (int8_t cch = kcchBase; --cch >= 0;) {
            rgch[--ichDst] = (char) ('0' + uDig % 10);
            uDig /= 10;
        }
    }
    for (uint32_t uDig = rguDst[cuDst - 1]; uDig != 0;) {
        rgch[--ichDst] = (char) ('0' + uDig % 10);
        uDig /= 10;
    }

    if (!decimalFmt) {
        char tmp[83] = {0};

        number_format(rgch + ichDst, decimals, tmp, sizeof(tmp));
        memcpy(rgch + ichDst, tmp, sizeof(tmp));
    }

    int numDigitsPrinted = cchMax - ichDst;
    while (digits > 0 && digits > numDigitsPrinted) {
        // pad leading zeros
        rgch[--ichDst] = '0';
        digits--;
    }
    if (in.sign < 0) {
        rgch[--ichDst] = '-';
    }

    int resultLength =
        ((decimals < cchMax - ichDst) ? cchMax - ichDst : decimals + 1) + ((decimals > 0) ? 1 : 0);

    memcpy(out, rgch + ichDst, resultLength);
    out[resultLength] = '\n';

    return resultLength;
}

uint64_t make_uint64(uint32_t hi, uint32_t lo) {
    return ((uint64_t) hi << 32) | lo;
}

void substring(char s[], char sub[], int p, int l) {
    int c = 0;

    while (c < l) {
        sub[c] = s[p + c - 1];
        c++;
    }
    sub[c] = '\0';
}

void number_format(char val[], int8_t decimals, char *out, size_t out_len) {
    int32_t len = strlen(val);
    int32_t trailing_zeros = 0;

    for (int32_t i = 0; i < decimals; i++) {
        if (len - i - 1 < 0 || val[len - i - 1] != '0') break;
        trailing_zeros++;
    }

    int32_t position = len - decimals;

    if (position > 0) {
        if (len - position - trailing_zeros > 0) {
            char sub[18] = {0};
            substring(val, sub, position + 1, len - position - trailing_zeros);

            snprintf(out, out_len, "%.*s.%s\n", position, val, sub);
        } else {
            snprintf(out, out_len, "%.*s\n", position, val);
        }
    } else if (position < 0) {
        if (len - trailing_zeros > 0) {
            // zero left-padding with width sub-specifier is not supported by BOLOS snprintf
            // function.
            snprintf(out,
                     out_len,
                     "0.%.*s%.*s\n",
                     abs(position),
                     "000000000000000000",
                     len - trailing_zeros,
                     val);
        } else {
            snprintf(out, out_len, "0\n");
        }
    } else {
        if (len - trailing_zeros > 0) {
            snprintf(out, out_len, "0.%.*s\n", len - trailing_zeros, val);
        } else {
            snprintf(out, out_len, "0\n");
        }
    }
}