#ifndef MATRIX_FORMATTER_H
#define MATRIX_FORMATTER_H

#include <stdbool.h>

typedef struct {

    int num_keys;
    int* keys;

} matrix_format;


typedef struct node node;
struct node {

    matrix_format m;
    node* next;

};

typedef struct {

    node* head;
    int list_size;

} matrix_input_list;


matrix_input_list* init_matrix_input_list();

void insert_line(matrix_input_list* list, matrix_format m);

matrix_format get_at_index(matrix_input_list* list, int index);
int** convert_to_arr(matrix_input_list* list);
void unique_insert(matrix_input_list* list, matrix_format m);
bool is_val_exists(matrix_input_list* list, matrix_format m);
void destory_matrix_input_list(matrix_input_list* list);

#endif