#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#define ind_of_data_head 0

#define ERROR_(COND, TO_RET)                                    \
    if (COND) {                                                 \
        return TO_RET;                                          \
    }


static int get_index_by_id(List* list, int id) {
    int index = 0;
    for (int i = 0; i < id; i++) {
        // printf("next ind: %d\n", list->buffer[0].ind_of_next_element);
        index = list->buffer[index].ind_of_next_element;
    }
    return index;
}


List list_create(int capacity) {

    List_element* buffer = (List_element*)calloc(capacity, sizeof(List_element));
    
    List list = (List) {
        .capacity = capacity,
        .size = 0,
        .buffer = buffer,
        .head_of_free_elements_index = 1 // 0 - head of data units list, 1 - head of free data units list 
    };

    for (int i = 1; i < capacity; i++) {

        list.buffer[i].ind_of_next_element= i + 1;
        list.buffer[i].ind_of_prev_element= i - 1;

        if (i == 1) {
            list.buffer[i].ind_of_prev_element= capacity - 1;
        }

        if (i == capacity - 1) {
            list.buffer[i].ind_of_next_element= 1;
        }
    }


    list.buffer[0].ind_of_prev_element = 0;
    list.buffer[0].ind_of_next_element = 0;

    return list;
}

static int cut_element_by_index(List* list, int ind_of_element) {

    int ind_before_element = list->buffer[ind_of_element].ind_of_prev_element;
    int ind_after_element = list->buffer[ind_of_element].ind_of_next_element;


    list->buffer[ind_before_element].ind_of_next_element= ind_after_element;
    list->buffer[ind_after_element].ind_of_prev_element= ind_before_element;

    return ind_after_element;
}

static void insert_between_elements(List* list, int ind_of_element, int ind_of_left_element, int ind_of_right_element, list_data_type data){

    list->buffer[ind_of_left_element].ind_of_next_element = ind_of_element;
    list->buffer[ind_of_element].ind_of_prev_element = ind_of_left_element;

    list->buffer[ind_of_right_element].ind_of_prev_element = ind_of_element;
    list->buffer[ind_of_element].ind_of_next_element = ind_of_right_element;

    list->buffer[ind_of_element].data = data;

    list->size++;

}


#define attach_verify() do {                                    \
    ERROR_(id > list->size, INVALID_ELEM_ID);                   \
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    \
} while (0)


status_t attach_next_by_id(List* list, int id, list_data_type data) {

    attach_verify();
    int new_element_index = list->head_of_free_elements_index; 
    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    int ind_of_current_element = get_index_by_id(list, id);                                                                   //function?????????????????????
    int ind_after_current_element = list->buffer[ind_of_current_element].ind_of_next_element;     
    insert_between_elements(list, new_element_index, ind_of_current_element, ind_after_current_element, data);      //

    return OK;

}

status_t attach_prev_by_id(List* list, int id, list_data_type data) {

    attach_verify();

    int new_element_index = list->head_of_free_elements_index; 
    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    int ind_of_current_element = get_index_by_id(list, id);                                                                   //function?????????????????????
    int ind_before_current_element = list->buffer[ind_of_current_element].ind_of_prev_element;                              //
    insert_between_elements(list, new_element_index, ind_before_current_element, ind_of_current_element, data);      //

    return OK;

}


status_t attach_to_data_tail(List* list, list_data_type data) {
    
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    

    int ind_of_data_tail = get_index_by_id(list, list->size);
    int new_element_index = list->head_of_free_elements_index;

    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    insert_between_elements(list, new_element_index, ind_of_data_tail, ind_of_data_head, data);    

    return OK;
}  


status_t delete_element_by_id(List* list, int id) {
    return OK;
}

void print_data_by_id(List* list, int id) {

    int index = get_index_by_id(list, id);

    printf("%d\n", list->buffer[index].data);
}

void print_list_data(List* list) {

    for (int id = 1; id <= list->size; id++) 
        print_data_by_id(list, id);
}