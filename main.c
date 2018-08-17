#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include"dataset.h"
//#include "hash.h"
#include "models.h"
#include "strassen.h"

int main() {

    //VERİ YAPISI KURULUMU//
    struct HouseList My_list;

    memset(My_list.N_list, 0,
           sizeof(My_list.N_list)); //Neigborhood hash datasını tutmak için gerekli liste için memory işlemleri

    memset(My_list.B_list, 0, sizeof(My_list.B_list));

    struct HouseData *head = malloc(sizeof(struct HouseData));
    if (head == NULL) {
        printf("Memory error");
        return 0;
    }
    My_list.head = head;
    My_list.count = 0;
    read_dataset_file("./house_price_data.csv",
                      &My_list); //data seti okuma komutu, yeni locationları listeye kaydeder;




    struct HouseData *head_test = malloc(sizeof(struct HouseData));
    if (head_test == NULL) {
        printf("Memory error");
        return 0;
    }
    struct HouseList Test_List;
    Test_List.head = head_test;
    Test_List.count = 0;

    memset(Test_List.N_list, 0,
           sizeof(My_list.N_list)); //Neigborhood hash datasını tutmak için gerekli liste için memory işlemleri

    memset(Test_List.B_list, 0, sizeof(My_list.B_list));

    read_dataset_file("./house_price_test.csv", &Test_List);
    srand(time(NULL));
    //FONKSYON TESTLERİ//


    //    write_neighborhoods(neighborhood_list);
//        write_HouseData(My_list);
//        printf("countdata:%d",count_houses(My_list));
    //    printf("meandata:%d", mean_sale_prices(*My_list));
//        mean_saleprices_neighborhood_list(&My_list);
//        mean_saleprices_year(&My_list);
    //    min_max(&My_list);


    //TEST DATA SETİ FONKSYON TESTLERİ//
//    write_HouseData(Test_List);
//    model_by_neighborhood(Test_List,My_list);
//    save_price_list(Test_List);


    //MATRİSLER//
    struct Matris * W = calculate_parameter(My_list);
//    print_matrice(W);

    struct HouseData *tmp = Test_List.head;
    while(tmp != NULL ){
        double price = tmp->lotArea*W->MAT[1] + W->MAT[0];
        tmp->salePrice = price;
        tmp = tmp->next;
    }

    write_HouseData(Test_List);
    save_price_list(Test_List);

    cleaner(&My_list);//data için temizlik yapar
    cleaner(&Test_List);
    matris_cleaner(W);
//    matris_cleaner(M3);
    return 0;
}
