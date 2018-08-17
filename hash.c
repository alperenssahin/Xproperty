#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include"hash.h"
#include "dataset.h"

#define MULTIPLIER 31

/* Stringler icin basit bir hash hesaplama fonksiyonu.
 * Her harfin ASCII degeri, onceki hash degerinin bir
 * carpanina ekleniyor.
 * Ornegin: "abcde" stringi icin:
 * ascii degerleri : a=97, b=98,c=99,d=100,e=101 
 *  h0 = 0 * 31 + 97 = 97
 *  h1 = h0 * 31 + 98 = 3105
 *  h2 = h1 * 31 + 99 = 96354
 *  h3 = h2 * 31 + 100 = 2987074
 *  h4 = h3 * 31 + 101 = 92599395
 *  92599395 % MAXBUCKETS (1000) = 395
 */
unsigned int hash_compute(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * MULTIPLIER) + *str;
        ++str;

    }
    return hash % MAXBUCKETS;
}


struct Neighborhood *hash_lookup(struct Neighborhood **table, const char *str) {
    struct Neighborhood **tmp_list = table;
    unsigned int hash = hash_compute(str);
    if (tmp_list[hash] == NULL) {
//        strcpy(table[hash]->titre,str);
        tmp_list[hash] = malloc(sizeof(struct Neighborhood));
        if (tmp_list[hash] == NULL) {
            printf("memoryerror!!");
            return NULL;
        }
        tmp_list[hash]->titre = strdup(str);
        tmp_list[hash]->sum_price = 0;
        tmp_list[hash]->count=0;
        return table[hash];
    } else {
        return table[hash];
    }
}





