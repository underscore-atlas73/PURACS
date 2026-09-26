#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
void* memrev(void*, size_t, size_t);

#define ITOA_UBIN_MAXLEN	35
#define ITOA_BIN_MAXLEN 	36
#define ITOA_UOCT_MAXLEN	14
#define ITOA_OCT_MAXLEN		15
#define ITOA_UDEC_MAXLEN	11
#define ITOA_DEC_MAXLEN		12
#define ITOA_UHEX_MAXLEN	11
#define ITOA_HEX_MAXLEN		12
char* itoa(int, char*, unsigned int, bool);

#define I64TOA_UBIN_MAXLEN	67
#define I64TOA_BIN_MAXLEN 	68
#define I64TOA_UOCT_MAXLEN	25
#define I64TOA_OCT_MAXLEN	26
#define I64TOA_UDEC_MAXLEN	21
#define I64TOA_DEC_MAXLEN	22
#define I64TOA_UHEX_MAXLEN	19
#define I64TOA_HEX_MAXLEN	20
char* i64toa(int64_t, char*, uint64_t, bool);

#ifdef __cplusplus
}
#endif

#endif