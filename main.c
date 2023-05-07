#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "node_manager.h"
#include "matrix_formatter.h"



int main(int argc, char **argv) {
    // Initialize MPI
    int world_rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  
    int rows_a = 0, cols_a = 0, rows_b = 0, cols_b = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: mpirun -np <num_processes> %s <input_file> <output_file>\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }

    // Extract the filename from the commandline params
    const char* filename = argv[1];
    const char* output_file = argv[2];


    if (world_rank == 0) { // Nodemanager
        printf("Master with process_id %d, running on %s\n", world_rank, "lol");
        int lines_count = 0;
        
        char** lines = read_input(filename, &lines_count);
        matrix_input_list* mapper_inputs = parse_string_matrix(lines, lines_count, &rows_a, &cols_a, &rows_b, &cols_b);
        
        printf("rows_a = %d, cols_a = %d, rows_b = %d, cols_b = %d\n\n", rows_a, cols_a, rows_b, cols_b);
        int** input_arr = convert_to_arr(mapper_inputs);
        int* send_counts = (int*) calloc((world_size - 1), sizeof(int));
        int* dspls = (int*) calloc((world_size - 1), sizeof(int));
        generate_scatter_data(send_counts, dspls, mapper_inputs->list_size, world_size - 1);
        

        for (int i = 0; i < world_size - 1; i++) {
            MPI_Send(&send_counts[i], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);

            for (int j = dspls[i]; j < dspls[i] + send_counts[i]; j++) {
                printf("Task Map assigned to process %d\n", i + 1);
                MPI_Send(input_arr[j], 4, MPI_INT, i + 1, 0, MPI_COMM_WORLD);

            }
        }


        // free up all the memory
        free(send_counts);
        free(dspls);
        for (int i = 0; i < lines_count; i++){
            free(lines[i]);
        }
        free(lines);
        for (int i = 0; i < mapper_inputs->list_size; i++){
            free(input_arr[i]);
        }
        free(input_arr);
        destory_matrix_input_list(mapper_inputs);


    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&rows_a, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols_b, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank != 0) { // Mapper Processes
        int recv_count = 0;

        MPI_Recv(&recv_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        
        int** recv_buf = (int**) malloc(recv_count * sizeof(int*));
        for (int i = 0; i < recv_count; i++){
            recv_buf[i] = (int*) malloc(4 * sizeof(int));
            printf("Process %d, received task Map on %s.\n", world_rank, "lol");
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
                    free(outputs[j]);
                }
                free(outputs);
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
                    free(outputs[j]);
                }
                free(outputs);
            }
        }

        for (int i = 0; i < recv_count; i++){
            free(recv_buf[i]);
        }
        free(recv_buf);
    }

    int num_keys = 0;
    if (world_rank == 0) {
        matrix_input_list* pairs =  init_matrix_input_list();
        int total_pairs = (rows_a * cols_a * cols_b) + (rows_b * cols_b * rows_a);
        for (int i = 0; i < total_pairs; i++) {
            
            matrix_format p;
            p.num_keys = 5;
            p.keys = (int*) malloc(p.num_keys * sizeof(int));
            MPI_Status status;

            MPI_Recv(p.keys, p.num_keys, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Process %d has completed task Map\n", status.MPI_SOURCE);

            insert_line(pairs, p);
        }




        // Shuffling process
        int** pairs_arr = convert_to_arr(pairs);
        matrix_input_list* keys = init_matrix_input_list();

        get_keys_list(keys, pairs_arr, total_pairs);
        int** keys_arr = convert_to_arr(keys);



        int* send_counts = (int*) calloc((world_size - 1), sizeof(int));
        int* dspls = (int*) calloc((world_size - 1), sizeof(int));
        generate_scatter_data(send_counts, dspls, keys->list_size, world_size - 1);

        for (int i = 0; i < world_size - 1; i++) {
            MPI_Send(&send_counts[i], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);

            for (int j = dspls[i]; j < dspls[i] + send_counts[i]; j++) {

                int count = 0;
                for (int k = 0; k < total_pairs; k++) {
                    if (pairs_arr[k][0] == keys_arr[j][0] && pairs_arr[k][1] == keys_arr[j][1]) {
                        count++;
                    }
                }

                MPI_Send(&count, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
                printf("Task Reduce assigned to process %d\n", i + 1);

                for (int k = 0; k < total_pairs; k++) {
                    if (pairs_arr[k][0] == keys_arr[j][0] && pairs_arr[k][1] == keys_arr[j][1]) {
                        MPI_Send(pairs_arr[k], 5, MPI_INT, i + 1, 2, MPI_COMM_WORLD);
                    }
                }

            }
        }

        free(send_counts);
        free(dspls);
        num_keys = keys->list_size;
        for (int i = 0; i < keys->list_size; i++){
            free(keys_arr[i]);
        }
        free(keys_arr);
        for (int i = 0; i < pairs->list_size; i++){
            free(pairs_arr[i]);
        }
        free(pairs_arr);

        destory_matrix_input_list(pairs);
        destory_matrix_input_list(keys);
        
    }


    if (world_rank != 0){

        int recv_count = 0;

        MPI_Recv(&recv_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
        int*** recv_buf = (int***) malloc(recv_count * sizeof(int*));
        int* value_count = (int *) malloc(recv_count * sizeof(int*));
        for (int i = 0; i < recv_count; i++){
            MPI_Recv(&value_count[i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD, NULL);
            printf("Process %d, received task Reduce on %s.\n", world_rank, "lol");

            recv_buf[i] = (int**) malloc(value_count[i] * sizeof(int*));

            for (int j = 0; j < value_count[i]; j++) {
                recv_buf[i][j] = (int*) malloc(5 * sizeof(int));
                MPI_Recv(recv_buf[i][j], 5, MPI_INT, 0, 2, MPI_COMM_WORLD, NULL);
            }
        }
        

        for (int i = 0; i < recv_count; i++) { //number of pairs the reducer has to deal with
            int result = 0;
            for (int j = 0; j < value_count[i]; j++){
                
                if (recv_buf[i][j][2] == 0) {
                    int common_index = recv_buf[i][j][3];
                    int val = recv_buf[i][j][4];

                    for (int k = 0; k < value_count[i]; k++) {
                        if (recv_buf[i][k][2] == 1 && common_index == recv_buf[i][k][3]) {
                            result += val * recv_buf[i][k][4];
                        }
                    }
                }


            }

            int* send_res_buf = (int *) malloc(3 * sizeof(int));
            send_res_buf[0] = recv_buf[i][0][0];
            send_res_buf[1] = recv_buf[i][0][1];
            send_res_buf[2] = result;

            MPI_Send(send_res_buf, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }


        for (int i = 0; i < recv_count; i++){

            for (int j = 0; j < value_count[i]; j++){
                free(recv_buf[i][j]);
            }
            free(recv_buf[i]);
        }
        free(recv_buf);

    }

    
    if (world_rank == 0) {
        int result_r = rows_a;
        int result_c = cols_b;
        
        int** result_matrix = (int**) malloc(result_r * sizeof(int*));

        for (int i = 0; i < result_r; i++) {
            result_matrix[i] = (int*) malloc(result_c * sizeof(int));
        }

        for (int i = 0; i < num_keys; i++) {
            
            int* recv_buf = (int*) malloc(3 * sizeof(int));
            MPI_Status status;
            MPI_Recv(recv_buf, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Process %d has completed task Reduce\n", status.MPI_SOURCE);

            result_matrix[recv_buf[0]][recv_buf[1]] = recv_buf[2];
            
            free(recv_buf);
        }

        write_matrix_to_file(output_file, result_matrix, result_r, result_c);

    }



    MPI_Finalize();

    return 0;
}