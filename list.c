#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

static const int ind_of_data_head = 0;
// TODO: isn't this useful anywhere else, extract? 
#define ERROR_(COND, TO_RET)                                    \
    if (COND) {                                                 \
        return TO_RET;                                          \
    }


static int get_index_by_id(List* list, int id) {

    if (list->is_sorted) return id;

    int index = 0;

    for (int i = 0; i < id; i++) {
        index = list->buffer[index].next;
    }
    return index;
}

static const int dead_value = 0xdead;


List list_create(int capacity) {

    capacity++;

    List_element* buffer = (List_element*)calloc(capacity, sizeof(List_element));
    // TODO: status?
    
    List list = (List) {
        .capacity = capacity,
        .size = 0,
        .buffer = buffer,
        .head_of_free_elements_index = 1, // 0 - head of data units list, 1 - head of free data units list 
        .is_sorted = false
    };

    for (int i = 1; i < capacity; i++) {

        list.buffer[i].next = i + 1;
        list.buffer[i].prev = i - 1;

        if (i == 1) {
            list.buffer[i].prev = capacity - 1;
        }

        if (i == capacity - 1) {
            list.buffer[i].next = 1;
        }
        list.buffer[i].data = dead_value; // TODO: ?
    }


    list.buffer[0].prev = 0;
    list.buffer[0].next = 0;

    return list;
}


#define buffer list->buffer


static int cut_element_by_index(List* list, int ind_of_element) { 

    // TODO: char (*buffer)[] = list->buffer; Maybe you can?
    // get_el_by_ind(list, ind_of_element)->

    int ind_before_element = buffer[ind_of_element].prev;
    int ind_after_element = buffer[ind_of_element].next;

    buffer[ind_before_element].next = ind_after_element;
    buffer[ind_after_element].prev = ind_before_element;

    return ind_after_element;
}

typedef enum {
    free_data, 
    full_data
} data_t; // TODO: sushinostey naplodilos'


static void insert_between_elements(List* list, int ind_of_element, int ind_of_left_element, 
                                        int ind_of_right_element, list_data_type data, data_t data_type) {

    buffer[ind_of_left_element].next = ind_of_element;
    buffer[ind_of_element].prev = ind_of_left_element;

    buffer[ind_of_right_element].prev = ind_of_element;
    buffer[ind_of_element].next = ind_of_right_element;

    buffer[ind_of_element].data = data;

    if (data_type == full_data) list->size++; // TODO: should this change size?
}

#define attach_verify() do {                                    \
    ERROR_(id > list->size, INVALID_ELEM_ID);                   \
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    \
} while (0)


static const int next = 1; 
static const int prev = -1 

static status_t add_element_by_id_in_direction(List* list, int id, list_data_type data, int direction) {

    attach_verify();

    int new_element_index = list->head_of_free_elements_index; 
    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    int ind_of_current_element = get_index_by_id(list, id);  

    if (direction == next) {
        int ind_after_current_element = buffer[ind_of_current_element].next;     
        insert_between_elements(list, new_element_index, ind_of_current_element, ind_after_current_element, data, full_data);      
    }
    if (direction == prev) {
        int ind_before_current_element = buffer[ind_of_current_element].prev;                      
        insert_between_elements(list, new_element_index, ind_before_current_element, ind_of_current_element, data, full_data);      
    }

    return OK;
}


status_t add_next_by_id(List* list, int id, list_data_type data) {

    return add_element_by_id_in_direction(list, id, data, next);
}

status_t add_prev_by_id(List* list, int id, list_data_type data) {

    return add_element_by_id_in_direction(list, id, data, prev);
}


status_t add_to_tail(List* list, list_data_type data) {
    
    ERROR_(list->size == list->capacity, RUN_OUT_OF_MEMORY);    

    int ind_of_data_tail = get_index_by_id(list, list->size);
    int new_element_index = list->head_of_free_elements_index;

    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    insert_between_elements(list, new_element_index, ind_of_data_tail, ind_of_data_head, data, full_data);    

    return OK;
}  



status_t delete_element_by_id(List* list, int id) {

    ERROR_(list->size == 0, NOTHING_TO_DELETE);
    ERROR_(list->size < id, INVALID_ELEM_ID); 


    list->size--;

    int ind_of_element = get_index_by_id(list, id);
    int tail_of_free_elements = buffer[list->head_of_free_elements_index].prev;


    cut_element_by_index(list, ind_of_element); 

    insert_between_elements(list, ind_of_element, tail_of_free_elements, list->head_of_free_elements_index, dead_value, free_data);

    list->head_of_free_elements_index = ind_of_element;

    return OK;
}



