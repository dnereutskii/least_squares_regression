#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>

#define SIZEOFARR(ARR) (sizeof(ARR)/sizeof(*ARR))

#define ARR_SZ  (5)
#define LINEMAX (200)

/* least squares regresssion struct */
struct sigma { 
    int x;
    int y;
    int xy;
    int x2;
};

struct line {
    double slope;
    double intercept;
};

enum {
    X = 0,
    Y
};

int parse_csv_line(char *s, int *arr, int len);
int count_char(const char *s, char ch);

int main(int argc, char **argv)
{
    char line[LINEMAX];
    size_t line_len;
    int *vec, vec_sz, cnt;
    unsigned line_cnt = 0;
    struct sigma sgm = {0};
    struct line ln;
    
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("usage: %s filename\n", argv[0]);
        exit(0);
    }
    
    vec_sz = ARR_SZ;
    vec = malloc(ARR_SZ * sizeof(int));
    if (!vec) {
        perror(NULL);
        exit(1);
    }

    FILE *fl = fopen(argv[1], "r");
    if (!fl) {
        perror(argv[1]);
        exit(1);
    }
    
    while (fgets(line, LINEMAX, fl) != NULL) {        
        line_cnt++;
        line_len = strlen(line);
        if (line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
            line_len--;
        }
        
        cnt = count_char(line, ',');
        if (cnt > vec_sz) {
            free(vec);
            vec = malloc(cnt * sizeof(int));
            if (!vec) {
                perror(NULL);
                exit(1);
            }
        }

        cnt = parse_csv_line(line, vec, vec_sz);
        if (cnt < 0) {
            fprintf(stderr, "parsing end in %u row %d col\n",
                    line_cnt - 1, -cnt);
            exit(1);
        }
        sgm.x += vec[X];
        sgm.y += vec[Y];
        sgm.xy += vec[X] * vec[Y];
        sgm.x2 += vec[X] * vec[X];
        
    }

    // printf("%u %d %d %d %d\n", line_cnt, sgm.x, sgm.y, sgm.x2, sgm.xy);

    double a = (line_cnt * sgm.xy - sgm.x * sgm.y);
    double b = (line_cnt * sgm.x2 - sgm.x * sgm.x);
    ln.slope =  a / b ;
    ln.intercept = (sgm.y - ln.slope * sgm.x) / line_cnt;

    printf("m = %.3f b = %.3f\n", ln.slope, ln.intercept);
    
    /* deallocation */
    fclose(fl);
    free(vec);

    return 0;
}

int parse_csv_line(char *s, int *arr, int len)
{
    char *tok, *p, *end;
    int ii = 0;
    long num;

    for (p = s; ii < len && (tok = strtok(p, ",")); p = NULL) {
        num = (int)strtol(tok, &end, 10);
        if (*end != '\0') {
            ii = -ii;
            break;
        }
        arr[ii] = num;
        ii++;
    }

    return ii;
}

int count_char(const char *s, char ch)
{
    int cnt = 0;

    for (const char *p = s; (p = strchr(p, ch)) != NULL; p += 1) {
        cnt++;
        // printf("%s\n", p);
    }

    return cnt;
}
