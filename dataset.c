#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include"dataset.h"
#include "hash.h"

//, int ** neighborhood_table,struct HouseData *head


void read_dataset_file(char *filename, struct HouseList *my_list) {

    char str[50];
    int count = 0; // dizi uzunluğunu hesaplamak için kullanılacak;
    int t_year; //yıl tablosunun indeksi
    struct HouseData *tmp_current;
    struct HouseData *tmp_old;


    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    tmp_current = my_list->head;
    tmp_current->next = NULL;
    while (fgets(str, 50, fp)) {

        if (count == 0) { //ilk satırı kayıda almaz //head struct yapısını kurar
            count += 1;
            continue;
        }
        tmp_old = tmp_current;
        if (count > 1) { //ikinci döngüde tmp_node head yapısını doldurur
            tmp_current = malloc(sizeof(struct HouseData));
            if (tmp_current == NULL) {
                printf("memory error!!");
                return;
            }
            tmp_current->next = NULL;
            // bu kısıma kadar bir önceki ve şu anki değiskeni sakladık tarama işeleminden sonra nesnenin id' numarasına göre ekleme yapılacak.
        }

        // data set üzerindeki tokenleri bulup sonrasını tmp stringine aktarır7
        //veri yapısına göre tmp degiskene atar;
        char tmp[20];
        int item_counter = 0;
        int y = 0;
        for (int i = 0; i < strlen(str); i++) {
            if (str[i] == ',' || str[i] == '\n') {
                tmp[y] = '\0';
                switch (item_counter) {
                    case 0:
                        y = 0;
                        tmp_current->id = atoi(tmp);
//                        printf("%d\n",tmp_current->id);
                        break;
                    case 1:
                        y = 0;
                        tmp_current->lotArea = atoi(tmp);
//                        printf("%d\n",tmp_node->lotArea);
                        break;
                    case 2:
                        y = 0;
                        tmp_current->neighborhood_id = hash_compute(tmp);
//                        struct Neighborhood *stl =  hash_lookup(Neighborhood_list,tmp);
//                        printf("%s\n",stl->titre);
                        hash_lookup(my_list->N_list, tmp);
                        break;
                    case 3:
                        y = 0;
                        tmp_current->yearBuilt = atoi(tmp);
                        t_year = tmp_current->yearBuilt - MINYEAR;
                        if (my_list->B_list[t_year] == NULL) {

                            my_list->B_list[t_year] = malloc(sizeof(struct BuiltYear));
                            if (my_list->B_list[t_year] == NULL) {
                                printf("Memory error!!\n");
                                return;
                            }
                            my_list->B_list[t_year]->year = tmp_current->yearBuilt;
                            my_list->B_list[t_year]->count = 0;
                            my_list->B_list[t_year]->price = 0;
                        }
//                        printf("%s\n",tmp);
                        break;
                    case 4:
                        y = 0;
                        tmp_current->salePrice = atoi(tmp);
//                        printf("%s\n",tmp);
                        break;
                }
                item_counter++;
            } else {
                tmp[y] = str[i];
                y++;
            }
        }
        if(item_counter == 3){
            tmp_current->salePrice = 0;
        }
        //node değişim işlemi gerçekleşmeden önce konum fiyat bilgileri saklanır
        my_list->N_list[tmp_current->neighborhood_id]->sum_price += tmp_current->salePrice;
        my_list->N_list[tmp_current->neighborhood_id]->count++;
        my_list->B_list[t_year]->price += tmp_current->salePrice;
        my_list->B_list[t_year]->count++;
        if (count > 1) {

            if (tmp_current->id >= tmp_old->id) {
                tmp_old->next = tmp_current;
            } else {
                tmp_current->next = tmp_old;
                tmp_old = tmp_current;
            }
        }
        count += 1;
    }

    my_list->count = count-1; //header çıkarılır
    fclose(fp);
}

void cleaner(struct HouseList *data) {
    struct HouseData *tmp_node;
    struct HouseData *tmp_dlt;
    tmp_node = data->head;

    while (tmp_node->next != NULL) {
        tmp_dlt = tmp_node;
        tmp_node = tmp_node->next;
        free(tmp_dlt);
    }

    clean_neighborlist(data->N_list);
    clean_yearlist(data->B_list);
}

void clean_neighborlist(struct Neighborhood **Neighborhood_list) {
    for (int i = 0; i < 100; i++) {
        free(Neighborhood_list[i]);
    }
}

