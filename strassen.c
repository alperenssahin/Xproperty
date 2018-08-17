#include  <stdlib.h>
#include  <stdio.h>
#include  <sys/time.h>
#include  <time.h>
#include  <assert.h>
#include "strassen.h"
#include "models.h"

/* Zaman  olcum  fonksiyonlari  */
int timeval_subtract(struct timeval *result,
                     struct timeval *t2, struct timeval *t1) {
    long int diff =
            (t2->tv_usec + 1000000 * t2->tv_sec) -
            (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    return (diff < 0);
}

/* Bir  matrisi  rasgele  sayilarla  dolduran  fonksiyon  */
void matrand(int n, int pitch, double M[]) {
    const double r = 10.0;
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            M[i * pitch + j] = r * (2 * drand48() - 1);
        }
    }
}

/*  Matrisi  ekrana  yazdiran  fonksiyon  */
void matprint(int n, int pitch, const double M[]) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%10.2f ", M[i * pitch + j]);
        }
        printf("\n");
    }
}

/*  Matris  toplami: S = A + B */
void matadd(int n,
            int Apitch, const double A[],
            int Bpitch, const double B[],
            int Spitch, double S[]) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            S[i * Spitch + j] = A[i * Apitch + j] + B[i * Bpitch + j];
        }
    }
}

/*  Matris  farki: S = A - B */
void matsub(int n,
            int Apitch, const double A[],
            int Bpitch, const double B[],
            int Spitch, double S[]) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            S[i * Spitch + j] = A[i * Apitch + j] - B[i * Bpitch + j];
        }
    }
}

/* TODO: Klasik  matris  carpimi  algoritmasi: C = A * B */
void matmult(int n,
             int Apitch, const double A[],
             int Bpitch, const double B[],
             int Cpitch, double C[]) {
    int i, j, k;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            double sum = 0.0;
            for (k = 0; k < n; ++k) {
                sum += A[i * Apitch + k] * B[k * Bpitch + j];
            }
            C[i * Cpitch + j] = sum;
        }
    }
}

