#include <iostream>
#include "mnt.h"
#include <iostream>
#include <mpi.h>
#include "mnt.h"

// Echange des lignes fantômes d'un tableau d'entiers (nb_cols colonnes,
// n_loc lignes utiles + 1 ligne fantôme en haut et en bas).
static void echangeGhosts(int* tab, int n_loc, int nb_cols, int haut, int bas) {
    int* premiere = tab + nb_cols;                 // 1ere ligne utile
    int* derniere = tab + n_loc * nb_cols;         // derniere ligne utile
    int* ghost_haut = tab;
    int* ghost_bas = tab + (n_loc + 1) * nb_cols;
    MPI_Sendrecv(premiere, nb_cols, MPI_INT, haut, 0,
                 ghost_bas, nb_cols, MPI_INT, bas, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(derniere, nb_cols, MPI_INT, bas, 1,
                 ghost_haut, nb_cols, MPI_INT, haut, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

// Une seule passe du calcul d'accumulation (mnt.cpp n'est pas modifié : sa version
// boucle jusqu'à ce que tout soit calculé, ce qui ne finit jamais en parallèle
// quand une cellule attend une valeur fantôme). Retourne le nombre de cellules calculées.
static int passeAccumulation(int n_loc, int nb_cols, int* dir, int* acc) {
    const int di[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dj[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    int nb_modifs = 0;
    for (int i = 1; i <= n_loc; i++)
        for (int j = 0; j < nb_cols; j++) {
            if (dir[i * nb_cols + j] == no_dir_value || acc[i * nb_cols + j] != -1) continue;
            int somme = 0;
            bool ok = true;
            for (int k = 0; k < 8 && ok; k++) {
                int jj = j + dj[k];
                if (jj < 0 || jj >= nb_cols) continue;
                int idx = (i + di[k]) * nb_cols + jj;
                if (dir[idx] == ((k + 4) % 8) + 1) {   // ce voisin se déverse sur (i,j)
                    if (acc[idx] == -1) ok = false;
                    else somme += acc[idx];
                }
            }
            if (ok) { acc[i * nb_cols + j] = somme + 1; nb_modifs++; }
        }
    return nb_modifs;
}

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


    int n_loc = nb_lignes / nprocs;
    int haut = (pid > 0) ? pid - 1 : MPI_PROC_NULL;
    int bas  = (pid < nprocs - 1) ? pid + 1 : MPI_PROC_NULL;

    int* dir_local = initialisationDirection(n_loc, nb_cols);
    calculDirection(n_loc, nb_cols, no_value, terrain_local, dir_local);

    // IMPORTANT : les directions des lignes fantômes doivent venir des voisins
    // (sinon les écoulements d'un processus vers l'autre sont ignorés).
    echangeGhosts(dir_local, n_loc, nb_cols, haut, bas);

    int* direction = nullptr;
    if (pid == root)
        direction = initialisationDirection(nb_lignes, nb_cols);

    MPI_Gather(dir_local + nb_cols, n_loc * nb_cols, MPI_INT,
            direction + nb_cols, n_loc * nb_cols, MPI_INT,
            root, MPI_COMM_WORLD);

    if (pid == root) {
        std::cout << "Directions :" << std::endl;
        affichageDirection(nb_lignes, nb_cols, direction);
        delete[] direction;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //FLOTS D'ACCUMULATION

    //init
    int* flot_local = initialisationAccumulation(n_loc, nb_cols);
    int nb_vides_avant = -1;
    int global_change = 1;

    while (global_change) {
        echangeGhosts(flot_local, n_loc, nb_cols, haut, bas);
        passeAccumulation(n_loc, nb_cols, dir_local, flot_local);
        int nb_vides = 0;
        for (int i = nb_cols; i < nb_cols + n_loc * nb_cols; i++)
            if (flot_local[i] == -1) nb_vides++;
        int local_change = (nb_vides != nb_vides_avant);
        nb_vides_avant = nb_vides;
        MPI_Allreduce(&local_change, &global_change, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    }

    int* accumulation = nullptr;
    if(pid == root)
        accumulation = initialisationAccumulation(nb_lignes, nb_cols);
    
    MPI_Gather(flot_local + nb_cols, n_loc * nb_cols, MPI_INT,
            accumulation + nb_cols, n_loc * nb_cols, MPI_INT,
            root, MPI_COMM_WORLD);
    if(pid == root) {
        std::cout << "Flots d'accumulation :" << std::endl;
        affichageAccumulation(nb_lignes, nb_cols, accumulation);
    }

    delete[] dir_local;
    delete[] accumulation;
    delete[] flot_local;
    if (pid==root)
        delete[] terrain;
    delete[] terrain_local;
    delete[] terrain_info;

    MPI_Finalize();
    return 0;
}