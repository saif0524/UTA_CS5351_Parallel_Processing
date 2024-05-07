#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Check for correct usage
    if (argc != 2) {
        if (rank == 0) {  // Only master process issues the error message
            printf("Usage: <obj_file> <array_size>\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    int array_size = atoi(argv[1]);
    if (array_size <= 0) {
        if (rank == 0) {
            printf("Array size must be a positive integer\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    // Allocate memory for send and receive buffers
    int* send_data = (int*) malloc(array_size * size * sizeof(int));
    int* recv_data = (int*) malloc(array_size * size * sizeof(int));
    if (send_data == NULL || recv_data == NULL) {
        printf("Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize send data
    for (int i = 0; i < array_size; i++) {
        send_data[i] = rank * array_size + i;
    }

    // Execute MPI_Alltoall
    double start_time = MPI_Wtime();
    
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Alltoall(send_data, array_size/size, MPI_INT, recv_data, array_size/size, MPI_INT, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    // Print timing information from the master process
    if (rank == 0) {
        printf("\nTime taken for MPI_AlltoAll with array size %d on %d processors: %f seconds\n", array_size, size, end_time - start_time);
    }



    // Display sent and received data for each process
    printf("\nProcessor %d, initial array: ", rank);
    for (int i = 0; i < array_size; i++) {
        printf("%d ", send_data[i]);
    }

    printf("\nProcessor %d, final array: ", rank);
    for (int i = 0; i < array_size; i++) {
        printf("%d ", recv_data[i]);
    }
    printf("\n\n");


    // Free allocated memory and finalize MPI
    free(send_data);
    free(recv_data);
    MPI_Finalize();
    return 0;
}
