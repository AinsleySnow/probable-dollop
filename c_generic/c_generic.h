#ifndef _C_GENERIC_H_
#define _C_GENERIC_H_

#define CREATE_LIST(type)       \
typedef struct list_##type      \
{                               \
    type key;                   \
    struct list_##type* next;   \
} list_##type; 

#define list(x) list_##x

#define CREATE_FUNCTION_APPEND(type)                \
void append_##type(list(type)* head, type key)      \
{                                                   \
    list(type)* trailing = NULL;                    \
    list(type)* current = head;                     \
    while (current)                                 \
    {                                               \
        trailing = current;                         \
        current = current->next;                    \
    }                                               \
    current = calloc(1, sizeof(list(type)));        \
    current->key = key;                             \
    trailing->next = current;                       \
}

#define append(head, key, type) append_##type(head, key)

#define CREATE_FUNCTION_FREE(type)          \
void free_list_##type(list(type)* head)     \
{                                           \
    list(type)* current = head;             \
    list(type)* trailing = NULL;            \
    while (current)                         \
    {                                       \
        trailing = current;                 \
        current = current->next;            \
        free(trailing);                     \
    }                                       \
}                                                   

#define free_list(head, type) free_list_##type(head)

#endif // _C_GENERIC_H_