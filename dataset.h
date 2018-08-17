#ifndef _DATASET_H
#define _DATASET_H

#include "hash.h"

#define MINYEAR 1500
#define YEARBUCKETS (2020 - 1500)
struct HouseData {
    int id;
    int lotArea;
    int neighborhood_id;
    int yearBuilt;
    int salePrice;
    struct HouseData *next;
};
struct Neighborhood {
    char *titre;
    unsigned int sum_price;
    int count;
    double Kn;
};
struct BuiltYear {
    int year;
    int count;
    unsigned int price;
    double Kb;
};

struct HouseList {
    struct HouseData *head;
    int count;
    struct Neighborhood *N_list[MAXBUCKETS];
    struct BuiltYear *B_list[YEARBUCKETS];
};


//, int **neighborhood_table,struct HouseData *head
void read_dataset_file(char *filename, struct HouseList *my_list);

void cleaner(struct HouseList *data);

void clean_neighborlist(struct Neighborhood **Neighborhood_list);

void write_neighborhoods(struct Neighborhood **Neighborhood_list);

void write_HouseData(struct HouseList list);

int count_houses(struct HouseList list);

int mean_sale_prices(struct HouseList list);

int mean_saleprices_neighborhood(struct HouseList list, const char *str);

void mean_saleprices_neighborhood_list(struct HouseList *list);

void mean_saleprices_year(struct HouseList *list);

int mean_year(struct HouseList *list, int year);

void clean_yearlist(struct BuiltYear **Year_list);

void min_max(struct HouseList *list);

struct HouseData *findByID(int id, struct HouseList *list);
//todo: foksyonlar rapora eklenecek 

void coeff_calculator(struct HouseList list);
#endif _DATASET_H
