#ifndef NODEMANAGER_H
#define NODEMANAGE_H
#include "matrix_formatter.h"
#include <string.h>

matrix_input_list* parse_string_matrix(char** lines, int lines_count, int* rows_a, int* cols_a, int* rows_b, int* cols_b);
void generate_scatter_data(int* send_counts, int* dspls, int list_size, int num_mappers);
void get_keys_list(matrix_input_list* keys, int** pairs_arr, int total_pairs);
char** read_input(const char *input_file, int* lines_count);
void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols);


#endif