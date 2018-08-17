#ifndef _MODELS_H
#define _MODELS_H

#include "dataset.h"

struct Matris {
    double *MAT;
    int pitch;
    int size;
};

void model_by_neighborhood(struct HouseList test, struct HouseList referance);

void save_price_list(struct HouseList test);

void create_data_matrices(struct HouseList test, struct Matris *Y, struct Matris *X);

void print_matrice(struct Matris *M);

struct Matris *get_transpose(struct Matris *M);

struct Matris *get_multiplication(struct Matris *X, struct Matris *Y);

struct Matris *new_Matris(int line, int pitch);

void matris_cleaner(struct Matris *M);

double get_determinant(struct Matris *M);

struct Matris *partition(struct Matris *M, int position);

int check_inversible(struct Matris *M);

struct Matris *create_identite(int pitch);

struct Matris *pivot_de_gauss(struct Matris *M);

void copy_matrix(struct Matris *X, struct Matris *Y);

struct Matris *get_inverse(struct Matris *M);

struct Matris *calculate_parameter(struct HouseList list);


#endif _MODELS_H
