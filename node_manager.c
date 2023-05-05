#include "node_manager.h"
#include "matrix_formatter.h"
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define JOB_TAG 0
#define MAPPER_COMPLETE_TAG 1
#define SHUFFLE_TAG 2
#define REDUCER_INPUT_TAG 3

void generate_scatter_data(int* send_counts, int* dspls, int list_size, int num_mappers, int curr_rank) {
    
    

    int block_size = list_size / num_mappers;
    int remaining = list_size - (block_size * num_mappers);



    int accumulated_displacement = 0;
    for (int i = 0; i < num_mappers; i++) {
        send_counts[i] = block_size;
        if (remaining > 0) {
            send_counts[i]++;
            remaining--;
        }
        dspls[i] = accumulated_displacement;
        accumulated_displacement += send_counts[i];
    }



}

matrix_input_list* parse_string_matrix(char** lines, int lines_count, int* rows_a, int* cols_a, int* rows_b, int* cols_b) {
    
    matrix_input_list* list =  init_matrix_input_list();

    bool is_processing_matrix_a = true;
    int matrix_b_start = 0;

    for (int i = 0; i < lines_count; i++, matrix_b_start++) {


        if (lines[i][0] == '*') {
            *rows_a = i;
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
        if (is_processing_matrix_a == true && lines[i + 1][0] == '*') {
            *cols_a = curr_j;
        }
        if (i == lines_count - 1 && is_processing_matrix_a == false){
            *cols_b = curr_j;
        }

    }

    *rows_b = lines_count - *rows_a;
    *rows_b = *rows_b - 1;


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