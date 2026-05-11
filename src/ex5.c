#include "ex5.h"


// Fonction qui vérifie si une chaîne de caractères correspond à un motif (pattern) via une expression régulière
int matches(const char *pattern, const char *string) {
    regex_t regex;
    
    // Compilation de l'expression régulière
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        // Si la compilation échoue, afficher une erreur
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    
    // Exécution de l'expression régulière sur la chaîne donnée
    result = regexec(&regex, string, 0, NULL, 0);
    
    // Libération des ressources utilisées par l'expression régulière
    regfree(&regex);
    
    // Si la correspondance est trouvée, retourne 1, sinon 0
    return result == 0;
}

// Fonction pour gérer le mode d'adressage immédiat
void *immediate_addressing(CPU *cpu, const char *operand) {
    const char* reg = "^[0-9]+$"; // Expression régulière pour les nombres
    
    // Vérifie si l'opérande est un nombre
    if (matches(reg, operand) == 0) {
        return NULL; // Si ce n'est pas un nombre, retourne NULL
    }

    // Recherche si l'opérande existe déjà dans le pool constant
    void* vallu = hashmap_get(cpu->constant_pool, operand);

    if (vallu != NULL && vallu != TOMBSTONE) {
        return vallu; // Si trouvé, retourne la valeur correspondante
    }

    // Si l'opérande n'est pas dans le pool constant, on le convertit en entier et l'ajoute au pool
    int *val = malloc(sizeof(int));
    *val = atoi(operand);
    char *key_copy = strdup(operand);

    hashmap_insert(cpu->constant_pool, key_copy, val);
    free(key_copy);
    
    return val;
}

// Fonction pour gérer le mode d'adressage par registre
void *register_addressing(CPU *cpu, const char *operand) {
    const char* reg = "^[A-Z]X$"; // Expression régulière pour les registres du type AX, BX, etc.
    
    // Vérifie si l'opérande correspond à un registre
    if (matches(reg, operand) == 0) {
        return NULL; // Si ce n'est pas un registre, retourne NULL
    }
    
    // Recherche le registre dans le contexte du CPU
    void* point = hashmap_get(cpu->context, operand);
    
    // Si le registre existe, retourne son contenu
    if (point != NULL) {
        return point;
    }
    
    return NULL; // Sinon, retourne NULL
}

// Fonction pour gérer le mode d'adressage direct en mémoire
void *memory_direct_addressing(CPU *cpu, const char *operand) {
    const char* reg = "^\\[[0-9]+\\]$"; // Expression régulière pour un nombre entre crochets
    
    // Vérifie si l'opérande correspond à une adresse mémoire directe (ex: [123])
    if (matches(reg, operand) == 0) {
        return NULL; // Si ce n'est pas une adresse mémoire valide, retourne NULL
    }

    int var;
    
    // Extrait l'adresse mémoire de l'opérande
    if (sscanf(operand, "[%d]", &var) != 1) {
        printf("Erreur : impossible d'extraire l'adresse mémoire\n");
        return NULL;
    }

    // Vérifie si l'adresse mémoire contient des données et les retourne
    void *seg = cpu->memory_handler->memory[var];
    if (seg != NULL) {
        return seg;
    }

    return NULL; // Si rien n'est trouvé à cette adresse, retourne NULL
}

// Fonction pour gérer le mode d'adressage indirect par registre
void *register_indirect_addressing(CPU *cpu, const char *operand) {
    const char* reg = "^\\[[A-Z]{2}\\]$";

	if(matches(reg,operand) == 0){
		return NULL;
	}

	char var[10] = {};
	if(sscanf(operand,"[%2s]",var) != 1){
		printf("erreur i pas extrait\n");
		return NULL;
	}

    // Recherche le registre dans le contexte du CPU
    void* point = hashmap_get(cpu->context, var);
    if (point == NULL) {
        printf("Erreur : registre introuvable\n");
        return NULL;
    }

    // Si le registre est trouvé, on récupère la valeur qu'il contient
    int val = *(int*)point;

    // Retourne la valeur à l'adresse mémoire spécifiée par le registre
    void *seg = cpu->memory_handler->memory[val];
    if (seg != NULL) {
        return seg;
    }

    return NULL; // Si aucune valeur n'est trouvée à l'adresse, retourne NULL
}

// Fonction pour exécuter l'instruction MOV qui déplace une valeur d'une source vers une destination
void handle_MOV(CPU* cpu, void* src, void* dest) {
    if (src == NULL || dest == NULL) {
        return; // Si l'une des valeurs est NULL, on ne fait rien
    }
    
    // Déplace la valeur de la source vers la destination
    int entiersource = *(int*)src;
    int *destination = (int*)dest;
    *destination = entiersource;
}

// Fonction pour initialiser un environnement de test avec un CPU et des valeurs spécifiques
CPU *setup_test_environment() {
    // Initialiser le CPU avec 1024 octets de mémoire
    CPU *cpu = cpu_init(1024);
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    // Initialiser les registres avec des valeurs spécifiques
    int *ax = (int*) hashmap_get(cpu->context, "AX");
    int *bx = (int*) hashmap_get(cpu->context, "BX");
    int *cx = (int*) hashmap_get(cpu->context, "CX");
    int *dx = (int*) hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    // Créer et initialiser le segment de données (DS)
    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        printf("\n");
        create_segment(cpu->memory_handler, "DS", 0, 20);

        // Initialiser le segment de données avec des valeurs de test
        for (int i = 0; i < 10; i++) {
            int *value = (int*) malloc(sizeof(int));
            *value = i * 10 + 5; // Valeurs 5, 15, 25, 35...
            store(cpu->memory_handler, "DS", i, value);
        }
    }
    
    printf("\nTest environment initialized\n");
    return cpu;
}

// Fonction qui résout l'adresse en fonction de l'opérande et du mode d'adressage
void *resolve_addressing(CPU *cpu, const char *operand) {
    void *a = immediate_addressing(cpu, operand);
    void *b = register_addressing(cpu, operand);
    void *c = memory_direct_addressing(cpu, operand);
    void *d = register_indirect_addressing(cpu, operand);

    // Si l'un des modes d'adressage fonctionne, retourne l'adresse correspondante
    if (a != NULL) {
        return a;
    }

    if (b != NULL) {
        return b;
    }

    if (c != NULL) {
        return c;
    }

    if (d != NULL) {
        return d;
    }

    // Si aucun mode d'adressage ne fonctionne:
    printf("Aucun des modes d'adressage ne fonctionne\n\n");
    return NULL;
}




