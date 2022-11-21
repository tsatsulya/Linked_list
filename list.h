#include <stddef.h>

typedef int list_element_type; 

typedef struct {
    int ind_of_prev_elem;
    int ind_of_next_elem;
    list_element_type element;
} List_element;

typedef struct {
    int capacity;
    int size;
    List_element* buffer;
    List_element* head_of_free_elements;
    bool is_sorted;
} List;

List list_create(int capacity);
void list_destruct(List* list);
status attach_next_by_id(List* list, int index, list_element_type element);

typedef enum {
    OK = 0,
    INVALID_ELEM_ID,
    RUN_OUT_OF_MEMORY,
} status_t;