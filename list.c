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
        index = list->buffer[index].ind_of_next_element;
    }
    return index;
}

static const int dead_value = 0xdead;


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
        list.buffer[i].data = dead_value;
    }


    list.buffer[0].ind_of_prev_element = 0;
    list.buffer[0].ind_of_next_element = 0;

    return list;
}


#define buffer list->buffer


static int cut_element_by_index(List* list, int ind_of_element) {

    int ind_before_element = buffer[ind_of_element].ind_of_prev_element;
    int ind_after_element = buffer[ind_of_element].ind_of_next_element;

    buffer[ind_before_element].ind_of_next_element= ind_after_element;
    buffer[ind_after_element].ind_of_prev_element= ind_before_element;

    return ind_after_element;
}

typedef enum {
    free_data, 
    full_data
} data_t;


static void insert_between_elements(List* list, int ind_of_element, int ind_of_left_element, 
                                                    int ind_of_right_element, list_data_type data, data_t data_type) {

    buffer[ind_of_left_element].ind_of_next_element = ind_of_element;
    buffer[ind_of_element].ind_of_prev_element = ind_of_left_element;

    buffer[ind_of_right_element].ind_of_prev_element = ind_of_element;
    buffer[ind_of_element].ind_of_next_element = ind_of_right_element;

    buffer[ind_of_element].data = data;

    if (data_type == full_data) list->size++;
}


#define attach_verify() do {                                    \
    ERROR_(id > list->size, INVALID_ELEM_ID);                   \
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    \
} while (0)


#define next 1
#define prev -1

static status_t attach_element_by_id_in_direction(List* list, int id, list_data_type data, int direction) {

    attach_verify();

    int new_element_index = list->head_of_free_elements_index; 
    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    int ind_of_current_element = get_index_by_id(list, id);  

    if (direction == next) {
        int ind_after_current_element = buffer[ind_of_current_element].ind_of_next_element;     
        insert_between_elements(list, new_element_index, ind_of_current_element, ind_after_current_element, data, full_data);      
    }
    if (direction == prev) {
        int ind_before_current_element = buffer[ind_of_current_element].ind_of_prev_element;                      
        insert_between_elements(list, new_element_index, ind_before_current_element, ind_of_current_element, data, full_data);      
    }

    return OK;
}


status_t attach_next_by_id(List* list, int id, list_data_type data) {

    return attach_element_by_id_in_direction(list, id, data, next);
}

status_t attach_prev_by_id(List* list, int id, list_data_type data) {

    return attach_element_by_id_in_direction(list, id, data, prev);
}


status_t attach_to_data_tail(List* list, list_data_type data) {
    
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    

    int ind_of_data_tail = get_index_by_id(list, list->size);
    int new_element_index = list->head_of_free_elements_index;

    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    insert_between_elements(list, new_element_index, ind_of_data_tail, ind_of_data_head, data, full_data);    

    return OK;
}  



status_t delete_element_by_id(List* list, int id) {

    ERROR_(list->size == 0, NOTHING_TO_DELETE);
    ERROR_(list->size < id, INVALID_ELEM_ID); // in previous line????


    list->size-=1;

    int ind_of_element = get_index_by_id(list, id);
    int tail_of_free_elements = buffer[list->head_of_free_elements_index].ind_of_prev_element;


    cut_element_by_index(list, ind_of_element); 

    insert_between_elements(list, ind_of_element, tail_of_free_elements, list->head_of_free_elements_index, -1, free_data);

    list->head_of_free_elements_index = ind_of_element;

    return OK;
}


#define to ind_of_next_element
#define from ind_of_prev_element


void print_free_indexes(List* list) {
    int ind = list->head_of_free_elements_index;

    for (int i = 0; i < list->capacity - list->size; i++) {
        printf("free ind: %d\n", ind);
        ind = buffer[ind].to;
    }
}

static void swap_values(int* first_value, int* second_value) {
    int tmp = *first_value;
    *first_value = *second_value;
    *second_value = tmp;
}

static int my_abs(int value) {
    return (value > 0) ? value : -1 * value;
}
void swap_elements(List* list, int first_index, int second_index) {
    
    bool is_beside = (my_abs(first_index - second_index) == 1);


    List_element first_element = buffer[first_index];
    List_element second_element = buffer[second_index];

    buffer[first_element.to].from = second_index;
    buffer[first_element.from].to = second_index;

    printf("ind%d\n", second_index);
    printf("1: to %d from: %d\n", buffer[first_index].to, buffer[first_index].from);
    printf("2: to %d from: %d\n", buffer[second_index].to, buffer[second_index].from);

    printf("inddd %d\n", buffer[second_element.from].to);

    buffer[second_element.to].from = first_index;
    buffer[second_element.from].to = first_index;
    printf("inddd %d\n", buffer[second_element.from].to);

    
    if (my_abs(first_index - second_index) > 1) {
        swap_values(&(buffer[first_index].to), &(buffer[second_index].to));
        swap_values(&(buffer[first_index].from), &(buffer[second_index].from));
    }
    swap_values(&(buffer[first_index].data), &(buffer[second_index].data));

    printf("1: to %d from: %d\n", buffer[first_index].to, buffer[first_index].from);
    printf("2: to %d from: %d\n", buffer[second_index].to, buffer[second_index].from);
    
    if (first_index == list->head_of_free_elements_index)  list->head_of_free_elements_index = second_index;
    else if (second_index == list->head_of_free_elements_index) list->head_of_free_elements_index = first_index;
}


// void print_list_data(List* list);

void list_linearization(List* list) {

    print_array_data(list);
    print_free_indexes(list);


    int element_index = 0;

    for (int element_id = 1; element_id <= list->size; element_id++) {

        element_index = buffer[element_index].to;

        // printf("index: %d, id: %d\n", element_index, element_id);

        if (element_index != element_id) {

            swap_elements(list, element_id, element_index);

        }


    }
    print_array_data(list);
    print_free_indexes(list);

}
#undef to
#undef from


void print_data_by_id(List* list, int id) {

    int index = get_index_by_id(list, id);
    printf("[%d]: %d\n", index, buffer[index].data);
}

void print_list_data(List* list) {

    for (int id = 1; id <= list->size; id++) 
        print_data_by_id(list, id);
}

void print_array_data(List* list) {
    for (int i = 0; i < list->capacity; i++) {
        printf("%d ", buffer[i].data);
    }
    puts("\n");
}

#undef buffer