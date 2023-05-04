#ifndef NODEMANAGER_H
#define NODEMANAGE_H
#include "MatrixFormatter.h"
#include <string.h>

matrix_input_list* parse_string_matrix(char** lines, int lines_count);
void generate_scatter_data(int* send_counts, int* dspls, int list_size, int num_mappers, int curr_rank);
char** read_input(const char *input_file, int* lines_count);
void receive_mapper_output();
void shuffle_and_send_to_reducers();
void process_reducer_output();


#endif