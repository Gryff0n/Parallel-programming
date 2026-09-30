#include <iostream>
#include "mnt.h"
#include "fonctions.h"

// fonction utile pour le calcul des directions lorsqu'on traite un élément sur la première colonne
// Il n'y a pas de valeur à gauche
int f_bord1(float ref, float* tab, float no_value)
{
    float min = ref;
    int code =0;
    for (int i=0; i<5; i++)
        if (tab[i]!=no_value) {
            if (tab[i] < min) {
                min = tab[i];
                code = i + 1;
            }
        }
    return code;
}
// fonction utile pour le calcul des directions lorsqu'on traite un élément sur la dernière colonne
// Il n'y a pas de valeur à droite
int f_bord2(float ref, float* tab, float no_value)
{
    float min = ref;
    int code = -1;
    for (int i=0; i<5; i++)
        if (tab[i]!=no_value) {
            if (tab[i]<min) {
                min=tab[i];
                code=i;
            }
        }
    if (code==4)
      return 1;
    else if (code==-1)
        return 0;
    else {
      return code+5;
      }
}

int f(float ref, float* tab, float no_value)
{
    float min = ref;
    int code = 0;
    for (int i=0; i<8; i++)
        if (tab[i]!=no_value) {
            if (tab[i] < min) {
                min = tab[i];
                code = i + 1;
            }
        }
    return code;
}

// fonction élémentaire utile pour le calcul des flots des accumulations.
int f_acc(int *tab, int no_value, int *dir_bord, int l) {
    int val = 0;
    int nb1 = 0;
    int nb2 = 0;
    for (int i = 0; i < l; i++) {
        if (tab[l + i] != no_value){
            if (tab[l + i] == dir_bord[i]) {
                nb1++;
                if (tab[i] != -1) {
                    val += tab[i];
                    nb2++;
                }
            }
        }
    }
    if (nb1 == nb2)
        val++;
    else
        val = no_acc_value;
    return val;
}

int f_bassin(int *bassin, int *dir, int n_l, int n_c, int i, int j, int* num) {
    if (dir[i * n_c + j] == 0) {
        if (bassin[i * n_c + j] == -1) {
            (*num)++;
            bassin[i * n_c + j] = (*num);
            return (*num);
        }
        else {
            return bassin[i * n_c +j];
        }
    }
    else if (bassin[i * n_c + j] != -1) {
        return bassin[i * n_c + j];
    }
    else {
        int etiquette;
        switch (dir[i * n_c + j]) {
            case 1:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i - 1, j, num);
                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 2:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i - 1, j + 1, num);
                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 3:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i, j + 1, num);
                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 4:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i + 1, j + 1, num);
                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 5:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i + 1, j, num);

                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 6:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i + 1, j - 1, num);

                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 7:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i, j - 1, num);

                bassin[i * n_c + j] = etiquette;
                return etiquette;
            case 8:
                etiquette = f_bassin(bassin, dir, n_l, n_c, i - 1, j - 1, num);

                bassin[i * n_c + j] = etiquette;
                return etiquette;
        }
    }
    return 0;
}


