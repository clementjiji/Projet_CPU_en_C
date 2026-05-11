#include "ex7.h"

int push_value(CPU *cpu, int value){
    // Récupérer le pointeur de pile (SP) dans le contexte du CPU
    int *sp = hashmap_get(cpu->context,"SP");

    // Vérifier si le pointeur SP est valide
    if(sp == NULL){
        printf("Erreur : SP non trouvé\n");
        return -1;  // Retourner une erreur si SP est NULL
    }

    // Récupérer le segment de la pile (SS) alloué dans la mémoire
    Segment *ss = hashmap_get(cpu->memory_handler->allocated,"SS");

    // Vérifier si le segment de la pile (SS) existe
    if(ss == NULL){
        printf("Erreur : SS est NULL\n");
        return -1;  // Retourner une erreur si SS est NULL
    }

    // Vérifier si le SP est à une position valide dans la pile
    // Si le SP est inférieur ou égal au début du segment de pile, la pile est pleine
    if(*sp <= ss->start){
        printf("Valeur de SP : %d\n",*sp);
        return -1;  // Retourner une erreur si la pile est pleine
    }

    // Décrémenter le SP pour le déplacer vers le bas de la pile
    *sp -= 1;

    // Calculer la position dans le segment de pile en fonction de l'adresse de début du segment
    int pos = *sp - ss->start;
    
    // Allouer de la mémoire pour stocker la valeur à empiler
    int *val = malloc(sizeof(int));
    *val = value;  // Stocker la valeur dans la mémoire allouée

    // Stocker la valeur à la position calculée dans le segment de pile (SS)
    store(cpu->memory_handler,"SS",pos,val);
    
    return 0;  // Retourner 0 pour indiquer que l'opération a réussi
}

int pop_value(CPU *cpu, int *dest){
    // Récupérer le pointeur de pile (SP) dans le contexte du CPU
    int *sp = hashmap_get(cpu->context,"SP");

    // Vérifier si le pointeur SP est valide
    if(sp == NULL){
        printf("Erreur : SP NULL\n");
    }

    // Récupérer le segment de la pile (SS) alloué dans la mémoire
    Segment *ss = hashmap_get(cpu->memory_handler->allocated,"SS");

    // Vérifier si le segment de la pile (SS) existe
    if(ss == NULL){
        printf("Erreur : SS NULL\n");
        return -1;  // Retourner une erreur si SS est NULL
    }

    // Vérifier si le SP dépasse la taille du segment de pile
    // Si le SP est supérieur ou égal à la fin du segment, la pile est vide
    if(*sp >= (ss->start + ss->size)){
        printf("Valeur de SP : %d\n",*sp);
        printf("Valeur de start+size : %d\n",(ss->start + ss->size));
        printf("Erreur : la pile est vide\n");
        return -1;  // Retourner une erreur si la pile est vide
    }

    // Calculer la position dans le segment de pile en fonction de l'adresse de début du segment
    int pos = *sp - ss->start;

    // Charger la valeur à la position calculée dans le segment de pile
    int *val = load(cpu->memory_handler, "SS", pos);

    // Incrémenter le SP pour le déplacer vers le haut de la pile
    *sp += 1;

    // Vérifier si la valeur a été correctement chargée
    if(!val){
        printf("Erreur de load\n");
        return -1;  // Retourner une erreur si la valeur n'a pas pu être chargée
    }

    // Stocker la valeur récupérée dans la variable destination (dest)
    *dest = *val;

    return 0;  // Retourner 0 pour indiquer que l'opération a réussi
}

