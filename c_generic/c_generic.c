#include <stdlib.h>
#include <stdio.h>
#include "c_generic.h"

CREATE_LIST(int)
CREATE_FUNCTION_APPEND(int)
CREATE_FUNCTION_FREE(int)
CREATE_LIST(double)
CREATE_FUNCTION_APPEND(double)
CREATE_FUNCTION_FREE(double)

int main(void)
{
    list(int)* head = calloc(1, sizeof(list(int)));
    head->key = 99;
    append(head, 8, int);
    append(head, 9, int);

    while (head)
    {
        printf("%d ", head->key);
        head = head->next;
    }
    printf("\n");

    list(double)* head_d = calloc(1, sizeof(list(double)));
    head_d->key = 0.23;
    append(head_d, 0.98, double);
    append(head_d, 6.789, double);

    while (head_d)
    {
        printf("%lf ", head_d->key);
        head_d = head_d->next;
    }
    printf("\n");

    free_list(head, int);
    free_list(head_d, double);
    return 0;
}