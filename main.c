#include <stdio.h>
#include "list.h"
int main() {

    List list = list_create(10);
    attach_to_data_tail(&list, 5);
    attach_to_data_tail(&list, 6);
    attach_to_data_tail(&list, 7);

    print_list_data(&list);

    attach_next_by_id(&list, 1, 10);

    print_list_data(&list);
    
    attach_prev_by_id(&list, 3, 20);

    print_list_data(&list);

    return 0;
}