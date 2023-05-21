#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

#define max_size 1000
#define num_elements 10

int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20};
int arraySlaves[max_size];


int array_sum_serial() {
    int i;
    long int sum;

    /* initialize array with numbers */
    /*for (i = 0; i < num_elements; i++) 
        array[i] = i;*/
   
    /* compute sum */
    sum = 0;
    for (i = 0; i < num_elements; i++) {
        sum += array[i];
    }

    return sum;
}


int main(int argc, char* argv[]) {
    int myrank, num_proc, elements_per_process, num_elements_recieved, num_elements_to_send;
    MPI_Status status;


    MPI_Init(&argc, &argv);

    //find number of processes and process id
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    if (myrank == 0) {

        int start_index, end_index, i;
        elements_per_process = num_elements / num_proc;

        if (num_proc > 1) {

            /*
                master process distributes the array into portions for each slave process
                to calculate it's partial sum
            */
            for (i = 1; i < num_proc - 1; i++) {
                start_index = i * elements_per_process;

                //master sends the number of elements and the partial array to each slave process
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&array[start_index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            //remaining elements added by last process
            start_index = i * elements_per_process;
 
            int elements_rem = num_elements - start_index;

            MPI_Send(&elements_rem, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array[start_index], elements_rem, MPI_INT, i, 0, MPI_COMM_WORLD);

        }

        // master process add its partial array
        int total_sum_parallel = 0;
        for (i = 0; i < elements_per_process; i++)
            total_sum_parallel += array[i];

        // master process receives and adds the partial sums from other processes
        int partial_sums;
        for (i = 1; i < num_proc; i++) {
            MPI_Recv(&partial_sums, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            int sender = status.MPI_SOURCE;
            total_sum_parallel += partial_sums;
        }

        // prints the final sum of array
        printf("Total sum of array calculated using MPI is: %d\n", total_sum_parallel);

        //validate the calculated sum by comparing it to serial array sum
        int total_sum_serial = array_sum_serial();

        if (total_sum_parallel == total_sum_serial) {
            printf("Total sum of array calculated using MPI is correct - same as serial: %d\n", total_sum_serial);
        }
        else {
            printf("Total sum of array calculated using MPI is incorrect - different from serial: %d\n", total_sum_serial);
        }
        
    }

    else {

        // each slave process receives the number of elements to add and it's partial array 
        MPI_Recv(&num_elements_recieved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&arraySlaves, num_elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // each slave process calculates its partial sum
        int partial_sum = 0;
        for (int i = 0; i < num_elements_recieved; i++)
            partial_sum += arraySlaves[i];

        // each slave process sends the partial sum to the master process
        MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
