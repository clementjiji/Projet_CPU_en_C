#include "ex3.h"


// Variable statique pour gérer l'index des instructions dans le segment de données
static int nvi = 0;

// Fonction pour parser une instruction dans le segment de données
Instruction *parse_data_instruction(const char *line, HashMap *memory_locations) {
    int i = 0;
    char *ligne = strdup(line);  // Duplique la ligne pour pouvoir la manipuler
    int *i2 = malloc(sizeof(int));  // Alloue un entier pour y stocker l'index
    *i2 = nvi;  // Attribue la valeur actuelle de nvi
    Instruction *ins = malloc(sizeof(Instruction));  
    if (ins == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return NULL;
    }

    // Découpe la ligne en utilisant des espaces comme délimiteurs
    char *m = strtok(ligne, " ");
    ins->mnemonic = strdup(m);  // Enregistre le mnemonic
    ins->operand1 = strdup(strtok(NULL, " "));  // Enregistre l'operand1
    char *l = strtok(NULL, "\n");  // Récupère l'operand2
    ins->operand2 = strdup(l);

    // Insère l'emplacement de la donnée dans la table de hachage
    hashmap_insert(memory_locations, m, i2);

    // Compte le nombre d'opérandes séparés par des virgules
    const char *n = strtok(l, ",");
    while (n != NULL) {
        n = strtok(NULL, ",");
        i++;
    }
    
    nvi = nvi + i;  // Met à jour l'index global en fonction du nombre d'opérandes
    free(ligne);  // Libère la mémoire allouée pour la ligne dupliquée
    return ins;
}

// Variable statique pour gérer l'index des instructions dans le segment de code
static int nbligne = 0;

// Fonction pour parser une instruction dans le segment de code
Instruction *parse_code_instruction(const char *line, HashMap *labels, int code_count) {
    char *ligne = strdup(line);  // Duplique la ligne pour pouvoir la manipuler
    if (ligne == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        return NULL;
    }

    Instruction *ins = malloc(sizeof(Instruction));  // Alloue de la mémoire pour une instruction
    if (ins == NULL) {
        printf("Erreur d'allocation de mémoire pour l'instruction\n");
        free(ligne);
        return NULL;
    }

    ins->mnemonic = NULL;
    ins->operand1 = NULL;
    ins->operand2 = NULL;

    // Cherche si la ligne contient une étiquette (ex: LABEL: instruction)
    char *etiquette = strtok(ligne, ":");
    if (strcmp(etiquette, line) != 0) {  // Si la ligne contient une étiquette
        int *count_ptr = malloc(sizeof(int));
        *count_ptr = code_count;
        hashmap_insert(labels, etiquette, count_ptr);  // Enregistre l'étiquette dans la table des labels

        // Récupère la partie restante après l'étiquette
        char *reste = strtok(NULL, "");
        if (reste != NULL) {
            char *mnemonic = strtok(reste, " ");
            char *operand1 = strtok(NULL, ",\n");
            char *operand2 = strtok(NULL, "\n\n");

            // Remplit l'instruction avec les informations extraites
            if (mnemonic) ins->mnemonic = strdup(mnemonic);
            if (operand1) ins->operand1 = strdup(operand1);
            if (operand2) ins->operand2 = strdup(operand2);
        }
    } else {  // Si la ligne ne contient pas d'étiquette
        char *mnemonic = strtok(ligne, " \t");
        char *operand1 = strtok(NULL, ",\t\n");
        char *operand2 = strtok(NULL, "\n\n");

        // Remplit l'instruction avec les informations extraites
        if (mnemonic) ins->mnemonic = strdup(mnemonic);
        if (operand1) ins->operand1 = strdup(operand1);
        if (operand2) ins->operand2 = strdup(operand2);
        else ins->operand2 = strdup("''");  // Si pas d'operand2, on l'initialise avec une chaîne vide
    }

    nbligne++;  // Incrémente le compteur d'instructions
    free(ligne);  // Libère la mémoire allouée pour la ligne dupliquée
    return ins;
}

