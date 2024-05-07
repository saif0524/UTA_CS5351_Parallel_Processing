#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size, init_data_size, max_data_size, increment;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2 || size > 16 ) {
        if (rank == 0) {
            printf( "This program requires at least 2 but no more than 16 processors.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    else if(argc != 4){
        printf("\n# USAGE: <OBJECT_FILE> <INITIAL_DATA_SIZE_IN_BYTE> <MAX_DATA_SIZE_IN_BYTE> <INCREMENT_IN_BYTE>\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    //int max_data_size = 128 * 1024; // up to 128 KB
    init_data_size = atoi(argv[1]);
    max_data_size = atoi(argv[2]);
    increment = atoi(argv[3]);  
    int *buffer = malloc(max_data_size * sizeof(int));

    if (rank == 0) {
        printf("Data size (bytes),Time (seconds)\n");
    }

    for (int data_size = init_data_size; data_size <= max_data_size; data_size += increment) {

        if (rank == 0) {
            // Initialize the buffer with some data
            for (int i = 0; i < data_size; i++) {
                buffer[i] = rand() % 100;
            }
        }
        
        // Synchronize before starting timing
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();

        // Performing broadcast
        MPI_Bcast(buffer, data_size, MPI_BYTE, 0, MPI_COMM_WORLD);

        // Synchronize again to ensure all processes received the data
        MPI_Barrier(MPI_COMM_WORLD);
        double end_time = MPI_Wtime();

        // Only root prints the time
        if (rank == 0) {
            printf("%d,%.6f\n", data_size, end_time - start_time);
        }
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}