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

static bool is_bad_ptr(void* ptr) {
    if ((long int)ptr < 0x0000ffff) return true;
    return false;
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


list_status_t list_create(List* list, int capacity) {

    capacity++;

    List_element* buffer = (List_element*)calloc(capacity, sizeof(List_element));
    ERROR_(is_bad_ptr(buffer), ALLOCATION_ERROR);
    
    List new_list = (List) {
        .capacity = capacity,
        .size = 0,
        .buffer = buffer,
        .head_of_free_elements_index = 1, // 0 - head of data units list, 1 - head of free data units list 
        .is_sorted = false
    };

    for (int i = 1; i < capacity; i++) {

        new_list.buffer[i].next = i + 1;
        new_list.buffer[i].prev = i - 1;

        if (i == 1) {
            new_list.buffer[i].prev = capacity - 1;
        }

        if (i == capacity - 1) {
            new_list.buffer[i].next = 1;
        }
        new_list.buffer[i].is_full = false;
        new_list.buffer[i].data = dead_value; 
    }


    new_list.buffer[0].prev = 0;
    new_list.buffer[0].next = 0;

    *list = new_list;

    return OK;
}


#define buffer list->buffer


static int cut_element_by_index(List* list, int ind_of_element) { 

    int ind_before_element = buffer[ind_of_element].prev;
    int ind_after_element = buffer[ind_of_element].next;

    buffer[ind_before_element].next = ind_after_element;
    buffer[ind_after_element].prev = ind_before_element;

    return ind_after_element;
}


static void insert_between_elements(List* list, int ind_of_element, int ind_of_left_element, 
                                        int ind_of_right_element, list_data_type data) {

    buffer[ind_of_left_element].next = ind_of_element;
    buffer[ind_of_element].prev = ind_of_left_element;

    buffer[ind_of_right_element].prev = ind_of_element;
    buffer[ind_of_element].next = ind_of_right_element;

    buffer[ind_of_element].data = data;

    if (buffer[ind_of_element].is_full) list->size++; 
}




static const int next = 1; 
static const int prev = -1;

static list_status_t add_element_by_index_in_direction(List* list, int ind_of_current_element, list_data_type data, int direction) {

    if (list->size == list->capacity) {
        ERROR_(list_resize(list), REALLOCATION_ERROR);
    }
    int new_element_index = list->head_of_free_elements_index; 
    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    buffer[ind_of_current_element].is_full = true;
    if (direction == next) {
        int ind_after_current_element = buffer[ind_of_current_element].next;     
        insert_between_elements(list, new_element_index, ind_of_current_element, ind_after_current_element, data);      
    }
    if (direction == prev) {
        int ind_before_current_element = buffer[ind_of_current_element].prev;                      
        insert_between_elements(list, new_element_index, ind_before_current_element, ind_of_current_element, data);      
    }

    list->is_sorted = false;

    return OK;
}


list_status_t add_after_by_id(List* list, int id, list_data_type data) {

    ERROR_(id > list->size, INVALID_ELEM_ID); 
    ERROR_(is_bad_ptr(list), BAD_PTR);
    
    return add_element_by_index_in_direction(list, get_index_by_id(list, id), data, next);
}

list_status_t add_before_by_id(List* list, int id, list_data_type data) {

    ERROR_(id > list->size, INVALID_ELEM_ID); 
    ERROR_(is_bad_ptr(list), BAD_PTR);

    return add_element_by_index_in_direction(list, get_index_by_id(list, id), data, prev);
}



list_status_t add_after_by_index(List* list, int index, list_data_type data) {
    
    ERROR_(is_bad_ptr(list), BAD_PTR);
    ERROR_(index >= list->capacity, INVALID_ELEM_INDEX);


    return add_element_by_index_in_direction(list, index, data, next);
}

list_status_t add_before_by_index(List* list, int index, list_data_type data) {
    
    ERROR_(is_bad_ptr(list), BAD_PTR);
    ERROR_(index >= list->capacity, INVALID_ELEM_INDEX);


    return add_element_by_index_in_direction(list, index, data, prev);
}


list_status_t list_resize(List* list);

list_status_t add_to_tail(List* list, list_data_type data) {
    
    ERROR_(is_bad_ptr(list), BAD_PTR);

    if (list->size == list->capacity) {
        list_resize(list);
    }

    int ind_of_data_tail = get_index_by_id(list, list->size);
    int new_element_index = list->head_of_free_elements_index;

    list->head_of_free_elements_index = cut_element_by_index(list, list->head_of_free_elements_index);

    buffer[new_element_index].is_full = true;
    insert_between_elements(list, new_element_index, ind_of_data_tail, ind_of_data_head, data); 

    list->is_sorted = false;

    return OK;
}  



list_status_t delete_element_by_index(List* list, int ind_of_element) {

    ERROR_(list->size == 0, NOTHING_TO_DELETE);
    ERROR_(is_bad_ptr(list), BAD_PTR);


    list->size--;

    int tail_of_free_elements = buffer[list->head_of_free_elements_index].prev;


    cut_element_by_index(list, ind_of_element); 

    buffer[ind_of_element].is_full = false;

    insert_between_elements(list, ind_of_element, tail_of_free_elements, list->head_of_free_elements_index, dead_value);

    list->head_of_free_elements_index = ind_of_element;
    list->is_sorted = false;

    return OK;
}

list_status_t delete_element_by_id(List* list, int id) {
    
    ERROR_(list->size < id, INVALID_ELEM_ID);
    ERROR_(is_bad_ptr(list), BAD_PTR);
    

    return delete_element_by_index(list, get_index_by_id(list, id));
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
    bool is_diff = (buffer[first_index].is_full) ^ (buffer[second_index].is_full);

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

    list->is_sorted = true;
}


#define max(x, y) ((x) > (y)) ? (x) : (y)
const int resize_multiplier = 2;
list_status_t resize(List* list) {

    ERROR_(is_bad_ptr(list), BAD_PTR);

    list->capacity *= resize_multiplier * list->capacity;
    buffer = (List_element*) realloc(buffer, list->capacity);

    ERROR_(is_bad_ptr(buffer), REALLOCATION_ERROR);

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


#define fputs(x) fputs(x, visualization)
void print_graph_parameters(FILE* visualization) {

    fputs("digraph structs {\n");
    fputs("\tsubgraph {\n");
    fputs("\t\tnodesep = 0.5;\n");
    fputs("\t\tnode [ shape = box, weight = 100 ];\n");
	fputs("\t\tedge [ style = invis, dir=both, arrowsize = 0.5 ];\n\n");
}

void list_visualize(List* list) {

    FILE* visualization = fopen("r.gv", "w");
    
    const char color_of_full_element[] = "\"#c1f3a6\"";
    const char color_of_empty_element[] = "\"#f2d5c7\"";

    print_graph_parameters(visualization);
    fprintf(visualization, "\t\tstruct%d [label=\"0|0\", style = filled, fillcolor = %s];\n",  0, "\"#f2f1c7\"");
    
    for (int i = 1; i < list->capacity; i++) {
        if (buffer[i].is_full) 
            fprintf(visualization, "\t\tstruct%d [label=\"%d|%d\", style = filled, fillcolor = %s];\n", 
                    i, i, buffer[i].data, buffer[i].is_full ? color_of_full_element : color_of_empty_element);
        else 
        fprintf(visualization, "\t\tstruct%d [label=\"%d| dead_value\", style = filled, fillcolor = %s];\n", 
                    i, i, buffer[i].is_full ? color_of_full_element : color_of_empty_element);    
    }

    int index = 0;

    fputs("\n\n");
    fprintf(visualization, "\t\t{rank = same; ");

    for (int i = 0; i < list->capacity; i++) {
        fprintf(visualization, "struct%d; ", i);
    }
    fputs("}\n");

    fputs("\t\t");

    for (int i = 0; i < list->capacity - 1; i++) {
        fprintf(visualization, "struct%d -> ", i);
    }
    fprintf(visualization, "struct10\n\n");
    fputs("\t}\n");

    

    for (int i = 0; i <= list->size; i++) {

        fprintf(visualization, "struct%d -> struct%d [dir=both, arrowsize = 0.5]\n", index, buffer[index].next);
        index = buffer[index].next;

    }

    index = list->head_of_free_elements_index;

    for (int i = list->size + 1; i < list->capacity; i++) {
        fprintf(visualization, "struct%d -> struct%d [dir=both, arrowsize = 0.5]\n", index, buffer[index].next);

        index = buffer[index].next;
    }
   
    fputs("}");


    fclose(visualization);
}

#undef fputs
#undef buffer


