#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <string.h>
#include"models.h"
#include "dataset.h"

void model_by_neighborhood(struct HouseList test, struct HouseList referance) {

    struct HouseData *my_data = test.head;

    while (my_data != NULL) {

        my_data->salePrice = mean_saleprices_neighborhood(referance, referance.N_list[my_data->neighborhood_id]->titre);
        my_data = my_data->next;
    }
    write_HouseData(test);
}


void save_price_list(struct HouseList test) {
    struct HouseData *my_data = test.head;

    FILE *fp = fopen("test_data.txt", "w");
    fprintf(fp, "ID,lotArea,Neighborhood,builtYear,salePrices\n");

    while (my_data != NULL) {
        fprintf(fp, "%d,%d,%s,%d,%d\n", my_data->id, my_data->lotArea, test.N_list[my_data->neighborhood_id]->titre,
                my_data->yearBuilt, my_data->salePrice);
        my_data = my_data->next;
    }
    fclose(fp);

}

struct Matris *new_Matris(int line, int pitch) {
    double A[line * pitch];
    struct Matris *M = malloc(sizeof(struct Matris));
    M->MAT = malloc(sizeof(A));
    memset(M->MAT, 0, sizeof(A));
    M->pitch = pitch;
    M->size = line * pitch;
    return M;
}

void matris_cleaner(struct Matris *M) {
    free(M->MAT);
    free(M);
}

void create_data_matrices(struct HouseList test, struct Matris *Y, struct Matris *X) {
    struct HouseData *head = test.head;
    int countX = 0;
    int countY = 0;

    while (head != NULL) {
        if(Y != NULL) {
            Y->MAT[countY] = head->salePrice;
            countY++;
        }
        X->MAT[countX] = 1;
        X->MAT[countX + 1] = head->lotArea;
        countX += 2;
        head = head->next;


    }
}

void print_matrice(struct Matris *M) {
    int count = 0;
    for (int i = 0; i < M->size / M->pitch; i++) {
        for (int y = 0; y < M->pitch; y++) {
            printf("%10.20f\t", M->MAT[count]);
            count++;
        }
        printf("\n");
    }
}

struct Matris *get_transpose(struct Matris *M) {
    struct Matris *T = new_Matris(M->pitch, M->size / M->pitch);
    if (M->pitch == 1) {
        T->MAT = M->MAT;
        return T;
    }

    for (int i = 0; i < T->pitch; i++) {
        for (int y = 0; y < M->pitch; y++) {
            T->MAT[y * T->pitch + i] = M->MAT[i * M->pitch + y];
        }
    }
    return T;
}

struct Matris *get_multiplication(struct Matris *X, struct Matris *Y) {
    if (X->pitch != Y->size / Y->pitch) {
        printf("Matrix size is not correct!");
        return NULL;
    }
    int new_size = (X->size / X->pitch) * Y->pitch;
    struct Matris *M = new_Matris(X->size / X->pitch, Y->pitch);

    for (int i = 0; i < X->size / X->pitch; i++) {
        for (int j = 0; j < Y->pitch; j++) {
            double sum = 0.0;
            for (int k = 0; k < X->pitch; k++) {
                sum += X->MAT[i * X->pitch + k] * Y->MAT[k * Y->pitch + j];
            }
            M->MAT[i * M->pitch + j] = sum;
        }
    }
    return M;
}


double get_determinant(struct Matris *M) {
    if (M->pitch != M->size / M->pitch) {
        printf("matrix is not square\n");
        return 0;
    }
    if (M->pitch == 2) {
        double det = (M->MAT[0] * M->MAT[3]) - (M->MAT[1] * M->MAT[2]);
        return det;
    }
    double det = 0;
    for (int i = 0; i < M->pitch; i++) {
        struct Matris *Z = partition(M, i * M->pitch);
        double step = pow(-1, i) * M->MAT[i * M->pitch] * get_determinant(Z);
//        print_matrice(Z);
//        printf("\n%f\n",step);
        det += step;
        matris_cleaner(Z);
    }
    return det;
}

struct Matris *partition(struct Matris *M, int position) {
    struct Matris *X = new_Matris(M->pitch - 1, M->pitch - 1);
    int n = position / M->pitch;
//    printf("%d",M->pitch);
    int toggle = 0;
    for (int i = 0; i < M->pitch; i++) {
        int m = i;
        if (toggle) {
            m -= 1;
        }
        if (n == i) {
            toggle = 1;
            continue;
        }
        if (m == X->pitch) break;
        for (int k = 0; k < X->pitch; k++) {
//            printf("\n%d\n",((n+i+1)%M->pitch)*M->pitch+k+1);

//            X->MAT[i*X->pitch+k] = M->MAT[((n+i+1)%M->pitch)*M->pitch+k+1];
            X->MAT[m * X->pitch + k] = M->MAT[i * M->pitch + k + 1];

        }
    }
    return X;
}


