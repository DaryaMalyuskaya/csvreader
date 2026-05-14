#pragma once

typedef enum {
    UNEVALUATED,
    IN_PROGRESS,
    EVALUATED,
    ERROR
} State;

typedef struct {
    char     *raw;
    long long val;
    State     state;
} Cell;

typedef struct {
    int    num_cols, num_rows, rows_cap;
    char **col_names;
    int   *row_nums;
    Cell  *cells;
} Table;

#define CELL(t,r,c) (t)->cells[(r)*(t)->num_cols+(c)]

int  find_row(Table *t, int row_num);
int  add_row(Table *t, int row_num);
void free_table(Table *t);
void print_table(Table *t);