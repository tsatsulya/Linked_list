#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef int list_data_type; 

typedef enum {
    OK = 0,
    INVALID_ELEM_ID,
    RUN_OUT_OF_MEMORY,
    NOTHING_TO_DELETE,
} status_t;
typedef struct { //add type!!
    int prev; 
    int next; 
    list_data_type data;
} List_element;

typedef struct {
    int capacity;
    int size;
    List_element* buffer;
    int head_of_free_elements_index;
    bool is_sorted;
} List;

List list_create(int capacity); // TODO: status_t?
void list_destruct(List* list);

// TODO: add operations by index
status_t add_next_by_id(List* list, int id, list_data_type data); 
status_t add_prev_by_id(List* list, int id, list_data_type data);
status_t add_to_tail(List* list, list_data_type data); 
status_t delete_element_by_id(List* list, int id);
void list_linearize(List* list);

void list_vis(List* list); // TODO: visualize? ne smotry

void print_data_by_id(List* list, int id);
void print_list_data(List* list);
void print_array_data(List* list);
void print_full_information(List* list);