int check_inversible(struct Matris *M) {
    if (M->pitch != M->size / M->pitch) {
        printf("matrix is not square\n");
        return 0;
    }

    for (int i = 0; i < M->pitch; i++) {
        if (M->MAT[i * M->pitch + i] == 0) {
            printf("MAT[%d,%d] = 0!!", i, i);
            printf("matix is not inversible");
            return 0;
        }
    }
    return 1;
}

struct Matris *create_identite(int pitch) {
    struct Matris *M = new_Matris(pitch, pitch);
    for (int i = 0; i < pitch; i++) {
        M->MAT[i * pitch + i] = 1;
    }
    return M;
}

struct Matris *pivot_de_gauss(struct Matris *M) {
    struct Matris *TMP = new_Matris(M->pitch, M->pitch);
    copy_matrix(TMP, M);
    int col = 0;
    int is_zero = 0;
    int count = M->pitch;
    for (int line = 0; line < count; line++) {

        for (int i = line + 1; i < M->pitch; i++) {
            if (M->MAT[line * M->pitch + col] == 0) {
                for (int j = 0; j < M->pitch; j++) {
                    double tmp = M->MAT[line * M->pitch + j];
                    M->MAT[line * M->pitch + j] = M->MAT[(M->pitch - 1) * M->pitch + j];
                    M->MAT[(M->pitch - 1) * M->pitch + j] = tmp;
                }
                is_zero = 1;
                copy_matrix(TMP, M);
            }
            if (is_zero) {
                count--;
//                printf("%d\n",count);
                is_zero = 0;
            }
            for (int k = col; k < M->pitch; k++) {

                TMP->MAT[i * M->pitch + k] -=
                        (M->MAT[line * M->pitch + k] / M->MAT[line * M->pitch + col]) * M->MAT[i * M->pitch + col];
            }

        }
        count = M->pitch;
        col++;
        copy_matrix(M, TMP);
    }
    return TMP;
}

void copy_matrix(struct Matris *X, struct Matris *Y) {
    for (int i = 0; i < X->size; i++) {
        X->MAT[i] = Y->MAT[i];
    }
}

struct Matris *get_inverse(struct Matris *M) {
    struct Matris *Id = create_identite(M->pitch);

    if (M->pitch == 2) {
        double det = get_determinant(M);

        if (det == 0) {
            printf("MAT is not inversible\n");
            return NULL;
        } else {

            Id->MAT[0] = (M->MAT[3]);
            Id->MAT[1] = ((-1)*M->MAT[1]);
            Id->MAT[2] = ((-1)*M->MAT[2]);
            Id->MAT[3] = (M->MAT[0]);
            for(int i = 0; i<M->size; i++ ){
                Id->MAT[i] /= det;
            }
            return Id;
        }
    }
    for (int i = 0; i < M->pitch; i++) {
        double tmp_ii = M->MAT[i * M->pitch + i];
        if (tmp_ii == 0) {
            printf("MAT[%d],[%d] = 0\n", i, i);
            printf("MAT is not inversible\n");
            return Id;
        }
        for (int k = 0; k < M->pitch; k++) {
            M->MAT[i * M->pitch + k] = M->MAT[i * M->pitch + k] / tmp_ii;
            Id->MAT[i * M->pitch + k] = Id->MAT[i * M->pitch + k] / tmp_ii;
        }


        for (int j = 0; j < M->pitch; j++) {
            if (i == j) continue;
            double tmp_jj = M->MAT[j * M->pitch + i];
            for (int l = 0; l < M->pitch; l++) {
                M->MAT[j * M->pitch + l] -= M->MAT[i * M->pitch + l] * tmp_jj;
                Id->MAT[j * M->pitch + l] -= Id->MAT[i * M->pitch + l] * tmp_jj;
            }
        }
//        print_matrice(Id);
//        printf("I\n");
    }
//    printf("a\n");
//    print_matrice(M);
//    printf("a\n");
    return Id;
}

struct Matris *calculate_parameter(struct HouseList list){
    int count_ = count_houses(list);
//        int count_ = 100;
    struct Matris *PRICE = new_Matris(count_, 1);
    struct Matris *LOTAREA = new_Matris(count_, 2);
    struct Matris *MULTI;
//    struct Matris *M3 = get_multiplication(M,M2);



    create_data_matrices(list, PRICE, LOTAREA);
    struct Matris *TLOTAREA = get_transpose(LOTAREA);
    MULTI = get_multiplication(TLOTAREA,LOTAREA);
//    print_matrice(PRICE);
//    print_matrice(LOTAREA);
//    print_matrice(TLOTAREA);
//    print_matrice(MULTI);


    struct Matris *INVERS = get_inverse(MULTI);
//    print_matrice(INVERS);
    struct Matris *STEP1 = get_multiplication(INVERS,TLOTAREA);
//    print_matrice(STEP1);
    struct Matris *COF = get_multiplication(STEP1,PRICE);

    matris_cleaner(STEP1);
    matris_cleaner(INVERS);
    matris_cleaner(PRICE);
    matris_cleaner(LOTAREA);
    matris_cleaner(TLOTAREA);
    matris_cleaner(MULTI);
    return COF;
}