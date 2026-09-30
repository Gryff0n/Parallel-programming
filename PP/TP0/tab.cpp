#include <iostream>
#include <mpi.h>


const int tag = 10;

using namespace std;

int main(int argc, char **argv)
{
    int pid, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int size = atoi(argv[1]);

    int* a = new int[size];
    for (int i = 0; i < size; i++)
    {
        a[i] = pid;
    } 
    int* received = new int[size];

    int right = (pid + 1) % nprocs;
    int left = (pid - 1 + nprocs) % nprocs;

    MPI_Sendrecv_replace(a, size, MPI_INT, right, tag, left, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    cout << "je suis " << pid
         << " et j'ai recu ";
    for (int i = 0; i < size; i++)
    {
        cout << a[i] << " ";
    } cout << endl;

    MPI_Finalize();
    return 0;
}