#include "ex6.h"

char *trim(char *str) {
    // Skip leading whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }

    // Find the end of the string
    char *end = str + strlen(str) - 1;

    // Remove trailing whitespace
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    return str;
}

int search_and_replace(char **str, HashMap *values) {
    if (!str || !*str || !values) return 0;

    int replaced = 0;
    char *input = *str;

    // Iterate through all keys in the hashmap
    for (int i = 0; i < values->size; i++) {
        if (values->table[i].key && values->table[i].key != (void *)-1) {
            char *key = values->table[i].key;
            int value = *(int *)(long)values->table[i].value; //Ici on a largement modifiÃ© car nos valeurs sont allouÃ©es dynamiquement

            // Find potential substring match
            char *substr = strstr(input, key);
            if (substr) {
                // Construct replacement buffer
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

                // Calculate lengths
                int key_len = strlen(key);
                int repl_len = strlen(replacement);
                //int remain_len = strlen(substr + key_len);

                // Create new string
                char *new_str = (char *)malloc(strlen(input) - key_len + repl_len + 1);
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                // Free and update original string
                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    // Trim the final string
    if (replaced) {
        char *trimmed = trim(input);
        if (trimmed != input) {
            memmove(input, trimmed, strlen(trimmed) + 1);
        }
    }

    return replaced;
}


int resolve_constants(ParserResult *result) {
    int resu = 0;

    // Parcourir toutes les instructions
    for(int i=0; i<result->code_count; i++) {
        Instruction *Code = result->code_instructions[i];
        
        // Traiter l'opérande1 si il existe
        if(Code->operand1) {
            // Remplacer les emplacements mémoire dans operand1
            resu += search_and_replace(&(Code->operand1), result->memory_locations);
            
            // Remplacer les labels dans operand1
            resu += search_and_replace(&(Code->operand1), result->labels);
        }
        
        // Traiter l'opérande2 si il existe
        if(Code->operand2) {
            // Remplacer les emplacements mémoire dans operand2
            resu += search_and_replace(&(Code->operand2), result->memory_locations);
            
            // Remplacer les labels dans operand2
            resu += search_and_replace(&(Code->operand2), result->labels);
        }
    }

    // Retourner 1 si des remplacements ont eu lieu, sinon 0
    return (resu > 0) ? 1 : 0;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count){
    // Code commenté pour trouver un segment libre
    /*
    Segment *free_seg = find_free_segment(cpu->memory_handler, 0, code_count, &prev);

    if (!free_seg) {
        printf("Erreur : pas assez d'espace libre pour le segment de code.\n");
        return;
    }
    */
    // Déterminer le point de départ du segment libre
    int start = cpu->memory_handler->free_list->start; 
    
    // Créer un segment de code (CS)
    create_segment(cpu->memory_handler, "CS", start, code_count);

    // Copier les instructions dans la mémoire
    for(int i=0; i < code_count; i++){
        Instruction *ins = malloc(sizeof(Instruction));
        memcpy(ins, code_instructions[i], sizeof(Instruction)); 
        store(cpu->memory_handler, "CS", i, ins);
    }

    // Initialiser le pointeur d'instruction (IP) à 0
    int *ip = (int*)hashmap_get(cpu->context, "IP");
    if (ip != NULL){
        *ip = 0;
    }
}

// Fonction pour l'instruction ADD
void ADD(void *src, void *dest){
    *(int*)dest += *(int*)src;
}

// Fonction pour l'instruction CMP (comparaison)
void CMP(CPU *cpu, Instruction *instr, void *src, void *dest){
    HashMap *context = cpu->context;
    
    // Comparer les valeurs de src et dest
    int val = *(int*)dest - *(int*)src;

    // Mettre à jour le drapeau de zéro (ZF) si égalité
    if(val == 0 || *(int*)src == 1){
        int *r = (int*)hashmap_get(context,"ZF");
        *r = 1;
        return;
    }

    // Mettre à jour le drapeau de signe (SF) si src > dest
    if(*(int*)src > *(int*)dest){
        int *r = (int*)hashmap_get(context,"SF");
        *r = 1;
        return;
    }
}

// Fonction pour l'instruction JMP (saut inconditionnel)
void JMP(CPU *cpu,Instruction *instr){
    int *p = (int*)hashmap_get(cpu->context,"IP");
    if(p && instr->operand1 && strcmp(instr->operand2,"''") != 0){
        printf("IP est modifié par JMP\n");
        *p = (atoi(instr->operand1));  // Mettre à jour l'IP
    }
}

// Fonction pour l'instruction JNZ (saut si non zéro)
void JNZ(CPU *cpu, Instruction *instr){
    int *val = (int*)hashmap_get(cpu->context,"ZF");
    if(val && *val == 0){
        int *p = (int*)hashmap_get(cpu->context,"IP");
        printf("IP est modifié par JNZ\n");
        *p = (atoi(instr->operand1));  // Mettre à jour l'IP si ZF == 0
    }
}

// Fonction pour l'instruction JZ (saut si zéro)
void JZ(CPU *cpu, Instruction *instr){
    int *val = (int*)hashmap_get(cpu->context,"ZF");
    if(*val == 1){
        int *p = (int*)hashmap_get(cpu->context,"IP");
        printf("IP est modifié par JZ\n");
        *p = (atoi(instr->operand1));  // Mettre à jour l'IP si ZF == 1
    }
}

// Fonction pour traiter l'instruction selon le mnémotechnique
int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest){
    if(strcmp(instr->mnemonic,"MOV") == 0){
        handle_MOV(cpu,src,dest);  // Traiter l'instruction MOV
        return 1;
    }

    if(strcmp(instr->mnemonic,"ADD") == 0){
        ADD(src,dest);  // Traiter l'instruction ADD
        return 1;
    }

    if(strcmp(instr->mnemonic,"CMP") == 0){
        CMP(cpu,instr,src,dest);  // Traiter l'instruction CMP
        return 1;
    }

    if(strcmp(instr->mnemonic,"JMP") == 0){
        JMP(cpu,instr);  // Traiter l'instruction JMP
        return 1;
    }

    if(strcmp(instr->mnemonic,"JZ") == 0){
        JZ(cpu,instr);  // Traiter l'instruction JZ
        return 1;
    }

    if(strcmp(instr->mnemonic,"JMZ") == 0){
        JNZ(cpu,instr);  // Traiter l'instruction JNZ
        return 1;
    }

    if(strcmp(instr->mnemonic,"HALT") == 0){
        int *ip = (int*)hashmap_get(cpu->context, "IP");
        *ip = cpu->memory_handler->total_size;  // Arrêter le programme
        return 1;
    }

    if(strcmp(instr->mnemonic,"PUSH") == 0){
        // Empiler la valeur d'AX ou la valeur source
        if(!src){
            int *val = (int*)hashmap_get(cpu->context,"AX");
            push_value(cpu,*val);
        } else {
            push_value(cpu,*(int*)src);
        }
        return 1;
    }

    if(strcmp(instr->mnemonic,"POP") == 0){
        // Dépiler une valeur dans AX ou dans une autre destination
        if(dest == NULL){
            int *val = (int*)hashmap_get(cpu->context,"AX");
            int resu = pop_value(cpu,val);
            if(resu == -1) {return 0;}
        } else {
            int resu = pop_value(cpu,(int*)dest);
            if(resu == -1) {return 0;}
        }
        return 1;
    }

    if(strcmp(instr->mnemonic,"ALLOC") == 0){
        return alloc_es_segment(cpu);  // Allouer un segment de mémoire
    }
    if(strcmp(instr->mnemonic,"FREE") == 0){
        return free_es_segment(cpu);  // Libérer un segment de mémoire
    }
    
    return 0;  // Instruction non reconnue
}

// Fonction pour exécuter l'instruction dans le CPU
int execute_instruction(CPU *cpu, Instruction *instr){
    if(cpu == NULL || instr == NULL){
        printf("Erreur : les arguments sont nuls\n");
        return 0;
    }

    void *src = NULL;
    void *dst = NULL;

    // Résoudre les adresses pour les opérandes
    if(strcmp(instr->operand2,"") != 0){
        dst = resolve_addressing(cpu,instr->operand1);
        src = resolve_addressing(cpu,instr->operand2);
    }

    return handle_instruction(cpu,instr,src,dst);
}

// Fonction pour récupérer l'instruction suivante dans le segment de code
Instruction* fetch_next_instruction(CPU *cpu){
    Segment *seg = hashmap_get(cpu->memory_handler->allocated,"CS");
    if(seg == NULL){
        printf("Erreur : le segment CS est NULL\n");
        return NULL;
    }

    int *ip = hashmap_get(cpu->context,"IP");
    if(ip == NULL){
        printf("Erreur : IP est NULL\n");
        return NULL;
    }

    // Vérifier si l'IP est dans la plage valide
    if (*ip < 0 || *ip >= seg->size){
        printf("Erreur : la valeur de IP n'est pas valide\n");
        return NULL;
    }
    printf("Valeur de IP : %d\n", *ip);
    Instruction *newInstru = (Instruction*)load(cpu->memory_handler,"CS",*ip);
    *ip += 1;

    return newInstru;
}

// Fonction pour exécuter le programme
int run_program(CPU *cpu){
    if(cpu == NULL){
        printf("Erreur : le CPU est NULL\n");
    }

    // Affichage de l'état initial du CPU
    printf("=== Etat initial du CPU ===\n");
    print_data_segment(cpu);
    printf("\n=== Contexte du CPU ===");
    afficher(cpu->context);

    printf("\nAppuyez sur Entrée pour exécuter l'instruction ou 'q' pour quitter\n");

    int i = 1;
    char c = ' ';

    // Boucle d'exécution des instructions
    while(c != 'q'){
        if(c == '\n'){
            Instruction *instr = fetch_next_instruction(cpu);
            if(instr == NULL){
                printf("Il n'y a plus d'instruction à exécuter\n");
                break;
            }

            printf("<=== Instruction exécutée ===>\n");
            afficher_instru(instr);
            execute_instruction(cpu,instr);
            printf("Dans la boucle IP : %d\n", *(int*)hashmap_get(cpu->context,"IP"));
            i++;
            printf("Appuyez sur Entrée pour exécuter la %dème instruction ou 'q' pour quitter\n", i);
        }
        c = getchar();
    }

    // Affichage de l'état final du CPU
    printf("\n\n=== Etat final du CPU ===\n");
    print_data_segment(cpu);
    printf("\n=== Contexte final du CPU ===");
    afficher(cpu->context);
    return 1;
}



