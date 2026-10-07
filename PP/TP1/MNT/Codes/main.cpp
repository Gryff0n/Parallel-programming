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
        std::cout << "Terrain entier :" << std::endl;
        affichageTerrain(nb_lignes, nb_cols, terrain);

        terrain_info[0] = (float) nb_lignes;
        terrain_info[1] = (float) nb_cols;
        terrain_info[2] = no_value;

       
    } 
    MPI_Bcast(terrain_info, 3, MPI_FLOAT, root, MPI_COMM_WORLD);

    nb_lignes = (int) terrain_info[0];
    nb_cols = (int) terrain_info[1];
    no_value = terrain_info[2];

// if (pid==1) {
//      std::cout << "nb lignes : " << terrain_info[0]<< std::endl;

//         std::cout << "nb colonnes : " << terrain_info[1]<< std::endl;

//         std::cout << "no data : " << terrain_info[2]<< std::endl;
// }

    int nligne_local = (nb_lignes/nprocs)+2;

    int localSize = (nligne_local)*nb_cols ;
    float* terrain_local = new float[localSize];

    for (int i = 0; i < nb_cols; i++) {
        terrain_local[i] = no_value;                                   
        terrain_local[(nligne_local - 1) * nb_cols + i] = no_value;    
    }

    MPI_Scatter(terrain+nb_cols, (nb_lignes/nprocs)*nb_cols, MPI_FLOAT,terrain_local+nb_cols, (nb_lignes/nprocs)*nb_cols, MPI_FLOAT, root,MPI_COMM_WORLD);

    if(pid-1>=0) {
        MPI_Ssend(terrain_local+nb_cols, nb_cols, MPI_FLOAT, pid-1,  0, MPI_COMM_WORLD);
        MPI_Recv(terrain_local, nb_cols, MPI_FLOAT, pid-1, MPI_ANY_TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if(pid+1<=nprocs-1) {
        MPI_Recv(terrain_local+localSize-nb_cols, nb_cols, MPI_FLOAT, pid+1, MPI_ANY_TAG ,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Ssend(terrain_local+localSize-(2*nb_cols), nb_cols, MPI_FLOAT,  pid+1,  0, MPI_COMM_WORLD);
    }



    std::cout << "process  " << pid << " :" << std::endl;
    affichageTerrain(nligne_local, nb_cols, terrain_local-nb_cols);


//A Continer



    if (pid==root)
        delete[] terrain;

    delete[] terrain_local;
    delete[] terrain_info;

    MPI_Finalize();
    return 0;
}