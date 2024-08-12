#ifndef _STONN_PUBLIC_H
#define _STONN_PUBLIC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

int stonn(const char* ptr, size_t len, int base, long long* out);
int stounn(const char* ptr, size_t len, int base, unsigned long long* out);
int stonf(const char* ptr, size_t len, float* out);
int stonlf(const char* ptr, size_t len, double* out);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
