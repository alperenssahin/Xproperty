#ifndef  _HASH_H

#define  _HASH_H

#define  MAXBUCKETS  100


unsigned  int  hash_compute(const  char *str);

struct Neighborhood *  hash_lookup(struct Neighborhood *table[], const char *str);


#endif
