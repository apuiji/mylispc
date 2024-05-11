#ifndef ZLT_STRING_H
#define ZLT_STRING_H

#include<ctype.h>
#include"xyz.h"

/// @return 0-9 -1 when not a digit character
static inline int zltIsDigitChar(int c) {
  return isdigit(c) ? c - '0' : -1;
}

/// @param base 2-36 valid, else as 10
/// @return -1 when not a based digit character
int zltIsBasedDigitChar(int c, size_t base);

typedef struct {
  char *data;
  size_t size;
} zltString;

static inline zltString zltStrMake(const char *data, size_t size) {
  return (zltString) { .data = (char *) data, .size = size };
}

static inline zltString zltStrMakeBE(const char *begin, const char *end) {
  return zltStrMake(begin, end - begin);
}

/// @param data requires string constant literal
#define zltStrMakeStatic(data) zltStrMake(data, sizeof(data))

static inline zltString zltStrForward(zltString src, int n) {
  return zltStrMake(src.data + n, src.size - n);
}

zltString zltStrTrimStart(zltString str);
zltString zltStrTrimEnd(zltString str);

static inline zltString zltStrTrim(zltString str) {
  return zltStrTrimEnd(zltStrTrimStart(str));
}

/// different of stdlib strtol, match /^[+-]?[[:base-n-digit:]]+/, ignore bads like out of range
/// @param[out] dest usually initialized by 0
/// @return left contents
zltString zltStrToLong(long *dest, zltString src, size_t base);

/// different of stdlib strtoul,match /^[[:base-n-digit:]]+/, ignore bads like out of range
/// @param[out] dest usually initialized by 0
/// @return left contents
zltString zltStrToUnsignedLong(unsigned long *dest, zltString src, size_t base);

/// different of stdlib strtod, match /^[+-]?(\d+\.\d*|\.\d+)([Ee][+-]?\d+)?/, ignore bads like out of range
/// @param[out] dest usually initialized by 0
/// @return left contents
zltString zltStrToDouble(double *dest, zltString src);

/// different of stdlib strtod, match /^(\d+\.\d*|\.\d+)([Ee][+-]?\d+)?/, no signer character, ignore bads like out of range
/// @param[out] dest usually initialized by 0
/// @return left contents
zltString zltStrToUnsignedDouble(double *dest, zltString src);

#endif
