#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

#include "helpers.hpp"

#include "mpi.h"

using namespace std;

int main(int argc, char **argv)
{
    int max_iterations = 1000;
    double epsilon = 1.0e-3;
    bool verify = true, print_config = true;

    //  MPI hint: remember to initialize MPI first 
    //int numprocs = 1; // Use MPI process count instead of this

    int numprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //printf("Hello from process %d out of %d\n", rank, numprocs);

    // Default values for M rows and N columns
    int N = 12;
    int M = 12;

    process_input(argc, argv, N, M, max_iterations, epsilon, verify, print_config);

    if ( print_config )
        std::cout << "Configuration: m: " << M << ", n: " << N << ", max-iterations: " << max_iterations << ", epsilon: " << epsilon << ", processes: " << numprocs << std::endl;

    //auto time_1 = chrono::high_resolution_clock::now(); // change to MPI_Wtime() / omp_get_wtime()
    auto time_1 = MPI_Wtime();

    // The main part of the code that needs to use MPI/OpenMP timing routines 
    
    int i, j;
    double diffnorm;
    int iteration_count = 0;

    int globalM = M;

    // saves how many rows each process has, important for MPI_GatherV
    int proc_row_cnt [numprocs];
    for (size_t i = 0; i < numprocs; i++){
        proc_row_cnt[i] = globalM/numprocs;
        if(rank == numprocs-1){
            proc_row_cnt[i] = globalM%numprocs + globalM/numprocs;
        }
    }

    // ghost region; 2 additional rows.
    M = proc_row_cnt[rank] + 2;


    Mat U(M, N); // MPI: use local sizes with MPI, e.g., recalculate M and N (e.g., M/numprocs + 2)
    Mat W(M, N); // MPI: use local sizes with MPI, e.g., recalculate M and N
    
    // define the iteration ranges of our 
    int start = 1;
    int end = M-1;


    // Init & Boundary (MPI: different)
    for (i = start; i < end; ++i) {
        for (j = 0; j < N; ++j) {
            W[i][j] = U[i][j] = 0.0;
        }

        W[i][0] = U[i][0] = 0.05; // left side
        W[i][N-1] = U[i][N-1] = 0.1; // right side
    }


    if(rank == 0){
        for (j = 0; j < N; ++j) {
            W[start][j] = U[start][j] = 0.02; // top 
        }
    }
    if(rank == numprocs-1){
        for (j = 0; j < N; ++j) {
            W[end - 1][j] = U[end - 1][j] = 0.2; // bottom 
        }
    }
    //cout << rank << '\n';
    //U.print();

    /*
    for (j = 0; j < N; ++j) {
        W[0][j] = U[0][j] = 0.02; // top 
        W[M - 1][j] = U[M - 1][j] = 0.2; // bottom 
    }*/
    // End init

    iteration_count = 0;
    int comp_start_row = start;
    int comp_end_row = end;

    MPI_Datatype MATRIX_ROW;
    MPI_Type_contiguous(N, MPI_DOUBLE, &MATRIX_ROW);
    MPI_Type_commit(&MATRIX_ROW);
    MPI_Request request;  

    if(rank == 0)
        comp_start_row++;
    if(rank == numprocs-1)
        comp_end_row--;

    do
    {
        iteration_count++;
        diffnorm = 0.0;
        
        // MPI Hint: ISend/IRecv the needed rows based on the position - to process above, below or both
        if(rank != numprocs-1){
            // sent down second to last row
            MPI_Isend(&U[M-2][0], 1, MATRIX_ROW, rank+1, 69, MPI_COMM_WORLD, &request);
        }

        if(rank != 0){
            //receive top row block
            MPI_Recv(&U[0][0], 1, MATRIX_ROW, rank-1, 69, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        if(rank != numprocs-1){
            // wait
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }

        if(rank != 0){
            //sent top 1th row
            MPI_Isend(&U[1][0], 1, MATRIX_ROW, rank-1, 420, MPI_COMM_WORLD, &request);
        }

        if(rank != numprocs-1){
            // receive from down last row block
            MPI_Recv(&U[M-1][0], 1, MATRIX_ROW, rank+1, 420, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if(rank != 0){
            // wait
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
        

        // Compute new values (but not on boundary) 
        // MPI: based on your process you may need to start and stop at the different row index
        for (i = comp_start_row; i < comp_end_row; ++i)
        {
            for (j = 1; j < N - 1; ++j)
            {
                W[i][j] = (U[i][j + 1] + U[i][j - 1] + U[i + 1][j] + U[i - 1][j]) * 0.25;
                diffnorm += (W[i][j] - U[i][j]) * (W[i][j] - U[i][j]);
            }
        }

        // Only transfer the interior points
        // MPI: based on your process you may need to start and stop at the different row index
        for (i = 1; i < M - 1; ++i)
            for (j = 1; j < N - 1; ++j)
                U[i][j] = W[i][j];

        // MPI: make sure that you have the total diffnorm on all processes for exit criteria
        MPI_Allreduce(MPI_IN_PLACE, &diffnorm, 1, MPI_DOUBLE, MPI_SUM,MPI_COMM_WORLD);
        diffnorm = sqrt(diffnorm); // all processes need to know when to stop
        
    } while (epsilon <= diffnorm && iteration_count < max_iterations);
    
    //auto time_2 = chrono::high_resolution_clock::now(); // change to MPI_Wtime() / omp_get_wtime()
    auto time_2 = MPI_Wtime();

    // TODO for MPI: collect all local parts of the U matrix, and save it to another "big" matrix
    // that has the same size the whole size: like bigU(M,N) with the whole matrix allocated. 
    // Hint: star with MPI_Gather then extend to MPI_Gatherv

    //displacements for gathering
    int displacements [numprocs];
    displacements[0]=0;
    for (size_t i = 1; i < numprocs; i++){
        displacements[i] = displacements[i-1] + proc_row_cnt[i-1];
    }

    Mat bigU(globalM,N);
    
    // gatherv
    MPI_Gatherv(&U[1][0],           // sendbuf
                proc_row_cnt[rank], // sendcount
                MATRIX_ROW,         // sendtype
                &bigU[0][0],        // recvbuf
                proc_row_cnt,       // recvcount
                displacements,      // displs
                MATRIX_ROW,         // recvtype
                0,                  // root
                MPI_COMM_WORLD);    // com

    // Print time measurements 
    cout << "Elapsed time: "; 
    cout << std::fixed << std::setprecision(4) << chrono::duration<double>(time_2 - time_1).count(); // remove for MPI/OpenMP
    // cout << std::fixed << std::setprecision(4) << (time_2 - time_1); // modify accordingly for MPI/OpenMP
    cout << " seconds, iterations: " << iteration_count << endl; 
 
    // Verification (required for MPI)
    if ( verify && rank == 0) {
        Mat U_sequential(globalM, N); // init another matrix for the verification

        int iteration_count_seq = 0;
        heat2d_sequential(U_sequential, max_iterations, epsilon, iteration_count_seq); 

        //U_sequential.print();

        // Here we need both results - from the sequential (U_sequential) and also from the OpenMP/MPI version, then we compare them with the compare(...) function 
        cout << "Verification: " << ( bigU.compare(U_sequential) && iteration_count == iteration_count_seq ? "OK" : "NOT OK") << std::endl;
    }

    //cout << rank;
    //U.print();
    

    MPI_Type_free(&MATRIX_ROW);
    // MPI: do not forget to call MPI_Finalize()
    MPI_Finalize();
    //U.save_to_disk("heat2d.txt"); // not needed
    
    //if(rank == 0)
    //    bigU.print();

    return 0;
}