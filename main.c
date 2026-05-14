#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "parse.h"
#include "evaluate.h"

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Использование: %s <file>.csv\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (!file) {
        perror(argv[1]);
        return 1;
    }

    Table t = {0};
    int ok = read_table(file, &t);
    fclose(file);

    if (!ok) {
        free_table(&t);
        return 1;
    }

    for (int r = 0; r < t.num_rows; r++){
        for (int c = 0; c < t.num_cols; c++){
            eval(&t, r, c);
        }       
    }
        
    print_table(&t);
    free_table(&t);
    return 0;
}