#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "node_manager.h"




int main(int argc, char **argv) {
    // Initialize MPI
    int world_rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        
    int rows_a = 0, cols_a = 0, rows_b = 0, cols_b = 0;
    MPI_Datatype int_array_type;
    MPI_Type_contiguous(4, MPI_INT, &int_array_type);
    MPI_Type_commit(&int_array_type);

    const char* filename = "input.txt"; // extract from the commandline params

    if (world_rank == 0) { // Nodemanager
        int lines_count = 0;
        char** lines = read_input(filename, &lines_count);

        matrix_input_list* mapper_inputs = parse_string_matrix(lines, lines_count, &rows_a, &cols_a, &rows_b, &cols_b);
        
        printf("rows_a = %d, cols_a = %d, rows_b = %d, cols_b = %d\n", rows_a, cols_a, rows_b, cols_b);
        int** input_arr = convert_to_arr(mapper_inputs);
        int* send_counts = (int*) malloc((world_size - 1) * sizeof(int));
        int* dspls = (int*) malloc((world_size - 1) * sizeof(int));
        generate_scatter_data(send_counts, dspls, mapper_inputs->list_size, world_size - 1, world_rank);
        for (int i = 0; i < mapper_inputs->list_size; i++) {
 
            printf("matrix: %d, i: %d, j: %d, val: %d\n", input_arr[i][0], input_arr[i][1], input_arr[i][2], input_arr[i][3]);
        }

        for (int i = 0; i < world_size - 1; i++) {
            MPI_Send(&send_counts[i], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);

            for (int j = dspls[i]; j < dspls[i] + send_counts[i]; j++) {
                MPI_Send(input_arr[j], 4, MPI_INT, i + 1, 0, MPI_COMM_WORLD);

            }
        }
        printf("\n");

    }



    MPI_Bcast(&rows_a, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols_b, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank != 0) { // Mapper Processes
        int recv_count = 0;

        MPI_Recv(&recv_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        
        int** recv_buf = (int**) malloc(recv_count * sizeof(int*));
        for (int i = 0; i < recv_count; i++){
            recv_buf[i] = (int*) malloc(4 * sizeof(int));
            MPI_Recv(recv_buf[i], 4, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        }

        for (int i = 0; i < recv_count; i++) {
            
            int matrix_id = recv_buf[i][0];
            int r = recv_buf[i][1];
            int c = recv_buf[i][2];
            int val = recv_buf[i][3];
            
            if (matrix_id == 0) {
                int** outputs = (int**) malloc(cols_b * sizeof(int*));

                for (int j = 0; j < cols_b; j++){
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
                int** outputs = (int**) malloc(rows_a * sizeof(int*));

                for (int j = 0; j < rows_a; j++){
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


    if (world_rank == 0) {  // Reducer Processors
        matrix_input_list* pairs =  init_matrix_input_list();
        for (int i = 0; i < (rows_a * cols_a * cols_b) + (rows_b * cols_b * rows_a); i++) {

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