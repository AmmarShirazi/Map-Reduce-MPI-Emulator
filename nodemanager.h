#ifndef NODEMANAGER_H
#define NODEMANAGE_H
#include <string.h>

char** split_input(const char *input_file, int num_mappers);
void receive_mapper_output();
void shuffle_and_send_to_reducers();
void process_reducer_output();


#endif