//            _     _            _    _
//      X = | A   B |     Y = | E F |
//           | C   D |          | G H |
//      P0 = A * (F - H)
//      P1 = (A + B) * H
//      P2 = (C + D) * E
//      P3 = D * (G - E)
//      P4 = (A + D) * (E + H)
//      P5 = (B - D) * (G + H)
//      P6 = (A - C) * (E + F)
//          _                                                     _
//    Z = | (P3 + P4) + (P5 - P1)    P0 + P1                 |
//         | P2 + P3                     (P0 + P4) - (P2 + P6)|
//          -                                                     -
//
void matmult_fast(int n,
                  int Xpitch, const double X[],
                  int Ypitch, const double Y[],
                  int Zpitch, double Z[], int min_mat_recurse) {
//  Recursive  base  case.
// If  matrices  are  min_mat_recurse x min_mat_recurse  or  smaller  we just  use
// the  conventional  algorithm.
    if (n <= min_mat_recurse) {
        matmult(n, Xpitch, X, Ypitch, Y, Zpitch, Z);
        return;
    }
/* Bu  cagridaki alt -matrislerin  boylari n/2  olacak  */
    const int new_n = n / 2;

/* TODO: 7 adet Px  hesabi  icin   yer  ayirin  */
    const int sz = new_n * new_n * sizeof(double);
    double *P[7];
    int i;
    for (i = 0; i < 7; ++i)
        P[i] = malloc(sz);
/* TODO: Toplama  ve  cikarmalar  icin  gecici T ve U matrisleri  icin  yer
ayirin  */
    double *T = malloc(sz);
    double *U = malloc(sz);
/* TODO: A-B-C-D matrislerinin  baslangic  adreslerini  ayarlayin
* (Hepsi X matrisinin  icerisinde  gomulu) */
    const double *A = X;
    const double *B = X + new_n;
    const double *C = X + new_n * Xpitch;
    const double *D = C + new_n;
/* TODO: E-F-G-H matrislerinin  baslangic  adreslerini  ayarlayin
* (Hepsi Y matrisinin  icerisinde  gomulu) */
    const double *E = Y;
    const double *F = Y + new_n;
    const double *G = Y + new_n * Ypitch;
    const double *H = G + new_n;
/* TODO: P0 = A*(F - H) */
    matsub(new_n, Ypitch, F, Ypitch, H, new_n, T);
    matmult_fast(new_n, Xpitch, A, new_n, T, new_n, P[0], min_mat_recurse);
/* TODO: P1 = (A + B)*H */
    matadd(new_n, Xpitch, A, Xpitch, B, new_n, T);
    matmult_fast(new_n, new_n, T, Ypitch, H, new_n, P[1], min_mat_recurse);
/* TODO: P2 = (C + D)*E */
    matadd(new_n, Xpitch, C, Xpitch, D, new_n, T);
    matmult_fast(new_n, new_n, T, Ypitch, E, new_n, P[2], min_mat_recurse);
/* TODO: P3 = D*(G - E) */
    matsub(new_n, Ypitch, G, Ypitch, E, new_n, T);
    matmult_fast(new_n, Xpitch, D, new_n, T, new_n, P[3], min_mat_recurse);
/* TODO: P4 = (A + D)*(E + H) */
    matadd(new_n, Xpitch, A, Xpitch, D, new_n, T);
    matadd(new_n, Ypitch, E, Ypitch, H, new_n, U);
    matmult_fast(new_n, new_n, T, new_n, U, new_n, P[4], min_mat_recurse);
/* TODO: P5 = (B - D)*(G + H) */
    matsub(new_n, Xpitch, B, Xpitch, D, new_n, T);
    matadd(new_n, Ypitch, G, Ypitch, H, new_n, U);
    matmult_fast(new_n, new_n, T, new_n, U, new_n, P[5], min_mat_recurse);
/* TODO: P6 = (A - C)*(E + F) */
    matsub(new_n, Xpitch, A, Xpitch, C, new_n, T);
    matadd(new_n, Ypitch, E, Ypitch, F, new_n, U);
    matmult_fast(new_n, new_n, T, new_n, U, new_n, P[6], min_mat_recurse);
/*  Sonucun  hesaplanmasi  */
/* TODO: Z sol  ust = (P3 + P4) + (P5 - P1) */
    matadd(new_n, new_n, P[4], new_n, P[3], new_n, T);
    matsub(new_n, new_n, P[5], new_n, P[1], new_n, U);
    matadd(new_n, new_n, T, new_n, U, Zpitch, Z);
/* TODO: Z sol  alt = (P2 + P3) */
    matadd(new_n, new_n, P[2], new_n, P[3], Zpitch, Z + new_n * Zpitch);
/* TODO: Z sag  ust = (P0 + P1) */
    matadd(new_n, new_n, P[0], new_n, P[1], Zpitch, Z + new_n);
/* TODO: Z sag  alt = (P0 + P4) - (P2 + P6) */
    matadd(new_n, new_n, P[0], new_n, P[4], new_n, T);
    matadd(new_n, new_n, P[2], new_n, P[6], new_n, U);
    matsub(new_n, new_n, T, new_n, U, Zpitch, Z + new_n * (Zpitch + 1));
/* TODO: Gecici  pointerlar U ve T’yi free() edin */
    free(U);
    free(T);
/* TODO: P[]  dizisindeki  bellek  alanlarini  free() edin */
    for (i = 6; i >= 0; i--)
        free(P[i]);
}

//struct Matris *strassen_encoder(struct Matris *M) {
//    if (M->pitch == M->size / M->pitch) {
//        return M;
//    }
//    int newSize;
//    if (M->pitch < M->size / M->pitch) {
//        newSize = (M->size / M->pitch) * (M->size / M->pitch);
//    } else {
//        newSize = M->pitch * M->pitch;
//    }
//
//    struct Matris *TMP = new_Matris(newSize,newSize);
//    for(int i = 0; i<TMP->size;i++){
//        TMP->MAT[i] =
//    }
//
//}