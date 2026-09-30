//
// Created by sophie on 20/11/24.
//

#ifndef FONCTIONS_H
#define FONCTIONS_H

int f_bord1(float ref, float* tab, float no_value);
int f_bord2(float ref, float* tab, float no_value);
int f(float ref, float* tab, float no_value);
int f_acc(int *tab, int no_value, int *dir_bord, int l) ;
int f_bassin(int *bassin, int *dir, int n_l, int n_c, int i, int j, int* num);


#endif //FONCTIONS_H
