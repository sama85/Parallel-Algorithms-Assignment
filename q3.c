#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

#define PI 3.1415926535

void integral_serial(int num_intervals) {
   
    double rect_width, area, total_sum, x_mid;

    rect_width = PI / num_intervals;

    total_sum = 0;
    for (int i = 1; i < num_intervals + 1; i++) {

        /* 1. compute the interval middle
           2. compute the rectangle height by sin function
           3. finally compute the area which is rect width * rect height (sin x_mid)
        */
        x_mid = (i - 0.5) * rect_width;
        area = sin(x_mid) * rect_width;
        total_sum = total_sum + area;
    }

    printf("The total area computed by serial integral is: %f\n", (float)total_sum);
}

int main() {
    
    int myrank, num_proc, num_intervals, i;
    double rect_width, area, sum, x_mid, partial_sum;
    MPI_Status status;

    MPI_Init(NULL, NULL);

    /* Find out MY process ID, and how many processes were started. */

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    //mark process 0 as the master process
    if (myrank == 0) {

        /* master proccess takes the number of intervals as input from user */
        printf("Please enter the number of intervals to integrate: ");
        scanf_s("%d", &num_intervals);
        integral_serial(num_intervals);
    }

    /* broadcast the number of intervals to all processes */
    MPI_Bcast(&num_intervals, 1, MPI_INT, 0,
        MPI_COMM_WORLD);

    /* each procces calculates the width of a rectangle
       then calculates the partial summation of areas for its own part/interval */
    rect_width = PI / num_intervals;
    partial_sum = 0;

    for (i = myrank + 1; i < num_intervals + 1; i += num_proc) {

        /* 
          1. compute the interval middle
          2. compute the rectangle height by sin function
          3. finally compute the area which is rect width * rect height (sin x_mid)
       */
        x_mid = (i - 0.5) * rect_width;
        area = sin(x_mid) * rect_width;
        partial_sum = partial_sum + area;
    }
  
    /* reduce all partial sums into variable sum at the master proccess */
    MPI_Reduce(&partial_sum, &sum, 1, MPI_DOUBLE,
        MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank == 0)
        printf("The total area computed by parallel integral using MPI is: %f\n", (float)sum);

    MPI_Finalize();
    return 0;
}

