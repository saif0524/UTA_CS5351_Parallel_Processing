#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_TAG 1

int main(int argc, char *argv[]) {
    int rank, size, init_data_size, max_data_size, increment;
    MPI_Status status;
    double start_time, end_time, total_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ensuring there are enough processors
    if (size < 2 || size > 8) {
        if (rank == 0) {
            printf("This program requires at least 2  but not more than 8 processors.\n");
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
    int* buffer = malloc(max_data_size * sizeof(int));
        
    if (rank == 0) {
        printf("Data size {bytes}, Round-trip time {seconds}\n");
        for (int data_size = init_data_size; data_size <= max_data_size; data_size += increment) {
            // Initialize buffer with some data
            for (int i = 0; i < data_size; i++) {
                buffer[i] = i % increment;
            }

            start_time = MPI_Wtime();
            MPI_Send(buffer, data_size, MPI_BYTE, 1, DATA_TAG, MPI_COMM_WORLD);
            MPI_Recv(buffer, data_size, MPI_BYTE, 1, DATA_TAG, MPI_COMM_WORLD, &status);
            end_time = MPI_Wtime();

            total_time = end_time - start_time;
            printf("%d,%f\n", data_size, total_time);
        }
    } else if (rank == 1) {
        for (int data_size = init_data_size; data_size <= max_data_size; data_size += increment) {
            MPI_Recv(buffer, data_size, MPI_BYTE, 0, DATA_TAG, MPI_COMM_WORLD, &status);
            MPI_Send(buffer, data_size, MPI_BYTE, 0, DATA_TAG, MPI_COMM_WORLD);
        }
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}
