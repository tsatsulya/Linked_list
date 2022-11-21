#include <stddef.h>
#include <stdbool.h>

typedef int list_data_type; 

typedef enum {
    OK = 0,
    INVALID_ELEM_ID,
    RUN_OUT_OF_MEMORY,
} status_t;
typedef struct {
    int ind_of_prev_element;
    int ind_of_next_element;
    list_data_type data;
} List_element;

typedef struct {
    int capacity;
    int size;
    List_element* buffer;
    int head_of_free_elements_index;
    bool is_sorted;
} List;

List list_create(int capacity);
void list_destruct(List* list);
status_t attach_next_by_id(List* list, int id, list_data_type data);
status_t attach_prev_by_id(List* list, int id, list_data_type data);
status_t attach_to_data_tail(List* list, list_data_type data);
status_t delete_element_by_id(List* list, int id);
void print_data_by_id(List* list, int id);
void print_list_data(List* list);



