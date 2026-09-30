#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "mnt.h"
#include "fonctions.h"
using namespace std;


void lecture(string filename, int *nb_lignes, int *nb_cols, float *no_value, float** terrain) {
    ifstream f(filename);
    f>>*nb_lignes;
    f>>*nb_cols;
    int tmp;
    f>>tmp;
    f>>tmp;
    f>>tmp;
    f>>*no_value;
    *terrain = new float[(*nb_lignes+2)*(*nb_cols)];

    for (int i = 0; i < *nb_lignes; i++)
        for (int j = 0; j < *nb_cols; j++)
            f>>(*terrain)[(i+1) * (*nb_cols) + j];

     for (int j = 0; j < *nb_cols; j++) {
            (*terrain)[j] = *no_value;
            (*terrain)[(*nb_lignes + 1) * (*nb_cols) + j] = *no_value;
        }

}

void affichageTerrain(int nb_lignes, int nb_cols, float* terrain) {
    for (int i = 0; i < nb_lignes; i++) {
        for (int j = 0; j < nb_cols; j++)
            std::cout << terrain[(i+1) * nb_cols + j] << " ";
        std::cout << std::endl;
    }
}

int* initialisationDirection(int nb_lignes, int nb_cols) {
    int* direction = new int[(nb_lignes+2)*nb_cols];
    for (int i = 0; i < nb_cols; i++) {
           direction[i] = 1;
           direction[(nb_lignes + 1) * nb_cols + i] = 5;
       }
    for (int i = 0; i < nb_lignes; i++)
        for (int j = 0; j < nb_cols; j++)
            direction[(i+1) * nb_cols + j] = no_dir_value;
    return direction;
}


void calculDirection(int nb_lignes, int nb_cols, float no_value, float* terrain, int* direction) {
	int x, y;
    int x1, y1;
    int x2, y2;
    float tab[8];
    float tab_bord[5];
    for (int i = 0; i < nb_lignes; i++) {
      x = i+1 ;
      x1 = x - 1;
      x2 = x + 1;
      for (int j = 0; j < nb_cols; j++) {
	y = j;
	y1 = y-1;
	y2 = y+1;
	float val = terrain[x * nb_cols + y];

	if (val != no_value) {
	  if (j==0) {
	    tab_bord[0] = terrain[x1 * nb_cols + y];
	    tab_bord[1] = terrain[x1 * nb_cols + y2];
	    tab_bord[2] = terrain[x * nb_cols + y2];
	    tab_bord[3] = terrain[x2 * nb_cols + y2];
	    tab_bord[4] = terrain[x2 * nb_cols + y];
	    direction[(i+1)*nb_cols+ j] = f_bord1(val,tab_bord,no_value);
	  }
	  else if (j==(nb_cols-1)) {
	    tab_bord[0] = terrain[x2 * nb_cols + y];
	    tab_bord[1] = terrain[x2 * nb_cols + y1];
	    tab_bord[2] = terrain[x * nb_cols + y1];
	    tab_bord[3] = terrain[x1 * nb_cols + y1];
	    tab_bord[4] = terrain[x1 * nb_cols + y];
	    direction[(i+1)*nb_cols + j] = f_bord2(val,tab_bord,no_value);
	  }
	  else {
	    tab[0] = terrain[x1 * nb_cols + y];
	    tab[1] = terrain[x1 * nb_cols + y2];
	    tab[2] = terrain[x * nb_cols + y2];
	    tab[3] = terrain[x2 * nb_cols + y2];
	    tab[4] = terrain[x2 * nb_cols + y];
	    tab[5] = terrain[x2 * nb_cols + y1];
	    tab[6] = terrain[x * nb_cols + y1];
	    tab[7] = terrain[x1 * nb_cols + y1];
	    direction[(i+1)* nb_cols + j]=f(val, tab,no_value);
	  }
	}
      }
    }
}


void affichageDirection(int nb_lignes, int nb_cols, int* direction) {
    for (int i = 0; i < nb_lignes+2; i++) {
        for (int j = 0; j < nb_cols; j++) {
            cout << direction[i * nb_cols + j] << " ";
        }
        cout << endl;
    }
}

int* initialisationAccumulation(int nb_lignes, int nb_cols) {
    int* accumulation = new int[(nb_lignes+2)*nb_cols];
     for (int i = 0; i < nb_cols; i++) {
            accumulation[i] = 0;
            accumulation[(nb_lignes + 1) * nb_cols + i] = 0;
        }
        // Le reste de la matrice est initialisée à -1 pour non marqué.
        for (int i = 0; i < nb_lignes; i++)
            for (int j = 0; j < nb_cols; j++)
                accumulation[(i + 1) * nb_cols + j] = no_acc_value;
    return accumulation;
}


