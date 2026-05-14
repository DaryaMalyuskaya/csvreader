#include "evaluate.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static int find_ref(Table *t, const char *s, int *ri, int *ci) {
    for (int c = 0; c < t->num_cols; c++) {
        size_t col_len = strlen(t->col_names[c]);

        if (strncmp(s, t->col_names[c], col_len) != 0) {
            continue;
        }

        char *e;
        long row_num = strtol(s + col_len, &e, 10);

        if (*e || row_num <= 0 || row_num > INT_MAX) {
            continue;
        }

        int r = find_row(t, (int)row_num);

        if (r < 0) {
            return 0;
        }

        *ri = r;
        *ci = c;
        return 1;
    }
    return 0;
}

static int valid_arg(Table *t, const char *s) {
    char *e;
    strtoll(s, &e, 10);

    if (*s && !*e) {
        return 1;
    }

    int r, c;
    return find_ref(t, s, &r, &c);
}

static long long get_val(Table *t, const char *s, int *ok) {
    char *e;
    long long v = strtoll(s, &e, 10);
    if (*s && !*e) {
        *ok = 1;
        return v;
    }
    int r, c;
    if (find_ref(t, s, &r, &c)) {
        eval(t, r, c);
        *ok = (CELL(t, r, c).state == EVALUATED);
        return CELL(t, r, c).val;
    }
    *ok = 0;
    return 0;
}

static int apply_op(char op, long long a, long long b, long long *res) {
    switch (op) {
    case '+':
        if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b))
            return 0;
        *res = a + b;
        return 1;
    case '-':
        if ((b < 0 && a > LLONG_MAX + b) || (b > 0 && a < LLONG_MIN + b))
            return 0;
        *res = a - b;
        return 1;
    case '*':
        if (a > 0 && b > 0 && a > LLONG_MAX / b) return 0;
        if (a > 0 && b < 0 && b < LLONG_MIN / a) return 0;
        if (a < 0 && b > 0 && a < LLONG_MIN / b) return 0;
        if (a < 0 && b < 0 && a < LLONG_MAX / b) return 0;
        *res = a * b;
        return 1;
    case '/':
        if (b == 0 || (a == LLONG_MIN && b == -1))
            return 0;
        *res = a / b;
        return 1;
    }
    return 0;
}

long long eval(Table *t, int r, int c) {
    Cell *cell = &CELL(t, r, c);
    if (cell->state == EVALUATED || cell->state == ERROR)
        return cell->val;
    if (cell->state == IN_PROGRESS) {
        cell->state = ERROR;
        return 0;
    }
    cell->state = IN_PROGRESS;

    if (cell->raw[0] != '=') {
        char *e;
        cell->val = strtoll(cell->raw, &e, 10);
        cell->state = (e == cell->raw || *e) ? ERROR : EVALUATED;
        return cell->val;
    }

    char *expr = cell->raw + 1;
    int len = strlen(expr), op = -1;
    char opch = 0;
    char *a1 = xmalloc(len + 1);
    char *a2 = xmalloc(len + 1);

    for (int i = 1; i < len - 1 && op < 0; i++) {
        char ch = expr[i];
        if (ch != '+' && ch != '-' && ch != '*' && ch != '/') continue;
        memcpy(a1, expr, i);
        a1[i] = 0;
        memcpy(a2, expr + i + 1, len - i - 1);
        a2[len - i - 1] = 0;
        trim(a1);
        trim(a2);
        if (valid_arg(t, a1) && valid_arg(t, a2)) {
            opch = ch;
            op = i;
        }
    }

    if (op < 0) {
        free(a1);
        free(a2);
        cell->state = ERROR;
        return 0;
    }

    int left_ok, right_ok;
    long long left  = get_val(t, a1, &left_ok);
    long long right = get_val(t, a2, &right_ok);
    free(a1);
    free(a2);

    if (!left_ok || !right_ok || cell->state == ERROR) {
        cell->state = ERROR;
        return 0;
    }
    if (!apply_op(opch, left, right, &cell->val)) {
        cell->state = ERROR;
        return 0;
    }
    cell->state = EVALUATED;
    return cell->val;
}