void clean_yearlist(struct BuiltYear **Year_list) {
    for (int i = 0; i < YEARBUCKETS; i++) {
        free(Year_list[i]);
    }
}

void write_neighborhoods(struct Neighborhood **Neighborhood_list) {
    for (int i = 0; i < MAXBUCKETS; i++) {
        if (Neighborhood_list[i] != NULL) {
            printf("%s\n", Neighborhood_list[i]->titre);
        }
    }
}

void write_HouseData(struct HouseList list) {
    struct HouseData *tmp_data;
    tmp_data = list.head;
    while (tmp_data != NULL) {
        printf("\n HouseID: %d\n", tmp_data->id);
        printf("\t\t Lot Area: %d\n", tmp_data->lotArea);
        printf("\t\t Neighborhood: %s\n", list.N_list[tmp_data->neighborhood_id]->titre);
        printf("\t\t YearBuilt: %d\n", tmp_data->yearBuilt);
        printf("\t\t SalePrice: %d\n", tmp_data->salePrice);
        tmp_data = tmp_data->next;
    }
}

int count_houses(struct HouseList list) {
    return list.count;
}


int mean_sale_prices(struct HouseList list) {
    struct HouseData *tmp = list.head;
    unsigned int sum = 0;
    int mean = 0;
    while (tmp != NULL) {
        sum += tmp->salePrice;
        tmp = tmp->next;
    }
    mean = sum / count_houses(list);
    return mean;
}

int mean_saleprices_neighborhood(struct HouseList list, const char *str) {
    int hash = hash_compute(str);
    int mean = list.N_list[hash]->sum_price / list.N_list[hash]->count;
    return mean;
}

void mean_saleprices_neighborhood_list(struct HouseList *list) {
    int mean;
    int echelle;
    for (int i = 0; i < MAXBUCKETS; i++) {
        if (list->N_list[i] != NULL) {
            mean = mean_saleprices_neighborhood(*list, list->N_list[i]->titre);
            printf("%s: %d -- %d House", list->N_list[i]->titre, mean, list->N_list[i]->count);
            echelle = mean / 30000;
            for (int y = 0; y < echelle; y++) {
                printf("\t");
            }
            printf("===>%d\n", echelle);
        }
    }
}

int mean_year(struct HouseList *list, int year) {
    if (list->B_list[year - MINYEAR] == NULL) {
        printf("Houses not found for %d\n", year);
        return 0;
    }
    int mean = list->B_list[year - MINYEAR]->price / list->B_list[year - MINYEAR]->count;
    return mean;
}

void mean_saleprices_year(struct HouseList *list) {
    int mean;
    int echelle;
    for (int i = 0; i < YEARBUCKETS; i++) {
        if (list->B_list[i] != NULL) {
            mean = mean_year(list, list->B_list[i]->year);
            printf("%d: %d -- %d House", list->B_list[i]->year, mean, list->B_list[i]->count);
            echelle = mean / 30000;
            for (int y = 0; y < echelle; y++) {
                printf("\t");
            }
            printf("===>%d\n", echelle);
        }
    }
}

void min_max(struct HouseList *list) {
    struct HouseData *H_max;
    struct HouseData *H_min;
    struct HouseData *tmp = list->head;
    int min = tmp->salePrice, max = tmp->salePrice;
    int min_id, max_id;
    while (tmp != NULL) {
        if (tmp->salePrice <= min) {
            min = tmp->salePrice;
            min_id = tmp->id;
        }
        if (tmp->salePrice >= max) {
            max = tmp->salePrice;
            max_id = tmp->id;
        }
        tmp = tmp->next;
    }
    H_max = findByID(max_id,list);
    H_min = findByID(min_id,list);
    printf("Max price: %d\n\tid:%d\n\tlocation:%s\n\tbuiltyear:%d\n\tlotarea:%d\n\n",max,max_id,list->N_list[H_max->neighborhood_id]->titre,H_max->yearBuilt,H_max->lotArea);
    printf("Max price: %d\n\tid:%d\n\tlocation:%s\n\tbuiltyear:%d\n\tlotarea:%d\n\n",min,min_id,list->N_list[H_min->neighborhood_id]->titre,H_min->yearBuilt,H_min->lotArea);
}

struct HouseData *findByID(int id, struct HouseList *list) {
    struct HouseData *tmp = list->head;
    while (tmp != NULL) {
        if (tmp->id == id) {
            return tmp;
        }
        tmp = tmp->next;
    }
}