void calculAccumulation(int nb_lignes, int nb_cols, int* direction, int* accumulation) {
    int dir_bord1[5] = {5, 6, 7, 8, 1};
    int dir_bord2[5] = {1, 2, 3, 4, 5};
    int dir_general[8] = {5, 6, 7, 8, 1, 2, 3, 4};
    unsigned stop = 0;
    int nb_non_calculs = 0;
    int x, y;
    int x1, y1;
    int x2, y2;
    int tab[16];
    int tab_bord[10];

    while (stop == 0) {
        for (int i = 0; i < nb_lignes; i++) {
            x = i + 1;
            x1 = x - 1;
            x2 = x + 1;
            for (int j = 0; j < nb_cols; j++) {
                y = j;
                y1 = y - 1;
                y2 = y + 1;
                int d = direction[x * nb_cols + y];
                if (d != no_dir_value) {
                    if (accumulation[x * nb_cols + y] == -1) {
                        if (j == 0) {
                            tab_bord[0] = accumulation[x1 * nb_cols + y];
                            tab_bord[1] = accumulation[x1 * nb_cols + y2];
                            tab_bord[2] = accumulation[x * nb_cols + y2];
                            tab_bord[3] = accumulation[x2 * nb_cols + y2];
                            tab_bord[4] = accumulation[x2 * nb_cols + y];
                            tab_bord[5] = direction[x1 * nb_cols + y];
                            tab_bord[6] = direction[x1 * nb_cols + y2];
                            tab_bord[7] = direction[x * nb_cols + y2];
                            tab_bord[8] = direction[x2 * nb_cols + y2];
                            tab_bord[9] = direction[x2 * nb_cols + y];
                            int res = f_acc(tab_bord, no_dir_value, dir_bord1, 5);
                            if (res != -1)
                                accumulation[x * nb_cols + j] = res;
                            else {
                                nb_non_calculs++;
                            }
                        } else if (j == (nb_cols - 1)) {
                            tab_bord[0] = accumulation[x2 * nb_cols + y];
                            tab_bord[1] = accumulation[x2 * nb_cols + y1];
                            tab_bord[2] = accumulation[x * nb_cols + y1];
                            tab_bord[3] = accumulation[x1 * nb_cols + y1];
                            tab_bord[4] = accumulation[x1 * nb_cols + y];
                            tab_bord[5] = direction[x2 * nb_cols + y];
                            tab_bord[6] = direction[x2 * nb_cols + y1];
                            tab_bord[7] = direction[x * nb_cols + y1];
                            tab_bord[8] = direction[x1 * nb_cols + y1];
                            tab_bord[9] = direction[x1 * nb_cols + y];

                            int res = f_acc(tab_bord, no_dir_value, dir_bord2, 5);
                            if (res != -1)
                                accumulation[x * nb_cols + j] = res;
                            else {
                                nb_non_calculs++;
                            }
                        } else {
                            tab[0] = accumulation[x1 * nb_cols + y];
                            tab[1] = accumulation[x1 * nb_cols + y2];
                            tab[2] = accumulation[x * nb_cols + y2];
                            tab[3] = accumulation[x2 * nb_cols + y2];
                            tab[4] = accumulation[x2 * nb_cols + y];
                            tab[5] = accumulation[x2 * nb_cols + y1];
                            tab[6] = accumulation[x * nb_cols + y1];
                            tab[7] = accumulation[x1 * nb_cols + y1];
                            tab[8] = direction[x1 * nb_cols + y];
                            tab[9] = direction[x1 * nb_cols + y2];
                            tab[10] = direction[x * nb_cols + y2];
                            tab[11] = direction[x2 * nb_cols + y2];
                            tab[12] = direction[x2 * nb_cols + y];
                            tab[13] = direction[x2 * nb_cols + y1];
                            tab[14] = direction[x * nb_cols + y1];
                            tab[15] = direction[x1 * nb_cols + y1];
                            int res = f_acc(tab, no_dir_value, dir_general, 8);
                            if (res != -1)
                                accumulation[x * nb_cols + j] = res;
                            else
                                nb_non_calculs++;
                        }
                    }
                }
            }
        }
        if (nb_non_calculs == 0)
            stop = 1;
        else
            nb_non_calculs = 0;
    }
}

void affichageAccumulation(int nb_lignes, int nb_cols, int* accumulation) {
    for (int i = 0; i < nb_lignes+2; i++) {
        for (int j = 0; j < nb_cols; j++)
            std::cout << accumulation[i * nb_cols + j] << " ";
        std::cout << std::endl;
    }
}

int* initialisationBassin(int nb_lignes, int nb_cols) {
    int* bassin = new int[(nb_lignes+2)*nb_cols];
    for (int i=0; i<nb_lignes+2; i++)
            for (int j=0; j<nb_cols; j++)
                bassin[i*nb_cols+j] = no_bassin_value;
    return bassin;
}


void calculBassin(int nb_lignes, int nb_cols, int* direction, int* bassin) {
    int num = premiere_etiquette;
    int* ptr_direction = direction+nb_cols;
    for (int i = 0; i < nb_lignes; i++)
        for (int j = 0; j < nb_cols; j++) {
            if (ptr_direction[i * nb_lignes + j] != no_dir_value) {
                if (bassin[(i+1) * nb_cols + j] == -1)
                    bassin[(i+1) * nb_cols + j] = f_bassin(bassin+nb_cols, ptr_direction, nb_lignes, nb_cols, i, j, &num);
            }
            else {
                bassin[(i+1)*nb_cols+j] = no_bassin_value;
            }
        }
}

void affichageBassin(int nb_lignes, int nb_cols, int* bassin) {
    for (int i = 0; i < nb_lignes; i++) {
        for (int j = 0; j < nb_cols; j++) {
            cout << bassin[(i+1) * nb_cols + j] << " ";
        }
        cout << endl;
    }
}
