
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: <obj_file> <array_size>\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int array_size = atoi(argv[1]);
    int* send_data = (int*) malloc(array_size * size * sizeof(int));
    int* recv_data = (int*) malloc(array_size * size * sizeof(int));

    if (!send_data || !recv_data) {
        printf("Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    printf("\nProcessor %d, inital array: ", rank);
    // Initialize send data
    for (int i = 0; i < array_size; i++) {

        send_data[i] = rank * array_size + i;
	printf("%d ", send_data[i]);
    }
     printf("\n");



    double start_time = MPI_Wtime();
    // Perform all-to-all communication
    for (int i = 0; i < size; i++) {
        if (i == rank) {
            // Copy own data directly to own part of recv_data
            memcpy(&recv_data[i * array_size/size], &send_data[i * array_size/size], array_size * sizeof(int));
        } else {
            MPI_Send(&send_data[i*array_size/size], array_size/size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    // Synchronizaion to ensure all sends are completed
    MPI_Barrier(MPI_COMM_WORLD); 

    for (int i = 0; i < size; i++) {
        if (i != rank) {
            MPI_Recv(&recv_data[i*array_size/size], array_size/size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all receives are completed
    

    double end_time = MPI_Wtime();
    if(rank==0){
        printf("Time taken for array size %d on %d processors: %lf\n", array_size, size, end_time-start_time);
    }
    // Print received data
    printf("\nProcessor %d, final array: ", rank);
    for (int i = 0; i < array_size; i++) {
        printf("%d ", recv_data[i]);
    }
    printf("\n");

    free(send_data);
    free(recv_data);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
