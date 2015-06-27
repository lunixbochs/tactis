#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

char *strdup(const char *s) {
    char *s2 = malloc(strlen(s));
    strcpy(s2, s);
    return s2;
}

char *struprdup(char *s) {
    char *s2 = malloc(strlen(s));
    char *pos = s2;
    do *pos++ = toupper(*s++); while (*s);
    *pos = 0;
    return s2;
}
