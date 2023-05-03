#include "nodemanager.h"
#include <stdio.h>
#include <string.h>

#define JOB_TAG 0
#define MAPPER_COMPLETE_TAG 1
#define SHUFFLE_TAG 2
#define REDUCER_INPUT_TAG 3




char** split_input(const char *input_file, int num_mappers) {
    // Implement input splitting and sending to mappers
    FILE* fileptr = fopen(input_file, "r");

    if (!fileptr) {
        printf("Error Opening File: %s\n", input_file);
        exit(1);
    }
    char buffer[1024];
    char* lines_buffer[1024];
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), fileptr)) {
        
        lines_buffer[line_count] = (char*) malloc(strlen(buffer) + 1);
        strcpy(lines_buffer[line_count], buffer);

        line_count++;
    }

    char** lines = (char**) malloc(line_count * sizeof(char*));
    for (int i = 0; i < line_count; i++) {
        lines[i] = lines_buffer[i];
    }

    fclose(fileptr);
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