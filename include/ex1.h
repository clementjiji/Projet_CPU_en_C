#ifndef ex1
#define ex1
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#define TABLESIZE 12
#define TOMBSTONE (( void *) -1)

typedef struct hashEntry {
	char * key ;
	void * value ;
} HashEntry ;

typedef struct hashmap {
	int size ;
	HashEntry * table ;
} HashMap ;


unsigned long simple_hash(const char *str);
HashMap *hashmap_create();
int hashmap_insert(HashMap *map, const char *key, void *value);
void *hashmap_get(HashMap *map, const char *key);
int hashmap_remove(HashMap *map, const char *clé);
void hashmap_destroy(HashMap *map);
void afficher(HashMap *map);
void afficher_val_seg(HashMap *map);



#endif 
