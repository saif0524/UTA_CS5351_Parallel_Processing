#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size, input, max = -99999; // Initialize max to a default value

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc-size != 1 ) {
        if (rank == 0) {
            printf("\n# USAGE: <OBJECT_FILE: %s> <INPUT_P1> <INPUT_P2> ... ... <INPUT_PN> #\n\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
    }
    
    input = atoi(argv[rank + 1]); // Each process(or) gets its input as a command line argument
    printf("Processor %d input: %d", rank, input);
    

    MPI_Reduce(&input, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("The maximum value is: %d\n", max);
    }

    MPI_Finalize();
    
    return 0;
}
