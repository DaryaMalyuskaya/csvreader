#pragma once
#include <stddef.h>

void *xmalloc(size_t n);
void *xrealloc(void *p, size_t n);
char *xdup(const char *s);
void  trim(char *s);