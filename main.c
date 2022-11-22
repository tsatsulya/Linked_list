#include <stdio.h>
#include "list.h"
int main() {

    List list = list_create(10);
    attach_to_data_tail(&list, 5);
    attach_to_data_tail(&list, 6);
    attach_to_data_tail(&list, 7);

    print_array_data(&list);

    print_list_data(&list);
    print_free_indexes(&list);

    swap_elements(&list, 5, 6);

    print_free_indexes(&list);

    print_array_data(&list);

    print_list_data(&list);
    



    // list_linearization(&list);

    return 0;
}