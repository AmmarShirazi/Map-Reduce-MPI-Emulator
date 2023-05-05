#include "matrix_formatter.h"

#include <stdlib.h>


matrix_input_list* init_matrix_input_list() {

    matrix_input_list* list = (matrix_input_list*) malloc (sizeof(matrix_input_list));
    list->head = NULL;
    list->list_size = 0;

    return list;

}



void insert_line(matrix_input_list* list, matrix_format m) {

    if (list->head == NULL) {
        list->head = (node*) malloc(sizeof(node));
        list->head->m = m;
        list->head->next = NULL;
        list->list_size++;
        return;
    }

    node* curr_node = list->head;

    while(curr_node->next != NULL) {
        curr_node = curr_node->next;
    }

    curr_node->next = (node*) malloc(sizeof(node));
    curr_node->next->m = m;
    curr_node->next->next = NULL;
    list->list_size++;

}

int** convert_to_arr(matrix_input_list* list) {

    node* curr_node = list->head;
    int** m_arr = (int**) malloc(list->list_size * sizeof(int*));
    for (int i = 0; i < list->list_size; i++) {
        m_arr[i] = (int*) malloc(curr_node->m.num_keys * sizeof(int));

        for (int j = 0; j < curr_node->m.num_keys; j++){
            m_arr[i][j] = curr_node->m.keys[j];
        }
        curr_node = curr_node->next;
    }
    
    return m_arr;
}

matrix_format get_at_index(matrix_input_list* list, int index) {

    if (index >= list->list_size)  {
        exit(1);
    }
    node* curr_node = list->head;

    for (int i = 0; i < index; i++) {
        curr_node = curr_node->next;
    }


    return curr_node->m;
}

void destory_matrix_input_list(matrix_input_list* list) {

    node* curr_node = list->head;
    node* trail = curr_node;
    curr_node = curr_node->next;
    while (curr_node != NULL) {

        free(trail);
        trail = curr_node;
        curr_node = curr_node->next;
    }

}



