#include <stdio.h>
#include "list.h"
int main() {

    List list = list_create(10);
    add_to_tail(&list, 5);
    add_to_tail(&list, 6);
    add_to_tail(&list, 7);
    add_to_tail(&list, 8);
    add_to_tail(&list, 9);
    add_to_tail(&list, 10);

    delete_element_by_id(&list, 2);
    // list_linearize(&list);
    attach_next_by_id(&list, 1, 2);
    print_full_information(&list);
    list_vis(&list);

    return 0;
}