// Fonction principale pour parser un fichier source (fichier d'assemblage)
ParserResult *parse(const char *filename) {
    ParserResult *pr = malloc(sizeof(ParserResult));
    pr->data_count = 0;
    pr->code_count = 0;
    pr->memory_locations = hashmap_create();
    pr->labels = hashmap_create();
    char buffer[256];
    int count = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("\nProblème d'ouverture du fichier\n");
        return NULL;
    }

    // Compte les lignes dans la section .DATA
    fgets(buffer, 256, f);
    while (fgets(buffer, 256, f)) {
        if (strcmp(buffer, ".CODE\n") == 0) {
            break;
        }
        pr->data_count++;
    }

    // Compte les lignes dans la section .CODE
    while (fgets(buffer, 256, f)) {
        pr->code_count++;
    }
    fclose(f);

    // Ré-ouvre le fichier pour le parser à nouveau
    f = fopen(filename, "r");
    pr->data_instructions = malloc(sizeof(Instruction*) * (pr->data_count));
    pr->code_instructions = malloc(sizeof(Instruction*) * (pr->code_count));
    fgets(buffer, 256, f);
    
    // Si la section .DATA est trouvée, on commence à parser les instructions de données
    if (strcmp(buffer, ".DATA\n") == 0) {
        while (fgets(buffer, 256, f)) {
            if (strcmp(buffer, ".CODE\n") != 0) {
                Instruction *l = parse_data_instruction(buffer, pr->memory_locations);
                if (l == NULL) {
                    printf("Erreur lors du parsing des données\n");
                }
                pr->data_instructions[count] = l;
                count++;
            } else {
                break;
            }
        }
    }

    // Repars de zéro pour les instructions de code
    count = 0;
    if (strcmp(buffer, ".CODE\n") == 0) {
        while (fgets(buffer, 256, f)) {
            pr->code_instructions[count] = parse_code_instruction(buffer, pr->labels, count);
            count++;
        }
    }
    fclose(f);
    
    // Réinitialise nvi pour de futures utilisations
    nvi = 0;
    return pr;  // Retourne le résultat du parsing
}

// Fonction pour afficher une instruction
void afficher_instru(Instruction *ins) {
    printf("\n");
    if (ins == NULL) {
        printf("Liste d'instructions vide\n");
        return;
    }
    printf("|%s  %s  %s|", ins->mnemonic, ins->operand1, ins->operand2);
}

// Fonction pour afficher un tableau d'instructions
void afficher__tab_instru(Instruction **ins, int taille) {
    printf("\n");
    if (ins == NULL) {
        printf("Liste d'instructions vide\n");
        return;
    }
    if (taille == 0) {
        printf("Erreur: aucune instruction à afficher.\n");
        return;
    }
    for (int i = 0; i < taille; i++) {
        if (ins[i] != NULL) {
            afficher_instru(ins[i]);
        }
    }
    printf("\n");
}

// Fonction pour libérer la mémoire utilisée par le résultat du parser
void free_parser_result(ParserResult *result) {
    // Libère les instructions de données
    for (int i = 0; i < result->data_count; i++) {
        if (result->data_instructions[i]) {
            free(result->data_instructions[i]->mnemonic);
            free(result->data_instructions[i]->operand1);
            free(result->data_instructions[i]->operand2);
            free(result->data_instructions[i]);
        }
    }
    free(result->data_instructions);

    // Libère les instructions de code
    for (int i = 0; i < result->code_count; i++) {
        if (result->code_instructions[i]) {
            if (result->code_instructions[i]->mnemonic) {
                free(result->code_instructions[i]->mnemonic);
            }
            if (result->code_instructions[i]->operand1) {
                free(result->code_instructions[i]->operand1);
            }
            if (result->code_instructions[i]->operand2) {
                free(result->code_instructions[i]->operand2);
            }
            free(result->code_instructions[i]);
        }
    }
    free(result->code_instructions);

    // Libère les tables de hachage utilisées pour les labels et les emplacements de mémoire
    hashmap_destroy(result->labels);
    hashmap_destroy(result->memory_locations);
    free(result);  // Libère le résultat final
}



