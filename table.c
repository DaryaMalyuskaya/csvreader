#include "table.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int find_row(Table *t, int row_num) {
    for (int i = 0; i < t->num_rows; i++){
        if (t->row_nums[i] == row_num) {
            return i;
        }
    }
    return -1;
}

int add_row(Table *t, int row_num) {
    if (t->num_rows >= t->rows_cap) {
        t->rows_cap = t->rows_cap ? t->rows_cap * 2 : 16;
        t->row_nums = xrealloc(t->row_nums, (size_t)t->rows_cap * sizeof(int));
        t->cells    = xrealloc(t->cells, (size_t)t->rows_cap * t->num_cols * sizeof(Cell));
    }
    t->row_nums[t->num_rows] = row_num;
    for (int c = 0; c < t->num_cols; c++){
        CELL(t, t->num_rows, c) = (Cell){NULL, 0, UNEVALUATED};
    }
        
    return t->num_rows++;
}

void free_table(Table *t) {
    for (int c = 0; c < t->num_cols; c++){
        free(t->col_names[c]);
    }
        
    free(t->col_names);
    for (int r = 0; r < t->num_rows; r++){
        for (int c = 0; c < t->num_cols; c++){
            free(CELL(t, r, c).raw);
        }
    }
        
    free(t->cells);
    free(t->row_nums);
}

void print_table(Table *t) {
    printf(",");
    for (int c = 0; c < t->num_cols; c++){
        printf("%s%s", t->col_names[c], c < t->num_cols - 1 ? "," : "\n");
    }
        

    for (int r = 0; r < t->num_rows; r++) {
        printf("%d,", t->row_nums[r]);
        for (int c = 0; c < t->num_cols; c++) {
            if (CELL(t, r, c).state == EVALUATED){
                printf("%lld", CELL(t, r, c).val);
            }   
            else{
                printf("#ERROR");
            }
            printf("%s", c < t->num_cols - 1 ? "," : "\n");
        }
    }
}