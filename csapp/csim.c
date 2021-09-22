#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STANDARD_HELP "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n-h         Print this help message.\n-v         Optional verbose flag.\n-s <num>   Number of set index bits.\n-E <num>   Number of lines per set.\n-b <num>   Number of block offset bits.\n-t <file>  Trace file.\nExamples:\n    linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n"
#define BUFFER_SIZE 1024
#define OP_LENGTH    128

int hit_count = 0, miss_count = 0, eviction_count = 0;
int timestamp = 0;

typedef struct per_block
{
    bool isVaild;
    int tag;
    int timestamp;
} per_block;

typedef per_block* set;

typedef struct cache
{
    int nlines;
    int nsets;
    int bblocks;
    int bsets;
    set* head;
} cache;

typedef struct operation
{
    char op;
    int size;
    unsigned long address;
} operation;

static inline unsigned gets_s(unsigned long address, int s, int b)
{
    return (address >> b) % (2 << (s - 1));
}

static inline unsigned gets_b(unsigned long address, int s, int b)
{
    return address % (2 << (b - 1));
}

static inline unsigned gets_t(unsigned long address, int s, int b)
{
    return address >> (s + b);
}

cache allocate_set(int s, int b, int e)
{
    int nsets = 1 << s;
    set* head = calloc(nsets, sizeof(set));
    for (int i = 0; i < nsets; ++i)
        head[i] = calloc(e, sizeof(per_block));

    cache c = { e, nsets, b, s, head };
    return c;
}

void free_set(cache* c)
{
    for (int i = 0; i < c->nsets; ++i)
    {
        free(c->head[i]);
        c->head[i] = NULL;
    }
    free(c->head);
    c->head = NULL;
}

operation fetch_operation(FILE* pfile)
{
    char temp[OP_LENGTH];
    operation oper = { 0, 0, 0 };
    if(!fgets(temp, OP_LENGTH, pfile))
        return oper;
    sscanf(temp, "\n%c %lx,%d", &oper.op, &oper.address, &oper.size);
    return oper;
}

per_block* find_match_block(per_block* pb, int e, int tag)
{
    for (int i = 0; i < e; ++i)
        if (pb[i].tag == tag && pb[i].isVaild)
            return pb + i;
    return NULL;
}

per_block* find_evict_block(per_block* pb, int e)
{
    int index = 0;
    for (int i = 1; i < e; ++i)
        if (pb[i].timestamp < pb[index].timestamp)
            index = i;
    return pb + index;
}

per_block* find_new_block(per_block* pb, int e)
{
    for (int i = 0; i < e; ++i)
        if (!(pb[i].isVaild))
            return pb + i;

    return NULL;
}

void load_or_store(cache* c, operation* op, bool isDisplay, bool isMainEnvironment)
{
    int s = gets_s(op->address, c->bsets, c->bblocks);
    int t = gets_t(op->address, c->bsets, c->bblocks);

    if (isDisplay && isMainEnvironment)
        printf("%c %lx,%d ", op->op, op->address, op->size);

    per_block* appropriate_tag = find_match_block(c->head[s], c->nlines, t);
    if (appropriate_tag)
    {
        ++hit_count;
        if (isDisplay)
            printf("hit ");
    }
    else
    {
        ++miss_count;
        if (isDisplay)
            printf("miss ");

        appropriate_tag = find_new_block(c->head[s], c->nlines);
        if (appropriate_tag)
            appropriate_tag->isVaild = true;
        else
        {
            ++eviction_count;
            appropriate_tag = find_evict_block(c->head[s], c->nlines);
            if (isDisplay)
                printf("eviction ");
        }

        appropriate_tag->tag = t;
    }

    appropriate_tag->timestamp = timestamp;
    timestamp++;

    if (isDisplay && isMainEnvironment)
        printf("\n");
}

void modify(cache* sh, operation* op, bool isDisplay)
{
    if (isDisplay)
        printf("%c %lx,%d ", op->op, op->address, op->size);
    op->op = 'L';
    load_or_store(sh, op, isDisplay, false);
    op->op = 'S';
    load_or_store(sh, op, isDisplay, false);
    if (isDisplay)
        printf("\n");
}

void get_arguments(int argc, char** argvs, int* s, int* e, int* b, char** path, bool* isDisplay)
{
    for (int i = 1; i < argc;)
    {
        if (strcmp(argvs[i], "-s") == 0)
        {
            *s = atoi(argvs[i + 1]);
            i += 2;
        }
        else if (strcmp(argvs[i], "-E") == 0)
        {
            *e = atoi(argvs[i + 1]);
            i += 2;
        }
        else if (strcmp(argvs[i], "-b") == 0)
        {
            *b = atoi(argvs[i + 1]);
            i += 2;
        }
        else if (strcmp(argvs[i], "-t") == 0)
        {
            *path = argvs[i + 1];
            i += 2;
        }
        else if (strcmp(argvs[i], "-v") == 0)
        {
            *isDisplay = true;
            ++i;
        }
        else
        {
            printf("%s: invalid option -- %s\n", argvs[0], argvs[i]);
            printf(STANDARD_HELP, argvs[0], argvs[0], argvs[0]);
            exit(1);
        }
    }
}

int main(int argc, char** argvs)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argvs[i], "-h") == 0)
        {
            printf(STANDARD_HELP, argvs[0], argvs[0], argvs[0]);
            return 0;
        }
    }

    char* path = NULL;
    bool isDisplay = false;
    int s = 0, e = 0, b = 0;
    get_arguments(argc, argvs, &s, &e, &b, &path, &isDisplay);
    cache sh = allocate_set(s, b, e);
    FILE* pfile = fopen(path, "r");
    setvbuf(pfile, NULL, _IOFBF, BUFFER_SIZE);

    operation o;
    while ((o = fetch_operation(pfile)).op)
    {
        if (o.op == 'M')
            modify(&sh, &o, isDisplay);
        else if (o.op == 'L' || o.op == 'S')
            load_or_store(&sh, &o, isDisplay, true);
        else if (o.op == 'I')
            continue;
    }
    
    fclose(pfile);
    free_set(&sh);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}