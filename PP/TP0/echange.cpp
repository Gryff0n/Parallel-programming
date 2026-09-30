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

    int a = pid;
    int received;

    int right = (pid + 1) % nprocs;
    int left = (pid - 1 + nprocs) % nprocs;

    MPI_Send(&a, 1, MPI_INT, right, tag, MPI_COMM_WORLD);
    MPI_Recv(&received, 1, MPI_INT, left, tag, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    cout << "je suis " << pid
         << " et j'ai recu " << received << endl;

    MPI_Finalize();
    return 0;
}

