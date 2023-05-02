#include "nodemanager.h"


#define JOB_TAG 0
#define MAPPER_COMPLETE_TAG 1
#define SHUFFLE_TAG 2
#define REDUCER_INPUT_TAG 3




void split_input(const char *input_file, int num_mappers) {
    // Implement input splitting and sending to mappers

    



}

void receive_mapper_output() {
    // Implement receiving output from mappers
}

void shuffle_and_send_to_reducers() {
    // Implement shuffling of mapper output and sending to reducers
}

void process_reducer_output() {
    // Implement processing of reducer output
}