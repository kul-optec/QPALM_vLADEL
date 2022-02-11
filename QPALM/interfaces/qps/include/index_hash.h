#ifndef INDEX_HASH_H
#define INDEX_HASH_H

#include "global_opts.h"

struct node{
    char* key; /*name of the row/col*/
    c_int index; /*index of the row/col */
    char sign; /*constraint sign for a row*/
    struct node *next;
};

struct list
{
    struct node* first;
};

struct index_table{
    c_int size;
    struct node **list;
};

struct list* list_create(void);

void list_append(struct list* list, char* key);

void list_populate_indices(struct list* list, struct index_table* col_index_table);

void free_list(struct list* list);

void print_list(struct list* list);

c_int calculate_index_offset(struct list *list, c_int index);

struct index_table *create_index_table(c_int size);

c_int hashcode(struct index_table *t,char* key);

void insert(struct index_table *t, char* key, c_int index, char sign);

struct node* lookup(struct index_table *t,char* key);

void print_table(struct index_table *t, size_t size);

size_t length_table(struct index_table *t, size_t size);

void free_index_table(struct index_table *t, c_int size);


#endif