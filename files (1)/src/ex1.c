#include "ex1.h"

// Fonction de hachage simple qui calcule une valeur de hachage pour une clé
unsigned long simple_hash(const char *str){
    char lettre;
    unsigned long res = 0; 
    int i = 0;
    // On parcourt chaque caractère de la chaîne pour calculer la valeur de hachage
    while(str[i] != '\0'){
        lettre = str[i];  // Prend le caractère à la position i
        res += (unsigned long)lettre;  // Ajoute la valeur ASCII du caractère au résultat
        i++;
    }
    // Retourne le résultat du hachage modulo la taille de la table
    return res % TABLESIZE;
}

// Fonction de création d'une table de hachage
HashMap *hashmap_create(){
    HashMap *tab = malloc(sizeof(HashMap));  // Alloue de la mémoire pour la table
    if (!tab) return NULL; 

    // Alloue de la mémoire pour une liste de taille TABLESIZE, initialisée à zéro
    HashEntry *liste = (HashEntry *)calloc(TABLESIZE, sizeof(HashEntry));
    if (!liste) return NULL;  

    tab->table = liste;  // Assigne la table à la liste allouée
    tab->size = TABLESIZE;  // Définit la taille de la table

    return tab; 
}

// Fonction d'insertion d'une clé et valeur dans la table de hachage
int hashmap_insert(HashMap *map, const char *key, void *value){
    unsigned long res = simple_hash(key);  
    int cpt = 0;  // Compteur de collisions
    while (cpt < TABLESIZE){
        if (map->table[res].key == NULL || map->table[res].key == TOMBSTONE){
            // Alloue de la mémoire pour la clé et copie la clé dans la table
            map->table[res].key = strdup(key);
            map->table[res].value = value;  // Assigne la valeur à cet emplacement
            return 1;  
        }
        // Si la clé existe déjà, remplace la valeur associée
        if (strcmp(map->table[res].key, key) == 0) {
            free(map->table[res].value);  // Libère la valeur précédente
            map->table[res].value = value;  // Remplace par la nouvelle valeur
            return 1;
        }
        
        // Gère les collisions en utilisant un "linear probing" (probe linéaire)
        res = (res + 1) % TABLESIZE;
        cpt++; 
    }

    return 0;  // Si la table est pleine et qu'on n'a pas pu insérer la clé, retourne 0
}

// Fonction pour obtenir la valeur associée à une clé
void *hashmap_get(HashMap *map, const char *key){
    unsigned long res = simple_hash(key);  // Calcule la valeur de hachage pour la clé
    int cpt = 0;  
    while (cpt < TABLESIZE){
        // Si la clé est présente et qu'elle n'est pas tombée dans un "tombstone"
        if (map->table[res].key != NULL && map->table[res].key != TOMBSTONE){
            // Si la clé correspond, retourne la valeur associée
            if (strcmp(map->table[res].key, key) == 0) {
                return map->table[res].value;
            }
        }
        
        // Gère les collisions en utilisant un "linear probing"
        res = (res + 1) % TABLESIZE;
        cpt++;  
    }

    return NULL; 
}

// Fonction pour supprimer une entrée de la table de hachage en fonction de la clé
int hashmap_remove(HashMap *map, const char *key){
    unsigned long res = simple_hash(key);  // Calcule la valeur de hachage pour la clé
    int cpt = 0;  
    while (cpt < TABLESIZE){
        if (strcmp(map->table[res].key, key) == 0){
            free(map->table[res].key);// Libère la mémoire allouée pour la clé
            map->table[res].key = TOMBSTONE;  // Marque cet emplacement comme TOMBSTONE (supprimé)
            free(map->table[res].value);  // Libère la mémoire de la valeur
            map->table[res].value = NULL;  // Met la valeur à NULL
            return 1;  // Suppression réussie
        }

        // Gère les collisions avec "linear probing"
        res = (res + 1) % TABLESIZE;
        cpt++;  
    }

    return 0;  // Si la clé n'a pas été trouvée
}

// Fonction pour détruire la table de hachage et libérer toute la mémoire associée
void hashmap_destroy(HashMap *map){
    if (!map) return; 
    int cpt = 0;

    while (cpt < TABLESIZE){
        if (map->table[cpt].key != NULL && map->table[cpt].key != TOMBSTONE){
            // Libère la mémoire allouée pour la clé et la valeur
            free(map->table[cpt].key);
            free(map->table[cpt].value);
        }
        cpt++; 
    }
    free(map->table);  // Libère la mémoire allouée pour la table elle-même
    free(map);  // Libère la mémoire allouée pour la structure HashMap
}

// === Affichage de la table de hachage (affiche seulement les entiers) ===
void afficher(HashMap *map) {
    printf("\nContenu de la table de hachage :\n");
    if (!map || !map->table) {
        printf("La table est vide \n");
        return;  
    }

    // Parcourt chaque entrée de la table
    for (int i = 0; i < TABLESIZE; i++) {
        if (map->table[i].key != NULL && map->table[i].key != TOMBSTONE) {
            // Vérifie si la valeur est non NULL avant de l'afficher
            if (map->table[i].value != NULL) {
                printf("| %s -> %d |\n", map->table[i].key, *(int*)map->table[i].value);
            }
        }
    }

    printf("\n");
}

