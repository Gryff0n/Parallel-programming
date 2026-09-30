#include <iostream>
#include "mnt.h"
#include <iostream>
#include <mpi.h>
#include "mnt.h"

int main(int argc, char **argv) {
    int pid, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    char* filename = argv[1];
    int root = atoi(argv[2]);

    int nb_lignes, nb_cols;
    float no_value;

    float* terrain;

    float* terrain_info = new float[3];

    if (pid==root) {
        lecture(filename, &nb_lignes, &nb_cols, &no_value, &terrain);
        std::cout << "le terrain" << std::endl;
        affichageTerrain(nb_lignes, nb_cols, terrain);

        terrain_info[0] = (float) nb_lignes;
        terrain_info[1] = (float) nb_cols;
        terrain_info[2] = no_value;
    } 
    MPI_Bcast(terrain_info, 3, MPI_FLOAT, root, MPI_COMM_WORLD);

    int localSize = ((nb_lignes)*nbcols)/nprocs;
    float* terrain_local = float[localSize+2];

    MPI_Scatter(terrain, localSize, MPI_FLOAT,terrain_local, localSize, MPI_FLOAT, root,MPI_COMM_WORLD);

//A Continer



    if (pid==root)
        delete[] terrain;

    MPI_Finalize();
    return 0;
}