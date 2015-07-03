#ifndef TACTIS_UTIL_H
#define TACTIS_UTIL_H

#include <stdint.h>
#include <stdlib.h>

char *strdup(const char *s);
char *strndup(const char *s, size_t len);
void strupr(char *s);
int16_t clamp_999(int16_t value);

#endif