static void swap_values(int* first_value, int* second_value) {
    int tmp = *first_value;
    *first_value = *second_value;
    *second_value = tmp;
}

static int my_abs(int value) {
    return (value > 0) ? value : -1 * value;
}

static void swap_elements(List* list, int first_index, int second_index) {
    
    bool is_beside = (my_abs(first_index - second_index) == 1);
    bool is_diff = (buffer[first_index].data == dead_value) ^ (buffer[second_index].data == dead_value);

    List_element first_element = buffer[first_index];
    List_element second_element = buffer[second_index];

    if (is_beside && !is_diff) { // TODO: podumoi esli smozhesh :)

        buffer[first_element.prev].next = second_index;
        buffer[second_element.next].prev = first_index;

        buffer[second_index].next = second_element.prev;
        buffer[second_index].prev = first_element.prev;
        buffer[first_index].prev = first_element.next;
        buffer[first_index].next = second_element.next;

    }   else {
        
        buffer[first_element.next].prev = second_index;
        buffer[first_element.prev].next = second_index;

        buffer[second_element.next].prev = first_index;
        buffer[second_element.prev].next = first_index;

        
        swap_values(&(buffer[first_index].next), &(buffer[second_index].next));
        swap_values(&(buffer[first_index].prev), &(buffer[second_index].prev));
    }

    swap_values(&(buffer[first_index].data), &(buffer[second_index].data));
    
    if (first_index == list->head_of_free_elements_index)
        list->head_of_free_elements_index = second_index;
    else if (second_index == list->head_of_free_elements_index) 
        list->head_of_free_elements_index = first_index;
}



void list_linearize(List* list) {

    int element_index = 0;

    for (int element_id = 1; element_id <= list->size; element_id++) {

        element_index = buffer[element_id-1].next;

        if (element_index != element_id) {
            swap_elements(list, element_id, element_index);
        }
    }

    // list->is_sorted = true; // TODO: ??? (make this useful, pleeeasseeeeeee)
}



void print_data_by_id(List* list, int id) {

    int index = get_index_by_id(list, id);
    printf("[%d]: %d\n", index, buffer[index].data);
}

void print_list_data(List* list) {

    for (int id = 1; id <= list->size; id++) 
        print_data_by_id(list, id);

}

void print_array_data(List* list) {
    puts("\n");

    for (int i = 0; i < list->capacity; i++) {
        printf("%d ", buffer[i].data);
    }
    puts("\n");
}

void print_full_information(List* list) {
    

    print_array_data(list);
    print_list_data(list);

    puts("\n");

    for (int i = 0; i < list->capacity; i++) {
        printf("[%d]  to: %d  from: %d data: %d\n", i, buffer[i].next, buffer[i].prev, buffer[i].data);
    }

    puts("----------------------------------------------------------");
}


void list_vis(List* list) {

    FILE* visualization = fopen("r.gv", "w");
    fputs("digraph structs {\n", visualization);
    fputs("\tnode [ shape=Mrecord ]\n", visualization);

    fputs("\t\trankdir=LR\n", visualization);
    fputs("\t\tsplines = true;\n", visualization);

    fprintf(visualization, "\tstruct%d [label=\"{%d|<here> %d}\"];\n", 0, 0, 0);

    
    
    for (int i = 0; i < list->capacity; i++) {
        fprintf(visualization, "\tstruct%d [label=\"{%d|<here> %d}\"];\n", i, i, buffer[i].data);
    }


    int index = 0;
    for (int i = 0; i <= list->size; i++) {

        
        fprintf(visualization, "struct%d -> struct%d [arrowsize=0.5]\n", index, buffer[index].next);
        fprintf(visualization, "struct%d -> struct%d [arrowsize=0.5]\n", buffer[index].next, index);
        index = buffer[index].next;

    }

    index = list->head_of_free_elements_index;

    for (int i = list->size + 1; i < list->capacity; i++) {
        fprintf(visualization, "struct%d -> struct%d [arrowsize=0.5]\n", index, buffer[index].next);
        fprintf(visualization, "struct%d -> struct%d [arrowsize=0.5]\n", buffer[index].next, index);

        index = buffer[index].next;
    }
   
    fputs("}", visualization);
    fclose(visualization);
}
#undef buffer
