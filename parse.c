#include "parse.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char *read_line(FILE *f) {
    size_t cap = 64, len = 0;
    char *buf = xmalloc(cap);
    int c;

    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (c == '\r') continue;
        if (len + 1 >= cap) buf = xrealloc(buf, cap *= 2);
        buf[len++] = c;
    }

    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }

    buf[len] = 0;
    return buf;
}

int split_csv(const char *line, char ***out) {
    int n = 1;
    for (const char *p = line; *p; p++){
        if (*p == ',') n++;
    }
       
    *out = xmalloc(n * sizeof(char *));

    for (int i = 0; i < n; i++) {
        const char *end = strchr(line, ',');

        if (!end) {
            end = line + strlen(line);
        }

        size_t len = end - line;
        (*out)[i] = xmalloc(len + 1);
        memcpy((*out)[i], line, len);
        (*out)[i][len] = 0;
        trim((*out)[i]);
        line = end + (*end ? 1 : 0);
    }

    return n;
}

void free_fields(char **f, int n) {
    for (int i = 0; i < n; i++){
        free(f[i]);
    } 
    free(f);
}

int read_table(FILE *file, Table *t) {
    char *line = read_line(file);

    if (!line) {
        fprintf(stderr, "Пустой файл!\n");
        return 0;
    }

    char **header;
    int header_count = split_csv(line, &header);
    free(line);

    if (header_count < 2 || header[0][0] != 0) {
        fprintf(stderr, "Неверный заголовок, формат файла или пустой файл\n");
        free_fields(header, header_count);
        return 0;
    }

    t->num_cols = header_count - 1;
    t->col_names = xmalloc(t->num_cols * sizeof(char *));

    for (int i = 0; i < t->num_cols; i++){
        t->col_names[i] = xdup(header[i + 1]);
    }
    
    free_fields(header, header_count);

    while ((line = read_line(file))) {

        if (!*line) {
            free(line);
            continue;
        }

        char **fields;
        int count = split_csv(line, &fields);
        free(line);

        if (count != t->num_cols + 1) {
            fprintf(stderr, "Ошибка! Неверное количество полей!\n");
            free_fields(fields, count);
            return 0;
        }
        char *e;
        long row_num = strtol(fields[0], &e, 10);

        if (*e || row_num <= 0 || row_num > INT_MAX) {
            fprintf(stderr, "Ошибка! Неверный номер строки!\n");
            free_fields(fields, count);
            return 0;
        }

        if (find_row(t, (int)row_num) >= 0) {
            fprintf(stderr, "Ошибка! Номер строки уже существует\n");
            free_fields(fields, count);
            return 0;
        }

        int row = add_row(t, (int)row_num);

        for (int c = 0; c < t->num_cols; c++){
            CELL(t, row, c).raw = xdup(fields[c + 1]);
        }
        free_fields(fields, count);
    }
    return 1;
}