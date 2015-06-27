#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *strdup(const char *s) {
    char *s2 = malloc(strlen(s));
    strcpy(s2, s);
    return s2;
}

char *strndup(const char *s, size_t len) {
    char *s2 = malloc(len + 1);
    strncpy(s2, s, len);
    s2[len] = '\0';
    return s2;
}

void strupr(char *s) {
    do {
        int c = toupper(*s);
        *s++ = c;
    } while (*s);
}
