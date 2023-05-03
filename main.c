#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "nodemanager.h"


typedef struct {

    int** matrix_a;
    int** matrix_b;

} Matrices;

Matrices parse_string_matrix(char** lines) {

    // return matrix A and B in the given struct return type


}

int main(int argc, char **argv) {
    // Initialize MPI
    int world_rank, world_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const char* filename = "test.txt"; // extract from the commandline params

    if (world_rank == 0) { // Nodemanager

        char** lines = split_input(filename, world_size - 1);
        
        receive_mapper_output();

        shuffle_and_send_to_reducers();

        process_reducer_output();
    } else { // Mappers and Reducers
        // Implement mappers and reducers logic here
    }




    MPI_Finalize();

    return 0;
}