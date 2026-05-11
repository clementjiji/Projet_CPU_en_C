#include "ex4.h"


// Initialisation du CPU avec un gestionnaire de mémoire et une configuration de contexte.
CPU *cpu_init(int memory_size) {
    // Allocation de mémoire pour le processeur (CPU)
    CPU *proc = malloc(sizeof(CPU));
    
    // Initialisation du gestionnaire de mémoire du CPU
    proc->memory_handler = memory_init(memory_size);
    
    // Création des structures de données pour le contexte (registre, pile, etc.)
    proc->context = hashmap_create();
    proc->constant_pool = hashmap_create();

    // Allocation de mémoire pour les registres du CPU
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    int *d = malloc(sizeof(int));
    int *ip = malloc(sizeof(int));
    int *zf = malloc(sizeof(int));
    int *sf = malloc(sizeof(int));
    int *sp = malloc(sizeof(int));
    int *bp = malloc(sizeof(int));
    int *es = malloc(sizeof(int));
    
    // Initialisation des registres avec des valeurs par défaut
    *a = 0, *b=0, *c=0, *d=0;
    hashmap_insert(proc->context, "AX", a);
    hashmap_insert(proc->context, "BX", b);
    hashmap_insert(proc->context, "CX", c);
    hashmap_insert(proc->context, "DX", d);

    // Initialisation des registres de flags et de pointeurs
    *ip = 0, *zf = 0, *sf = 0;
    hashmap_insert(proc->context, "IP", ip);
    hashmap_insert(proc->context, "ZF", zf);
    hashmap_insert(proc->context, "SF", sf);

    // Allocation pour la pile (stack) et base pointer
    int ss_size = 128;
    int ss_start = memory_size - ss_size;
    *sp = ss_start + ss_size, *bp = ss_start + ss_size;
    hashmap_insert(proc->context, "SP", sp);
    hashmap_insert(proc->context, "BP", bp);

    // Création du segment de pile dans la mémoire
    create_segment(proc->memory_handler, "SS", ss_start, ss_size);

    // Initialisation du registre "ES" avec une valeur par défaut
    *es = -1;
    hashmap_insert(proc->context, "ES", es);

    return proc;
}

// Fonction de destruction du CPU et nettoyage des ressources allouées.
void cpu_destroy(CPU *cpu) {
    // Si le CPU est NULL, on ne fait rien
    if (cpu == NULL) {
        return;
    }

    // Destruction du contexte et du pool constant
    if (cpu->context != NULL) {
        hashmap_destroy(cpu->context);
    }
    if (cpu->constant_pool != NULL) {
        hashmap_destroy(cpu->constant_pool);
    }

    // Libération du gestionnaire de mémoire et nettoyage de la mémoire allouée
    if (cpu->memory_handler != NULL) {
        if (cpu->memory_handler->memory != NULL) {
            void **memoire = cpu->memory_handler->memory;
            for (int i = 0; i < cpu->memory_handler->total_size; i++) {
                if (memoire[i] != NULL) {
                    free(memoire[i]);
                }
            }
            free(memoire);
        }

        // Libération de la liste des segments libres
        if (cpu->memory_handler->free_list != NULL) {
            Segment *liberation = cpu->memory_handler->free_list;
            Segment *tmp = NULL;
            while (liberation) {
                tmp = liberation->next;
                free(liberation);
                liberation = tmp;
            }
        }

        // Destruction de la table de hachage des segments alloués
        if (cpu->memory_handler->allocated != NULL) {
            hashmap_destroy(cpu->memory_handler->allocated);
        }

        // Libération du gestionnaire de mémoire
        free(cpu->memory_handler);
    }

    // Libération du CPU
    free(cpu);
}

// Fonction pour stocker des données à une position donnée dans un segment spécifique.
void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data) {
    // Recherche du segment dans la table des segments alloués
    Segment *seg = (Segment*)hashmap_get(handler->allocated, segment_name);
    
    // Si la position est déjà occupée, on libère la mémoire précédemment allouée
    if (handler->memory[seg->start + pos] != NULL) {
        free(handler->memory[seg->start + pos]);
    }
    
    // Si le segment existe et que la position est valide, on y stocke les données
    if (seg && pos < seg->size) {
        handler->memory[seg->start + pos] = data;
        return data;
    }
    
    return NULL;
}

