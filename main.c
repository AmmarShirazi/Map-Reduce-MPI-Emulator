#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "nodemanager.h"






MPI_Datatype create_matrix_format_mpi_datatype() {
    MPI_Datatype matrix_format_type;

    // Create a datatype for the integer array (keys)
    MPI_Datatype keys_type;
    MPI_Type_contiguous(4, MPI_INT, &keys_type);
    MPI_Type_commit(&keys_type);

    // Create a datatype for the struct
    MPI_Datatype types[] = {MPI_INT, keys_type};
    int block_lengths[] = {1, 1};
    MPI_Aint offsets[] = {offsetof(matrix_format, num_keys), offsetof(matrix_format, keys)};

    MPI_Type_create_struct(2, block_lengths, offsets, types, &matrix_format_type);
    MPI_Type_commit(&matrix_format_type);

    MPI_Type_free(&keys_type);

    return matrix_format_type;
}

int main(int argc, char **argv) {
    // Initialize MPI
    int world_rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Datatype matrix_format_type = create_matrix_format_mpi_datatype();

    int* send_counts = (int*) malloc((world_size - 1) * sizeof(int));;
    int* dspls = (int*) malloc((world_size - 1) * sizeof(int));;
    matrix_format* input_arr = NULL;

    const char* filename = "input.txt"; // extract from the commandline params

    if (world_rank == 0) { // Nodemanager
        int lines_count = 0;
        char** lines = read_input(filename, &lines_count);
        matrix_input_list* mapper_inputs = parse_string_matrix(lines, lines_count);

        for (int i = 0; i < mapper_inputs->list_size; i++) {
            matrix_format m = get_at_index(mapper_inputs, i);
            printf("matrix: %d, i: %d, j: %d, val: %d\n", m.keys[0], m.keys[1], m.keys[2], m.keys[3]);
        }
        input_arr = convert_to_arr(mapper_inputs);

        generate_mapper_inputs(send_counts, dspls, input_arr, mapper_inputs, world_size - 1, world_rank);



    }

    MPI_Bcast(send_counts, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(dspls, world_size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank != 0) {
        int recv_count = send_counts[world_rank - 1];
        printf("Rank: %d, recvcount: %d\n", world_rank, recv_count);
    }




    MPI_Type_free(&matrix_format_type);
    MPI_Finalize();

    return 0;
}