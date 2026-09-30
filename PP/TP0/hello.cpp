#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[])
{
  int pid, nprocs;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  if(pid%2==0) {
      cout << argv[1] << endl;
  }
  MPI_Finalize();
  return 0;
}

