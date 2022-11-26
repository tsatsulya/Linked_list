#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef int list_data_type; 

typedef enum {
    OK = 0,
    INVALID_ELEM_ID,
    RUN_OUT_OF_MEMORY,
    NOTHING_TO_DELETE,
    ALLOCATION_ERROR,
    BAD_PTR,
    INVALID_ELEM_INDEX,
    REALLOCATION_ERROR,
} list_status_t;
typedef struct { //add type!!
    int prev; 
    int next; 
    list_data_type data;
    bool is_full;
    // bool type; // true = full, false = empty
} List_element;

typedef struct {
    int capacity;
    int size;
    List_element* buffer;
    int head_of_free_elements_index;
    bool is_sorted;
} List;

list_status_t list_create(List* list, int capacity);
void list_destruct(List* list);

list_status_t add_after_by_id(List* list, int id, list_data_type data); 
list_status_t add_before_by_id(List* list, int id, list_data_type data);
list_status_t add_to_tail(List* list, list_data_type data); 

list_status_t add_after_by_index(List* list, int index, list_data_type data); 
list_status_t add_before_by_index(List* list, int index, list_data_type data);

list_status_t delete_element_by_index(List* list, int ind_of_element);
list_status_t delete_element_by_id(List* list, int id);

void list_linearize(List* list);
list_status_t list_resize(List* list);

void list_visualize(List* list); 

void print_data_by_id(List* list, int id);  //
void print_list_data(List* list);           //
void print_array_data(List* list);          //
void print_full_information(List* list);    //


