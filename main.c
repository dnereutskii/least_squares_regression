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

int parse_csv_line(char *s, int *arr, int len, char delim);
int count_char(const char *s, char ch);
void fltr_lf_cr(char *s);
bool calc_sigma(FILE *fl, struct sigma *sgm);

int main(int argc, char **argv)
{   
    struct sigma sgm = {0};
    struct line ln;
    
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("usage: %s filename\n", argv[0]);
        exit(0);
    }

    FILE *fl = fopen(argv[1], "r");
    if (!fl) {
        perror(argv[1]);
        exit(1);
    }

    // printf("%u %d %d %d %d\n", line_cnt, sgm.x, sgm.y, sgm.x2, sgm.xy);
    
    if (calc_sigma(fl, &sgm)) {
        double a = (line_cnt * sgm.xy - sgm.x * sgm.y);
        double b = (line_cnt * sgm.x2 - sgm.x * sgm.x);
        ln.slope =  a / b ;
        ln.intercept = (sgm.y - ln.slope * sgm.x) / line_cnt;
        printf("m = %.3f b = %.3f\n", ln.slope, ln.intercept);    
    }

    
    fclose(fl);

    return 0;
}

struct intbuf {
    int *p;
    size_t sz;
};

void intbuf_alloc(struct intbuf *ib, size_t sz)
{
    assert(ib);
    
    if (ib->p)
        free(ib->p);

    ib->p = malloc(sz * sizeof(int));
    if (!ib->p) {
        perror(NULL);
        exit(1);
    }
    ib->sz = sz;
}

void intbuf_free(struct intbuf *ib)
{
    assert(ib);

    if (ib->p) {
        free(ib->p);
        ib->sz = 0;
    }
}

bool calc_sigma(FILE *fl, struct sigma *sgm)
{
    char line[LINEMAX];
    unsigned line_cnt = 0;
    int cnt, vec[2], ret = true;
        
    assert(fl && sgm);

    intbuf_alloc(&vec, VEC_SZ);
    
    while (fgets(line, LINEMAX, fl)) {
        fltr_lf_cr(line);
                
        cnt = count_char(line, ',');
        if (cnt != sizeof(vec)) {
            frintf(stderr, "too few columns in %u row\n", line_cnt);
            ret = false;
            break;
        }

        cnt = parse_csv_line(line, &vec.p, vec.sz, ',');
        if (cnt < 0) {
            fprintf(stderr, "parsing end in %u row %d col\n", line_cnt, -cnt);
            ret = false;
            break;
        }
        
        sgm->x += vec[X];
        sgm->y += vec[Y];
        sgm->xy += vec[X] * vec[Y];
        sgm->x2 += vec[X] * vec[X];

        line_cnt++;
    }

    return ret;
}

void fltr_lf_cr(char *s)
{
    const char fltr[] = "\n\r";

    assert(s);
    
    for (const char *p = fltr; *p; p++) {
        char *ch = strrchr(s, *p);
        if (ch)
            *ch = '\0';
    }
}

int parse_csv_line(char *s, int *arr, int len, char delim)
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