// Fonction pour charger des données depuis un segment spécifique à une position donnée.
void* load(MemoryHandler *handler, const char *segment_name, int pos) {
    // Vérification des erreurs : gestionnaire de mémoire et mémoire non initialisés
    if (handler == NULL || handler->memory == NULL) {
        printf("Erreur : gestionnaire de mémoire ou mémoire non initialisée.\n");
        return NULL;
    }

    // Recherche du segment dans la table des segments alloués
    Segment *seg = (Segment*)hashmap_get(handler->allocated, segment_name);
    if (seg == NULL) {
        printf("Erreur : segment '%s' introuvable.\n", segment_name);
        return NULL;
    }

    // Vérification de la validité de la position dans le segment
    if (pos < 0 || pos >= seg->size) {
        return NULL;
    }

    // Retourne la donnée à la position donnée dans le segment
    void **mem = handler->memory;
    void *result = mem[seg->start + pos];
    return result;
}

// Fonction pour allouer des variables dans le segment de données (DS).
void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count) {
    // Vérification de la validité du tableau d'instructions
    if (data_instructions == NULL) {
        printf("Erreur : tableau des instructions est vide.\n");
        return;
    }
    if (data_count <= 0) {
        printf("Erreur : taille insuffisante.\n");
        return;
    }

    int iter = 0;
    int tot = 0;
    int i;

    // Parcours des instructions et calcul du nombre total d'éléments à allouer
    while (iter < data_count) {
        char *l = strdup(data_instructions[iter]->operand2);
        const char *n;
        i = 1;
        
        strtok(l, ",");
        n = strtok(NULL, ","); 
        
        if (n != NULL) {
            i++;
            while (n) {
                n = strtok(NULL, ","); 
                i++;
            }
        }
        free(l);
        tot += i; 
        iter++;
    }
    printf("Nombre total d'éléments dans les opérandes : %d\n", tot);
    
    // Création du segment "DS" (Data Segment) avec le nombre total d'éléments
    int verif = create_segment(cpu->memory_handler, "DS", cpu->memory_handler->free_list->start, tot);
    if (verif == 0) {
        printf("Erreur lors de la création du segment 'DS'.\n");
        return;
    }

    // Allocation des variables dans le segment de données
    i = 0;
    iter = 0; 
    while (iter < data_count && i < tot) {
        char *m = strdup(data_instructions[iter]->operand2);
        const char *n;
        
        // Si la variable a des valeurs séparées par des virgules
        if (strchr(m, ',')) {
            n = strtok(m, ",");
            while (n) {
                int *new_v = malloc(sizeof(int));
                *new_v = atoi(n);
                store(cpu->memory_handler, "DS", i, new_v);
                i++;
                n = strtok(NULL, ",");
            }
        } else {
            int *new_v = malloc(sizeof(int));
            *new_v = atoi(m);
            store(cpu->memory_handler, "DS", i, new_v);
            i++;
        }
        iter++;
        free(m);
    }
}

// Fonction pour afficher le contenu du segment de données "DS".
void print_data_segment(CPU *cpu) {
    printf("La mémoire est de taille : %d\n", cpu->memory_handler->total_size);

    if (cpu->memory_handler->total_size == 0) {
        printf("La mémoire est vide.\n");
    }

    printf("Affichage des éléments dans la mémoire :\n");
    
    // Vérification de l'existence du segment "DS"
    Segment *seg = (Segment*)hashmap_get(cpu->memory_handler->allocated, "DS");
    if (seg == NULL) {
        printf("Le segment '%s' n'est pas créé.\n", "DS");
        return;
    }

    // Affichage des valeurs dans le segment "DS"
    for (int i = 0; i < seg->size; ++i) {
        void *val = load(cpu->memory_handler, "DS", i);
        if (val != NULL) {
            printf("Élément à l'indice %d : %d\n", i, *(int*)val);
        }
    }
}

