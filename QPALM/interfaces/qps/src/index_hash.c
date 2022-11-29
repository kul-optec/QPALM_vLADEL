#include "index_hash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// create an empty list
struct list* list_create(void)
{
    struct list* list = malloc(sizeof(struct list));
    list->first = NULL;
    return list;
}

// appends the given key to the given list
void list_append(struct list* list, char* key)
{
    struct node* new_node = malloc(sizeof(struct node));
    new_node->key = (char*)malloc(sizeof(char)*(strlen(key)+1));
    strcpy(new_node->key, key);
    new_node->next = NULL;
    new_node->index = 0;
    
    // if the list is empty, make the new node the first node
    if (list->first == NULL) list->first = new_node;
    // else find the last node and set the new node as its next node
    else {
        struct node* node;
        node = list->first;
        while (node->next != NULL) node = node->next;
        node->next = new_node;
    }
}

void list_populate_indices(struct list* list, struct index_table* col_index_table) {
    struct node* node = list->first;
    struct node* col_node;
    while (node != NULL) {
        col_node = lookup(col_index_table, node->key);
        node->index = col_node->index-1;
        node = node->next;
    }
}

c_int calculate_index_offset(struct list *list, c_int index) {
    struct node* node = list->first;
    c_int index_offset = 0;
    while (node != NULL) {
        if (index > node->index) index_offset--;
        node = node->next;
    }
    return index_offset;
}

void free_list(struct list* list) {
    struct node* node = list->first;
    struct node* next_node;
    while (node != NULL) {
        next_node = node->next;
        qpalm_free(node->key);
        qpalm_free(node);
        node = next_node;
    }
    qpalm_free(list);
}

void print_list(struct list* list) {
    struct node* node = list->first;
    while (node != NULL) {
        printf("Key: %s, index: %" LADEL_PRIi "\n", node->key, node->index);
        node = node->next;
    }
}

struct index_table *create_index_table(c_int size){
    struct index_table *t = (struct index_table*)malloc(sizeof(struct index_table));
    t->size = size;
    t->list = (struct node**)malloc(sizeof(struct node*)*(size_t)size);
    c_int i;
    for(i=0; i<size; i++)
        t->list[i] = NULL;
    return t;
}

c_int hashcode(struct index_table *t,char* key){
    c_int code = 0;
    while(*key) code += (c_int)*key++;
    return code % t->size;
}

void insert(struct index_table *t, char* key, c_int index, char sign){
    c_int pos = hashcode(t,key);
    struct node *list = t->list[pos];
    struct node *new = (struct node*)malloc(sizeof(struct node));
    struct node *temp = list;
    while (temp) temp = temp->next;
    new->key = (char*)malloc(sizeof(char)*(strlen(key)+1));
    strcpy(new->key, key);
    new->index = index;
    new->sign = sign;
    new->next = list;
    t->list[pos] = new;
}

struct node* lookup(struct index_table *t,char* key){
    c_int pos = hashcode(t,key);
    struct node *list = t->list[pos];
    struct node *temp = list;
    while(temp){
        if (strcmp(key, temp->key)) temp=temp->next;
        else return temp;
    }
    return NULL;
}

size_t length_table(struct index_table *t, size_t size){
    struct node *temp;
    struct node *list;
    size_t pos, len = 0;
    for (pos = 0; pos < size; pos++) {
        list = t->list[pos];
        temp = list;
        while (temp) {
            len++;
            temp = temp->next;
        }
    }
    return len;
}

void print_table(struct index_table *t, size_t size){
    struct node *temp;
    struct node *list;
    size_t pos;
    for (pos = 0; pos < size; pos++) {
        list = t->list[pos];
        temp = list;
        while (temp) {
            printf("Entry: %s, %" LADEL_PRIi "\n", temp->key, temp->index);
            temp = temp->next;
        }
    }
}

void free_index_table(struct index_table *t, c_int size) {
    struct node *temp, *temp_next;
    struct node *list;
    c_int pos;
    for (pos = 0; pos < size; pos++) {
        list = t->list[pos];
        temp = list;
        while (temp) {
            temp_next = temp->next;
            qpalm_free(temp->key);
            qpalm_free(temp);
            temp = temp_next;
        }
    }
    qpalm_free(t->list);
    qpalm_free(t);
}