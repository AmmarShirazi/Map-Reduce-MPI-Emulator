#include "nodemanager.h"
#include "MatrixFormatter.h"
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define JOB_TAG 0
#define MAPPER_COMPLETE_TAG 1
#define SHUFFLE_TAG 2
#define REDUCER_INPUT_TAG 3

void generate_mapper_inputs(int* sendcounts, int* dspls, matrix_format* input_arr, matrix_input_list* mapper_inputs, int num_mappers, int curr_rank) {


    int block_size = mapper_inputs->list_size / num_mappers;
    int remaining = mapper_inputs->list_size - (block_size * num_mappers);

    
    


    int accumulated_displacement = 0;
    for (int i = 0; i < num_mappers; i++) {
        sendcounts[i] = block_size;
        if (remaining > 0) {
            sendcounts[i]++;
            remaining--;
        }
        dspls[i] = accumulated_displacement;
        accumulated_displacement += sendcounts[i];
    }



}

matrix_input_list* parse_string_matrix(char** lines, int lines_count) {
    
    matrix_input_list* list =  init_matrix_input_list();

    bool is_processing_matrix_a = true;
    int matrix_b_start = 0;

    for (int i = 0; i < lines_count; i++, matrix_b_start++) {


        if (lines[i][0] == '*') {
            is_processing_matrix_a = false;
            matrix_b_start = -1;
            continue;
        }
        

        char* token = strtok(lines[i], ",");

        int curr_j = 0;
        while (token != NULL) {
            
            matrix_format m;
            m.num_keys = 4;
            m.keys = (int*) malloc(m.num_keys * sizeof(int));
            m.keys[3] = atoi(token);
            m.keys[2] = curr_j;

            if (is_processing_matrix_a == true) {
                m.keys[0] = 0;
                m.keys[1] = i;
            }
            else {
                m.keys[0] = 1;
                m.keys[1] = matrix_b_start;
            }
            insert_line(list, m);
            curr_j++;
            token = strtok(NULL, ",");

        }

    }


    return list;

}

char** read_input(const char *input_file, int* lines_count) {
    // Implement input splitting and sending to mappers
    FILE* fileptr = fopen(input_file, "r");

    if (!fileptr) {
        printf("Error Opening File: %s\n", input_file);
        exit(1);
    }
    char buffer[1024];
    char* lines_buffer[1024];

    while (fscanf(fileptr, "%s\n", buffer) != EOF) {
        
        lines_buffer[*lines_count] = (char*) malloc(strlen(buffer) + 1);
        strcpy(lines_buffer[*lines_count], buffer);

        *lines_count = *lines_count + 1;
    }

    char** lines = (char**) malloc(*lines_count * sizeof(char*));
    for (int i = 0; i < *lines_count; i++) {
        lines[i] = lines_buffer[i];
    }

    fclose(fileptr);

    return lines;
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