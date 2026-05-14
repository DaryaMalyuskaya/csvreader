#pragma once
#include <stdio.h>
#include "table.h"

char *read_line(FILE *f);
int   split_csv(const char *line, char ***out);
void  free_fields(char **f, int n);
int   read_table(FILE *file, Table *t);