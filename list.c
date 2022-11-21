#include "list.h"

#define ERROR_(COND, TO_RET)                                    \
    if (COND) {                                                 \
        return TO_RET;                                          \
    }

static int get_index_from_id(List* list, int id) {
    int index = 0;
    for (int i = 0; i < id; i++) {
        index = list->buffer[index].ind_of_next_elem;
    }
    return index;
}


List list_create(int capacity) {

    list_element_type* buffer = (list_element_type*)calloc(capacity, sizeof(List_element));
    
    List list = (List) {
        .capacity = capacity,
        .size = 0,
        .buffer = buffer,
        .head_of_free_elements = 1 // 0 - head of data units list, 1 - head of free data units list 
    };

    for (int i = 0; i < capacity-1; i++) {

        list.buffer[i].ind_of_next_elem = i + 1;
        list.buffer[i].ind_of_prev_elem = i - 1;

        if (i == 1) {
            list.buffer[i].ind_of_prev_elem = capacity - 1;
        }

        if (i == capacity - 1) {
            list.buffer[i].ind_of_next_elem = 1;
        }
    }
    return list;
}


void list_destruct(List* list) {

    free(list->buffer);
    
    *list = {
        .capacity = 0,
        .size = 0,
        .buffer = NULL,
        .head_of_free_elements = -1
    }; 
}

static int cut_element(List* list, int ind_of_element) { //return index of next element

    int ind_before_element = list->buffer[ind_of_element].ind_of_prev_elem;
    int ind_after_element = list->buffer[ind_of_element].ind_of_next_elem;

    //cut --> in function!!!
    list->buffer[ind_before_element].ind_of_next_elem = ind_after_element;
    list->buffer[ind_after_element].ind_of_prev_elem = ind_before_element;

    return ind_after_element;
}

static int insert_between_elements(List* list, int ind_of_element, int ind_of_left_element, int ind_of_right_element) {

    list->buffer[ind_of_left_element].ind_of_next_elem = ind_of_element;
    list->buffer[ind_of_element].ind_of_prev_elem = ind_of_left_element;

    list->buffer[ind_of_right_element].ind_of_prev_elem = ind_of_element;
    list->buffer[ind_of_element].ind_of_next_elem = ind_of_right_element;
}


status_t attach_next_by_id(List* list, int id, list_element_type element) {

    ERROR_(id > list->size, INVALID_ELEM_ID);
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);

    int ind_of_current_element = get_index_from_id(id);
    int ind_after_current_element = list->buffer[ind_of_current_element].ind_of_next_elem;
    insert_between_elements(list, list->head_of_free_elements, ind_of_current_element, ind_after_current_element);

    list->buffer[list->head_of_free_elements].element = element;
    list->head_of_free_elements = cut_element(list, list->head_of_free_elements);

}