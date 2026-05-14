#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        perror(NULL);
        exit(1);
    }
    return p;
}

void *xrealloc(void *p, size_t n) {
    p = realloc(p, n);
    if (!p) {
        perror(NULL);
        exit(1);
    }
    return p;
}

char *xdup(const char *s) {
    size_t n = strlen(s) + 1;
    return memcpy(xmalloc(n), s, n);
}

void trim(char *s) {
    int n = strlen(s);
    
    while (n > 0 && isspace((unsigned char)s[n-1])) {
        s[--n] = 0;
    }
        
    int i = 0;
    while (s[i] && isspace((unsigned char)s[i])) {
        i++;
    }

    if (i) {
        memmove(s, s+i, n-i+1);
    }
        
}