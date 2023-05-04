#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "nodemanager.h"




int main(int argc, char **argv) {
    // Initialize MPI
    int world_rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int* send_counts = (int*) malloc((world_size - 1) * sizeof(int));
    int* dspls = (int*) malloc((world_size - 1) * sizeof(int));
    int input_arr_size = 0;
    int** input_arr = NULL;
    int rows = 0;

    MPI_Datatype int_array_type;
    MPI_Type_contiguous(4, MPI_INT, &int_array_type);
    MPI_Type_commit(&int_array_type);

    const char* filename = "input.txt"; // extract from the commandline params

    if (world_rank == 0) { // Nodemanager
        int lines_count = 0;
        char** lines = read_input(filename, &lines_count);
        matrix_input_list* mapper_inputs = parse_string_matrix(lines, lines_count);
        rows = (lines_count - 1) / 2;
        
        input_arr = convert_to_arr(mapper_inputs);
        input_arr_size = mapper_inputs->list_size;
        generate_scatter_data(send_counts, dspls, mapper_inputs->list_size, world_size - 1, world_rank);
        for (int i = 0; i < mapper_inputs->list_size; i++) {
 
            printf("matrix: %d, i: %d, j: %d, val: %d\n", input_arr[i][0], input_arr[i][1], input_arr[i][2], input_arr[i][3]);
        }

    }

    

    MPI_Barrier(MPI_COMM_WORLD);
    

    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(send_counts, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(dspls, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&input_arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);


    if (world_rank != 0) {
        input_arr = (int**) malloc(input_arr_size * sizeof(int*));

        for (int i = 0; i < input_arr_size; i++){
            input_arr[i] = (int*)malloc(4 * sizeof(int));
        }
    }

    for (int i = 0; i < input_arr_size; i++) {
        MPI_Bcast(input_arr[i], 4, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    int recv_count = 0;
    int displacement = 0;
    if (world_rank != 0) {
        recv_count = send_counts[world_rank - 1];
        displacement = dspls[world_rank - 1];


        for (int i = displacement; i < displacement + recv_count; i++) {
            
            int matrix_id = input_arr[i][0];
            int r = input_arr[i][1];
            int c = input_arr[i][2];
            int val = input_arr[i][3];
            
            if (matrix_id == 0) {
                int** outputs = (int**) malloc(rows * sizeof(int*));

                for (int j = 0; j < rows; j++){
                    outputs[j] = (int*) malloc(5 * sizeof(int));
                    outputs[j][0] = r;
                    outputs[j][1] = j;
                    outputs[j][2] = matrix_id;
                    outputs[j][3] = c;
                    outputs[j][4] = val;
                    MPI_Send(outputs[j], 5, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
            else {
                int** outputs = (int**) malloc(rows * sizeof(int*));

                for (int j = 0; j < rows; j++){
                    outputs[j] = (int*) malloc(5 * sizeof(int));
                    outputs[j][0] = j;
                    outputs[j][1] = c;
                    outputs[j][2] = matrix_id;
                    outputs[j][3] = r;
                    outputs[j][4] = val;
                    MPI_Send(outputs[j], 5, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
    }


    if (world_rank == 0) {
        matrix_input_list* pairs =  init_matrix_input_list();

        for (int i = 0; i < input_arr_size * rows; i++) {

            matrix_format p;
            p.num_keys = 5;
            p.keys = (int*) malloc(p.num_keys * sizeof(int));
            
            MPI_Recv(p.keys, p.num_keys, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, NULL);
            printf("(%d, %d): %d, %d, %d\n", p.keys[0], p.keys[1], p.keys[2], p.keys[3], p.keys[4]);
        }

    }


    
    
    //MPI_Scatterv(input_arr, send_counts, dspls, int_array_type, recv_buf, recv_count * 4, MPI_INT, 0, MPI_COMM_WORLD);
    // flatten, scatter, process, map, reduce, fap




    MPI_Finalize();

    return 0;
}