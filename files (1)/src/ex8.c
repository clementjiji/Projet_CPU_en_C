#include "ex8.h"
// Fonction pour résoudre l'adressage des segments avec des registres spécifiques
void* segment_override_addressing(CPU* cpu, const char* operand){
    // Définir une expression régulière pour identifier le format de l'operand
    const char* reg = "^\\[(DS|CS|SS|ES):(AX|BX|CX|DX)\\]$";

    // Vérifier si l'operand correspond à l'expression régulière
    if(matches(reg,operand) == 0){
        printf("Erreur : l'opérande n'est pas valide\n");
        return NULL;  // Retourner NULL si l'operand ne correspond pas au format
    }

    // Déclarer des variables pour extraire le segment et le registre de l'operand
    char segment[3];
    char registre[3];
    
    // Extraire le segment et le registre à partir de l'operand
    sscanf(operand, "[%2[^:]:%2[^]]]", segment, registre);

    // Chercher le segment dans la mémoire allouée
    Segment* seg = (Segment*)hashmap_get(cpu->memory_handler->allocated, segment);

    // Vérifier si le segment existe
    if(seg == NULL){
        printf("Erreur : segment non trouvé\n");
        return NULL;  // Retourner NULL si le segment n'existe pas
    }

    // Récupérer la valeur du registre dans le contexte du CPU
    int* reval = (int*)hashmap_get(cpu->context, registre);

    // Retourner la valeur chargée à partir de l'adresse du segment et de la valeur du registre
    return load(cpu->memory_handler, segment, *reval);
}

// Fonction pour trouver une adresse libre dans la mémoire selon la stratégie choisie
int find_free_address_strategy(MemoryHandler *handler, int size, int strategy) {
    // Vérifier que le gestionnaire de mémoire et la taille sont valides
    if (!handler || size <= 0) return -1;

    Segment *tmp = handler->free_list;

    // Stratégie "First Fit" : retourner la première adresse suffisamment grande
    if (strategy == 0){
        printf("First Fit\n");  
        while (tmp) {
            if (tmp->size >= size) {
                return tmp->start;  // Retourner l'adresse du premier segment adapté
            }
            tmp = tmp->next;
        }
        return -1;  // Retourner -1 si aucune adresse libre n'est trouvée
    }

    // Stratégie "Best Fit" : retourner l'adresse avec la plus petite différence de taille
    else if (strategy == 1) {  
        printf("Best FIT\n");
        int min_diff =  handler->total_size + 1; 
        int best_start = -1;

        while (tmp) {
            if (tmp->size >= size) {
                int diff = tmp->size - size;
                if (diff < min_diff) {
                    min_diff = diff;
                    best_start = tmp->start;  // Mettre à jour l'adresse avec la meilleure différence
                }
            }
            tmp = tmp->next;
        }
        return best_start;  // Retourner l'adresse du meilleur segment
    }

    // Stratégie "Worst Fit" : retourner l'adresse avec la plus grande différence de taille
    else if (strategy == 2) {  
        printf("Worst FIT\n");
        int max_diff = -1;
        int best_start = -1;

        while (tmp) {
            if (tmp->size >= size) {
                int diff = tmp->size - size;
                if (diff > max_diff) {
                    max_diff = diff;
                    best_start = tmp->start;  // Mettre à jour l'adresse avec la plus grande différence
                }
            }
            tmp = tmp->next;
        }
        return best_start;  // Retourner l'adresse du plus grand segment
    }

    return -1;  // Retourner -1 si aucune stratégie ne donne de résultat
}

// Fonction pour allouer un segment "ES" dans la mémoire
int alloc_es_segment(CPU *cpu){
    // Récupérer les valeurs nécessaires dans le contexte du CPU
    int *zfval = (int*)hashmap_get(cpu->context,"ZF");
    int *esval = (int*)hashmap_get(cpu->context,"ES");
    int *axval = (int*)hashmap_get(cpu->context,"AX");
    int *bxval = (int*)hashmap_get(cpu->context,"BX");

    // Vérifier que toutes les valeurs sont valides
    if(zfval == NULL || esval == NULL || axval == NULL || bxval == NULL){
        printf("Erreur : des valeurs dans le contexte sont nulles\n");
        return 0;  // Retourner 0 si une valeur est manquante
    }

    printf("La taille est de : %d et la stratégie : %d\n", *axval, *bxval);

    // Trouver une adresse libre selon la stratégie
    int freeseg = find_free_address_strategy(cpu->memory_handler, *axval, *bxval);
    if(freeseg == -1){
        printf("Erreur : échec de l'allocation\n");
        *zfval = 1;  // Indiquer une erreur dans le flag ZF
        return 0;  // Retourner 0 en cas d'échec
    }

    // Créer un segment à l'adresse trouvée
    int creeseg = create_segment(cpu->memory_handler, "ES", freeseg, *axval);
    if(creeseg == 0){
        printf("Erreur : échec de la création du segment\n");
        *zfval = 1;
        return 0;  // Retourner 0 si la création échoue
    }

    // Initialiser le segment avec des valeurs nulles
    for (int i = 0; i < *axval; i++) {
        int *value = (int *)malloc(sizeof(int));
        if (!value) {
            printf("Échec malloc à l'indice %d.\n", i);
            *zfval = 1;
            return 0;  // Retourner 0 si un échec de malloc se produit
        }
        *value = 0;
        store(cpu->memory_handler, "ES", i, (void *)value);  // Stocker la valeur dans le segment
    }

    *esval = freeseg;  // Mettre à jour l'adresse du segment ES
    *zfval = 0;  // Indiquer que l'allocation a réussi
    return 1;  // Retourner 1 pour indiquer le succès
}

// Fonction pour libérer le segment "ES" de la mémoire
int free_es_segment(CPU *cpu){
    // Récupérer la valeur du segment ES dans le contexte du CPU
    int *esval = (int*)hashmap_get(cpu->context,"ES");
    if(esval == NULL){
        printf("Erreur : ES est null dans le contexte du CPU\n");
        return 0;  // Retourner 0 si le segment ES n'est pas trouvé
    }

    // Récupérer le segment ES de la mémoire allouée
    Segment *esseg = (Segment*)hashmap_get(cpu->memory_handler->allocated,"ES");
    if(esseg == NULL){
        printf("Erreur : ES n'est pas dans les segments alloués\n");
        return 0;  // Retourner 0 si le segment ES n'est pas trouvé
    }

    // Libérer toutes les valeurs du segment ES
    for (int i = 0; i < esseg->size; i++) {
        void *value = load(cpu->memory_handler, "ES", i);
        if (value != NULL) {
            store(cpu->memory_handler, "ES", i, NULL);  // Libérer la valeur à la position donnée
        }
    }

    // Supprimer le segment ES
    int rem = remove_segment(cpu->memory_handler, "ES");
    if(rem == 0){
        printf("Erreur : échec de la suppression du segment (code: %d)\n", rem);
        return 0;  // Retourner 0 si la suppression échoue
    }

    *esval = -1;  // Réinitialiser l'adresse du segment ES dans le contexte
    return 1;  // Retourner 1 pour indiquer que la suppression a réussi
}

