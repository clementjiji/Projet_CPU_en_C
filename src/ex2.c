#include "ex2.h"


// Fonction d'initialisation du gestionnaire de mémoire
MemoryHandler *memory_init(int size) {
    // Alloue de la mémoire pour un MemoryHandler
    MemoryHandler *m = malloc(sizeof(MemoryHandler));
    // Alloue de la mémoire pour le tableau de mémoire
    m->memory = (void**)malloc(sizeof(void*)*size);
    int i = 0;
    // Initialise toutes les positions du tableau à NULL 
    while (i < size) {
        m->memory[i] = NULL;
        i++;
    }
    // Stocke la taille totale de la mémoire
    m->total_size = size;
    // Alloue un segment représentant toute la mémoire disponible
    Segment *s = malloc(sizeof(Segment));
    s->start = 0;
    s->size = size;
    s->next = NULL;
    // Initialise la liste des segments libres
    m->free_list = s;
    // Crée une table de hachage pour les segments alloués
    m->allocated = hashmap_create();
    return m;
}


// Retourne le segment libre et met à jour 'prev' pour contenir le segment précédent
Segment *find_free_segment(MemoryHandler* handler, int start, int size, Segment** prev) {
    if (!handler) {
        return NULL;
    }
    // Récupère le premier segment libre
    Segment *s = handler->free_list;
    if (s == NULL) {
        return NULL;  // Aucun segment libre trouvé
    }
    Segment *p = *prev;
    p = NULL;
    *prev = NULL;
    // Parcours la liste des segments libres
    while (s) {
        // Si le segment libre peut contenir le segment demandé
        if (s->start <= start && (s->start + s->size) >= start + size) {
            *prev = p;
            return s;  // Retourne le segment libre trouvé
        }
        p = s;
        s = s->next;
    }
    return NULL;
}

// Fonction pour créer un nouveau segment alloué dans la mémoire à partir de 'start' avec la taille 'size'
int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    Segment *prev = NULL;
    Segment *new_s = NULL;
    Segment *new_s2 = NULL;
    Segment *s = find_free_segment(handler, start, size, &prev);
    if (s == NULL) {
        return 0;  
    }

    // Crée un nouveau segment alloué
    Segment *new_seg = malloc(sizeof(Segment));
    if (!new_seg) {
        printf("Erreur d'allocation mémoire pour le segment.\n");
        return 0;
    }

    // Initialise le nouveau segment alloué
    new_seg->start = start;
    new_seg->size = size;
    new_seg->next = NULL;
    printf("Dans create segment on créee un nouveau segment :");
    afficher_seg(new_seg);
    
    // Insère le segment alloué dans la table de hachage
    int ret = hashmap_insert(handler->allocated, name, new_seg);
    if (ret == 0) {
        return 0;  // Échec de l'insertion dans la table de hachage
    }
    
    // Crée de nouveaux segments libres après l'allocation (si applicable)
    if ((start - s->start) > 0) {
        new_s = malloc(sizeof(Segment));
        if (!new_s) {
            printf("Erreur d'allocation mémoire pour le segment libre.\n");
            return 0;
        }
        new_s->start = s->start;
        new_s->size = (start - s->start);  // Taille du segment libre avant l'allocation
    }

    if ((s->size + s->start) - (start + size) > 0) {
        new_s2 = malloc(sizeof(Segment));
        if (!new_s2) {
            printf("Erreur d'allocation mémoire pour le segment libre.\n");
            return 0;
        }
        new_s2->start = (start + size);
        new_s2->size = (s->size + s->start) - (start + size);  // Taille du segment libre après l'allocation
    }

    // Supprime l'ancien segment libre de la liste
    if (prev != NULL) {
        prev->next = s->next;
    } else {
        handler->free_list = s->next;
    }
    free(s);

    // Ajoute les nouveaux segments libres à la liste des segments libres
    if (new_s2 != NULL) {
        new_s2->next = handler->free_list;
        handler->free_list = new_s2;
    }
    if (new_s != NULL) {
        new_s->next = handler->free_list;
        handler->free_list = new_s;
    }
    return 1;  // Création du segment réussie
}

// Fonction pour supprimer un segment alloué de la mémoire
int remove_segment(MemoryHandler *handler, const char *name) {
    Segment *segAliberer = (Segment*)hashmap_get(handler->allocated, name);
    if (segAliberer == NULL) {
        printf("Problème de segment à libérer.\n");
        return 0;  // Le segment à libérer n'a pas été trouvé
    }

    // Recherche le segment libre correspondant
    Segment *segfree = handler->free_list;
    Segment *debut = NULL;
    if (segfree == NULL) {
        printf("Liste des segments libres vide.\n");
        return 0;
    }

    // Parcours la liste des segments libres pour fusionner le segment libéré avec les segments adjacents
    while (segfree) {
        if ((segfree->start + segfree->size) == segAliberer->start) {
            segfree->size += segAliberer->size;  // Fusionne avec le segment libre précédent
            debut = segfree;
        }
        if (segfree->start == (segAliberer->start + segAliberer->size)) {
            if (debut) {
                debut->size += segfree->size;  // Fusionne avec le segment libre suivant
                debut->next = segfree->next;
            } else {
                segfree->start = segAliberer->start;
                segfree->size += segAliberer->size;
            }
        }
        segfree = segfree->next;
    }
    
    // Retire le segment alloué de la table de hachage
    hashmap_remove(handler->allocated, name);
    return 1;  // Suppression du segment réussie
}

// Fonction pour afficher un segment (son début et sa taille)
void afficher_seg(Segment *seg) {
    printf("(%d---%d)", seg->start, seg->size);
}

// Fonction pour afficher la liste des segments libres
void afficher_free_list(MemoryHandler *handler) {
    printf("\n");
    if (handler == NULL || handler->free_list == NULL) {
        printf("La liste des segments libres est vide ou le gestionnaire de mémoire est NULL.\n");
        return;
    }

    Segment *current = handler->free_list;
    printf("\n=== Liste des segments libres ===\n");

    // Affiche chaque segment libre
    while (current != NULL) {
        afficher_seg(current);
        printf("\n");
        current = current->next;
    }
    printf("\n");
}

// Fonction pour afficher le contenu de la mémoire sous forme textuelle
void affichage_mem_char(void **memory, int taille) {
    printf("\nAffichage du contenu de la mémoire (format texte):\n");

    if (taille == 0 || memory == NULL) {
        printf("Mémoire vide ou non initialisée.\n");
        return;
    }

    // Affiche chaque élément non NULL dans la mémoire
    for (int i = 0; i < taille; i++) {
        if (memory[i] != NULL) {
            printf("À l’indice %d : \"%s\"\n", i, (char*)memory[i]);
        }
    }
}

// Fonction pour détruire un gestionnaire de mémoire
void destroy_handler(MemoryHandler *handler) {
    // Libère la mémoire allouée au gestionnaire
    if (handler == NULL)
        return;

    // Libère la liste des segments libres
    Segment *courant = handler->free_list;
    while (courant) {
        Segment *next = courant->next;
        free(courant);
        courant = next;
    }

    // Libère la mémoire du tableau 'memory'
    if (handler->memory) {
        free(handler->memory);
    }

    // Détruit la table de hachage des segments alloués
    hashmap_destroy(handler->allocated);
    free(handler);  // Libère le gestionnaire de mémoire
}










