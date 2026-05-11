#include "ex8.h"
/*

int main (){
	printf("\n---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 1\n");
	printf("---------------------------------------------------------------\n\n");
	
    // ===== Création de la table de hachage =====
    printf("===== Création de la table de hachage =====\n");
    HashMap *map = hashmap_create();
    if (map == NULL) {
        printf("Erreur : création de la table de hachage échouée.\n\n");
        return 1; // Arrêt si la création échoue
    }
    printf("Création réussie.\n\n");

    // ===== Insertion de valeurs =====
    printf("===== Insertion de valeurs =====\n");

    int* valeur1 = malloc(sizeof(int)); 
    *valeur1 = 10;
    printf("Allocation et insertion : key1 -> %d\n", *valeur1);
    hashmap_insert(map, "key1", valeur1);

    int* valeur2 = malloc(sizeof(int)); 
    *valeur2 = 5;
    printf("Allocation et insertion : key2 -> %d\n", *valeur2);
    hashmap_insert(map, "key2", valeur2);

    int* valeur3 = malloc(sizeof(int)); 
    *valeur3 = 5;
    printf("Mise à jour : key1 -> %d\n", *valeur3);
    hashmap_insert(map, "key1", valeur3);

    printf("\nAffichage de la table après insertion :\n");
    afficher(map);

    // ===== Recherche avec hashmap_get =====
    printf("\n===== Test de récupération =====\n");
    int* val = hashmap_get(map, "key1");
    if (val) {
        printf("key1 trouvée : %d\n", *val);
    } else {
        printf("key1 non trouvée\n");
    }

    // ===== Suppression de 'key1' =====
    printf("\n===== Suppression de 'key1' =====\n");
    hashmap_remove(map, "key1");

    val = hashmap_get(map, "key1");
    if (val) {
        printf("Échec : 'key1' existe encore\n");
    } else {
        printf("'key1' bien supprimée\n");
    }

    // ===== Test de collision =====
    printf("\n===== Test de collision =====\n");
    int* vc1 = malloc(sizeof(int)); 
    *vc1 = 100;
    printf("Insertion : ab -> %d\n", *vc1);
    hashmap_insert(map, "ab", vc1);

    int* vc2 = malloc(sizeof(int)); 
    *vc2 = 200;
    printf("Insertion : ba -> %d\n", *vc2);
    hashmap_insert(map, "ba", vc2);

    printf("\nAffichage après insertion de clés provoquant une collision :\n");
    afficher(map);

    // ===== Destruction de la table =====
    printf("\n===== Destruction de la table =====\n");
    hashmap_destroy(map);
    printf("Table détruite.\n\n");


	printf("\n---------------------------------------------------------------\n");
	printf("TEST DE L'EXERCICE 2\n");   
	printf("---------------------------------------------------------------\n\n");
	
	

 // ===== Création du MemoryHandler =====
    printf("\n=== Création d'un MemoryHandler ===\n");
    MemoryHandler *mem1 = memory_init(18);  // Initialisation d'un gestionnaire de mémoire de taille 18
    Segment *prev;  // Pointeur pour garder une trace du segment précédent
    int start = 2;  // Position de départ pour la recherche de segments libres
    int size = 2;   // Taille du segment à rechercher
    
    if (mem1==NULL){
        printf("erreur lors de la création du MemoryHandler");
    }
    else{
         printf("Création du MemoryHandler\n");
    }
    
    // ===== Recherche d'un segment libre =====
    printf("\n=== Recherche d'un segment libre pour savoir si on peut créer un segment ===\n");
    Segment *f1 = find_free_segment(mem1, start, size, &prev);  // Cherche un segment libre à partir de la position 'start' et de la taille 'size'

    // Affichage du segment trouvé ou message d'erreur si aucun segment libre n'est trouvé
    if (f1) {
        printf("Segment trouvé : ");
        afficher_seg(f1);  // Affiche les informations sur le segment trouvé
    } else {
        printf("Aucun segment libre trouvé pour la taille %d à partir du départ %d.\n", size, start);
    }

    // ===== Tentative de création d'un segment =====
    printf("\n\n=== Tentative de création d'un segment ===\n");
    printf("Tentative de création d'un segment de taille %d à partir de %d\n", size, start);
    int verification1 = create_segment(mem1, "bonjour", 2, 3);  // Essaie de créer un segment nommé "bonjour" de taille 3 à partir de l'indice 2
    if (verification1 == 0) {
        printf("\nProblème détecté dans la fonction create_segment.\n");
    } else {
        printf("\nAucun problème dans la fonction create_segment.\n");
    }

    // ===== Affichage de l'état actuel après création =====
    printf("\n=== État actuel de la liste des segments libres après création ===");
    afficher_free_list(mem1);  // Affiche les segments libres après la création du segment "bonjour"
    printf("\n");

    // Affichage de l'état des segments alloués
    printf("=== État des segments alloués après la création ===\n");
    afficher(mem1->allocated);  // Affiche les segments alloués après la création

    // ===== Recherche d'un segment alloué spécifique =====
    printf("=== Recherche du segment alloué pour 'bonjour' ===\n");
    Segment *segAliberer = (Segment*)hashmap_get(mem1->allocated, "bonjour");  // Cherche le segment alloué pour la clé "bonjour"
    if (segAliberer) {
        printf("Segment alloué pour 'bonjour' trouvé :\n");
        afficher_seg(segAliberer);  // Affiche les informations sur le segment alloué
    } else {
        printf("Aucun segment alloué pour 'bonjour' trouvé.\n");
    }

    // ===== Test d'une erreur : Tentative d'allocation dans un espace inexistant =====
    printf("\n\n=== Test d'une erreur : essayer d'allouer dans un espace inexistant ===\n");
    MemoryHandler *mem2 = memory_init(18);  // Crée un nouveau gestionnaire de mémoire

    // Tentative de création d'un segment "bonjour1" qui empiéterait sur un segment existant
    int v2 = create_segment(mem2, "bonjour1", 4, 2);
    if (v2 == 0) {
        printf("Problème détecté dans la création de 'bonjour1' car un segment existe déjà à cet emplacement.\n");
    } else {
        printf("Aucun problème dans la création de 'bonjour1'.\n");
    }

    // ===== Tentative de suppression du segment "bonjour" =====
    printf("\n=== Tentative de suppression du segment 'bonjour' ===\n");
    int vvvv = remove_segment(mem2, "bonjour");  // Essaie de supprimer le segment nommé "bonjour"
    if (vvvv == 0) {
        printf("Erreur : Le segment '%s' n'existe pas dans la table des alloués.\n(l'erreur est donc détécté tout est bon!')\n", "bonjour");
    } else {
        printf("=== Suppression réussie du segment '%s' ===\n", "bonjour");
    }

    // ===== Destruction du gestionnaire de mémoire =====
    printf("\n=== Destruction du MemoryHandler ===\n");
    destroy_handler(mem1);  // Libère la mémoire utilisée par mem1
    destroy_handler(mem2);  // Libère la mémoire utilisée par mem2
    printf("Mémoire détruite.\n");
	
	
	
	printf("\n---------------------------------------------------------------\n");
	printf("TEST DE L'EXERCICE 3\n");   
	printf("---------------------------------------------------------------\n\n");
	
	// Création du ParserResult
    printf("===Création du ParserResult===\n");

    char *fichier2 = "parser.tkt";
    ParserResult *par = parse(fichier2);

    // Vérification de la création du ParserResult
    if (par == NULL) {
        printf("Erreur lors de la création du ParserResult\n");
    } else {
        printf("Création du ParserResult réussie\n");
    }

    // Affichage des instructions de données
    printf("\n===Affichage de data_instructions===");
    afficher__tab_instru(par->data_instructions, par->data_count);

    // Affichage des instructions de code
    printf("\n===Affichage de code_instructions===");
    afficher__tab_instru(par->code_instructions, par->code_count);

    // Affichage des emplacements mémoire
    printf("\n===Affichage de memory_locations===");
    afficher(par->memory_locations);

    // Affichage des labels
    printf("===Affichage de labels===");
    afficher(par->labels);

    // Destruction du ParserResult
    printf("===Destruction du ParserResult===\n");
    free_parser_result(par);

    // Message de fin
    printf("parse détruit\n\n\n");
	
	
	printf("---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 4\n");	
	printf("création du meme élément que a l'exercice 3:\n");
	printf("---------------------------------------------------------------\n");
	
	 const char *fichier = "parser.tkt";
    ParserResult *p2 = parse(fichier);
    if (!p2) {
        printf("Erreur : échec du parsing du fichier : %s\n", fichier);
        return 1;
    }

    printf("\n===Affichage de data_instructions===");
    afficher__tab_instru(p2->data_instructions, p2->data_count);

    printf("\n===Affichage de code_instructions===");
    afficher__tab_instru(p2->code_instructions, p2->code_count);

    printf("\n===Affichage de memory_locations===");
    afficher(p2->memory_locations);

    printf("\n===Affichage de labels===");
    afficher(p2->labels);

    // Initialisation du CPU
    printf("\n===Création du cpu_ex4===\n");
    CPU *cpu_ex4 = cpu_init(20);
    if (!cpu_ex4) {
        printf("Erreur : échec de l'initialisation du cpu_ex4.\n");
        free_parser_result(p2);
        return 1;
    }

    MemoryHandler *mem = cpu_ex4->memory_handler;

    // Création d'un segment mémoire "bonjour"
    const char *seg_name = "bonjour";
    int verification = create_segment(mem, seg_name, 9, 4);// on prend aléatoirement un segment [9-13] qui fait partie de segments libres
    if (verification == 0) {
        printf("Erreur : échec de la création du segment '%s'.\n", seg_name);
    } else {
        printf("\nSegment '%s' créé avec succès", seg_name);
    }
    //on affiche la liste des segments libres
    afficher_free_list(mem);

    printf("=== Segments alloués ===");
    afficher(mem->allocated);

    // Stockage d'une valeur dans la mémoire
    printf("=== Stockage d'une valeur 'hello' dans le segment '%s' ===", seg_name);
    char *val1 = malloc(strlen("hello") + 1);
    if (!val1) {
        printf("Erreur : allocation mémoire échouée pour 'hello'.\n");
        
    }
    strcpy(val1, "hello");
    store(mem, seg_name, 3, val1);

    // Affichage mémoire
    affichage_mem_char(mem->memory, mem->total_size);

    // Chargement depuis la mémoire
    printf("\n=== Lecture de la mémoire dans le segment '%s' ===\n", seg_name);
    void *val3 = load(mem, seg_name, 3);
    if (val3)
        printf("Indice 3 → Valeur : %s\n", (char *)val3);
    else
        printf("Indice 3 → Aucun élément trouvé.\n");

    void *val2 = load(mem, seg_name, 2);
    if (val2)
        printf("Indice 2 → Valeur : %s\n", (char *)val2);
    else
        printf("Indice 2 → Aucun élément trouvé.\n");
        
    //on affiche les segment libres
    afficher_free_list(mem);

    // Allocation des variables dans le segment "DS"
    printf("\n=== Allocation des variables dans le segment 'DS' ===\n");
    allocate_variables(cpu_ex4, p2->data_instructions, p2->data_count);
    printf("\nNombre de données allouées : %d\n", p2->data_count);

    printf("\n=== Segments alloués après allocation ===");
    afficher(mem->allocated);
    
    printf("\n=== Affichage des elements alloué dans le Segments DS après allocation ===");
    print_data_segment(cpu_ex4);
    
    // Nettoyage final
    printf("\n=== Destruction du cpu_ex4 et libération des ressources ===\n");
    cpu_destroy(cpu_ex4);
    free_parser_result(p2);
    printf("tout l'espace est libéré");

	
	
	
	printf("---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 5\n");
	printf("---------------------------------------------------------------\n");
	

	
    printf("===Création du CPU===\n");
    CPU* cpu = setup_test_environment();
    if (cpu==NULL){printf("erreur lors de la création de CPU");}
    else printf("Le  CPU est bien crée");

    printf("\n<===immediate addressing===>\n");
    
    void *a = immediate_addressing(cpu,"42");
    if(a == NULL){
        printf("erreur immediate addressing n'a pas fonctionner\n");
    }
    else{
        printf("immediate addressing est bon avec operand=42\n");
    }

    printf("\n<===register addressing===>\n");
    void *b = register_addressing(cpu,"BX");
    if(b == NULL){
        printf("erreur register addressing n'est pas bon \n");
    }
    else{
        printf("register addressing est bon avec operand=BX \n");
    }

    printf("\n<===memory direct addressing===>\n");
    void *c = memory_direct_addressing(cpu,"[1]");
    if(c == NULL){
        printf("erreur memory direct addressing n'est pas bon\n");
    }
    else{
        printf("memory direct addressing est bon avec operand=[1]\n");
    }

    printf("\n====register indirect addressing====\n");
    void *d = register_indirect_addressing(cpu,"[AX]");
    if(d == NULL){
        printf("erreur register indirect addressing n'est pas bon \n");
    }
    else{
        printf("register indirect addressing est bon avec operand=[AX]\n");
    }

    printf("\n=== Destruction du cpu_ex4 et libération des ressources ===\n");
    cpu_destroy(cpu);
    printf("tout l'espace est libéré");



    printf("---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 6\n");
	printf("---------------------------------------------------------------\n");
	
	
	// Création du ParserResult à partir d’un fichier de description
    printf("Création du ParserResult\n");
    char *nom_fichier_parser = "parser.tkt";
    ParserResult *parser_result = parse(nom_fichier_parser);
    if (parser_result==NULL){printf("erreur lors de la création de parser_result");}
    else printf("Le  ParserResult est bien créé");

    // Affichage des instructions de données
    printf("\nAffichage de data_instructions");
    afficher__tab_instru(parser_result->data_instructions, parser_result->data_count);

    // Affichage des instructions de code
    printf("\nAffichage de code_instructions");
    afficher__tab_instru(parser_result->code_instructions, parser_result->code_count);

    // Affichage des emplacements mémoire
    printf("\nAffichage de memory_locations");
    afficher(parser_result->memory_locations);

    // Affichage des labels
    printf("\nAffichage de labels");
    afficher(parser_result->labels);

    // Initialisation du CPU principal avec une taille donnée
    printf("\nCréation du CPU\n");
    CPU *cpu_principal = cpu_init(20);
    if (cpu_principal==NULL){printf("erreur lors de la création de CPU");}
    else printf("Le  CPU est bien créé");

    // Remplacement des constantes par leurs valeurs dans le code
    resolve_constants(parser_result);

    // Affichage du code modifié après remplacement des constantes
    printf("\nAffichage de code_instructions après resolve_constants");
    afficher__tab_instru(parser_result->code_instructions, parser_result->code_count);

    printf("\n");

    // Allocation des variables dans le segment de données du CPU
    printf("\nAllocation des variables dans le CPU\n");
    allocate_variables(cpu_principal, parser_result->data_instructions, parser_result->data_count);

    // Affichage des segments mémoire alloués (données, pile, etc.)
    printf("\nSegments alloués (Data + Autres)");
    afficher(cpu_principal->memory_handler->allocated);

    // Allocation du segment de code dans la mémoire
    allocate_code_segment(cpu_principal, parser_result->code_instructions, parser_result->code_count);

    // Vérification du segment de code ("CS") dans la mémoire
    MemoryHandler *gestionnaire_memoire = cpu_principal->memory_handler;
    printf("\nVérification du segment de code 'CS'\n");

    Segment *segment_cs = (Segment *)hashmap_get(gestionnaire_memoire->allocated, "CS");
    if (!segment_cs) {
        printf("Erreur : segment 'CS' introuvable.\n");
    } else {
        printf("Segment 'CS' trouvé. Début : %d | Taille : %d\n\n", segment_cs->start, segment_cs->size);

        // Parcours des instructions dans le segment de code
        for (int idx_instr = 0; idx_instr < segment_cs->size; idx_instr++) {
            Instruction *instr = (Instruction *)load(gestionnaire_memoire, "CS", idx_instr);

            if (!instr) {
                printf("Instruction NULL à l’indice %d\n", idx_instr);
                continue;
            }

            // Affichage formaté de chaque instruction
            printf("[%d] |%s %s %s|\n", idx_instr,
                   instr->mnemonic ? instr->mnemonic : "(null)",
                   (instr->operand1 && strcmp(instr->operand1, "''") != 0) ? instr->operand1 : "''",
                   (instr->operand2 && strcmp(instr->operand2, "''") != 0) ? instr->operand2 : "''");
        }
    }

    printf("Fin de la vérification du segment 'CS'.\n\n");

    // Affichage final des segments mémoire alloués
    if (gestionnaire_memoire->allocated == NULL) {
        printf("Erreur : Aucun segment alloué trouvé.\n");
    } else {
        printf("Affichage des segments alloués après allocation");
        afficher(gestionnaire_memoire->allocated);
    }

    // Création d’un second CPU pour tester des instructions individuellement
    printf("\nOn crée un nouveau CPU pour tester les fonctions\n");
    CPU *cpu_test = setup_test_environment();
    int registre_val1 = 10;
    int registre_val2 = 20;
    afficher(cpu_test->memory_handler->allocated);

    // Test de la fonction handle_instruction avec diverses instructions
    printf("\nTest Handle Instruction\n");
    Instruction *instr_test = malloc(sizeof(Instruction));

    // Test de l'instruction MOV
    printf("\nTest de MOV\n");
    printf("Valeurs avant MOV : val1 = %d, val2 = %d\n", registre_val1, registre_val2);
    instr_test->mnemonic = "MOV";
    handle_instruction(cpu_test, instr_test, &registre_val1, &registre_val2);
    printf("Valeurs après MOV : val1 = %d, val2 = %d\n\n", registre_val1, registre_val2);
    afficher(cpu_principal->context);

    // Test de l'instruction ADD
    instr_test->mnemonic = "ADD";
    printf("\nTest de ADD\n");
    handle_instruction(cpu_test, instr_test, &registre_val1, &registre_val2);
    printf("Valeurs après ADD : val1 = %d, val2 = %d\n\n", registre_val1, registre_val2);
    afficher(cpu_principal->context);

    // Test de l'instruction CMP
    instr_test->mnemonic = "CMP";
    printf("\nTest de CMP\n");
    printf("On met val1 = 20\n");
    registre_val1 = 20;
    handle_instruction(cpu_test, instr_test, &registre_val1, &registre_val2);
    printf("Valeurs : val1 = %d, val2 = %d\n", registre_val1, registre_val2);
    printf("Valeur de ZF : %d\n\n", *(int*)hashmap_get(cpu_test->context, "ZF"));
    afficher(cpu_principal->context);

    // Test de l'instruction JMP
    instr_test->mnemonic = "JMP";
    instr_test->operand1 = "5";
    instr_test->operand2 = " ";
    printf("\nTest de JMP\n");
    printf("On met operand1 = '5'\n");
    handle_instruction(cpu_test, instr_test, NULL, NULL);
    printf("JMP : IP = %d\n\n", *(int*)hashmap_get(cpu_test->context, "IP"));

    // Test de l'instruction JZ
    instr_test->mnemonic = "JZ";
    instr_test->operand1 = "10";
    printf("\nTest de JZ\n");
    printf("On met ZF à 1 et operand1 à '10'\n");
    *(int*)hashmap_get(cpu_test->context, "ZF") = 1;
    handle_instruction(cpu_test, instr_test, NULL, NULL);
    printf("JZ : IP = %d\n\n", *(int*)hashmap_get(cpu_test->context, "IP"));

    // Test de l'instruction JNZ
    instr_test->mnemonic = "JNZ";
    instr_test->operand1 = "10";
    printf("\nTest de JNZ\n");
    printf("On met ZF à 0 et operand1 à '10'\n");
    *(int*)hashmap_get(cpu_test->context, "ZF") = 0;
    handle_instruction(cpu_test, instr_test, NULL, NULL);
    printf("JNZ : IP = %d\n\n", *(int*)hashmap_get(cpu_test->context, "IP"));

    // Test de l'instruction HALT (nécessite un segment de code)
    instr_test->mnemonic = "HALT";
    create_segment(cpu_test->memory_handler, "CS", 100, 10);
    printf("\nTest de HALT\n");
    handle_instruction(cpu_test, instr_test, NULL, NULL);
    printf("\n");

    // Test de l'instruction PUSH
    instr_test->mnemonic = "PUSH";
    printf("\nTest de PUSH\n");
    handle_instruction(cpu_test, instr_test, &registre_val1, &registre_val2);
    printf("Valeurs après PUSH : val1 = %d, val2 = %d\n\n", registre_val1, registre_val2);

    // Test de l'instruction POP
    instr_test->mnemonic = "POP";
    printf("\nTest de POP\n");
    handle_instruction(cpu_test, instr_test, &registre_val1, &registre_val2);
    printf("Valeurs après POP : val1 = %d, val2 = %d\n\n", registre_val1, registre_val2);

    // Libération de l’instruction et du CPU test
    free(instr_test);
    cpu_destroy(cpu_test);

    // Exécution complète du programme sur le CPU principal
    printf("\n------------------------ Test de run_program ------------------------\n\n");
    run_program(cpu_principal);

    // Libération finale de toutes les ressources
    printf("\nDestruction des structures\n");
    free_parser_result(parser_result);
    cpu_destroy(cpu_principal);
    printf("tout l'espace est libre\n\n");

	        
	        
	
	printf("---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 7\n");
	printf("---------------------------------------------------------------\n");
	printf("\n");
	
    printf("===Création du CPU===\n");
    // Initialisation du CPU et du ParserResult pour test avec run_program
    CPU* cpu3 = setup_test_environment();
    ParserResult* result3 = parse("parser.tkt");

    if (cpu3==NULL){
        printf("erreur lors de la création de CPU");
    }
    else {
        printf("Le  CPU est bien crée");
    }

    // Remplacement des constantes dans les instructions du ParserResult
    resolve_constants(result3);

    // Allocation du segment de code du programme dans la mémoire du CPU
    allocate_code_segment(cpu3, result3->code_instructions, result3->code_count);

    printf("\n\n----------------run_program-------------------------------\n\n");
    // Exécution du programme en utilisant le CPU initialisé
    run_program(cpu3);

    // Libération des ressources après exécution
    printf("\n===Destruction des structures===\n");
    free_parser_result(result3);
    cpu_destroy(cpu3);
    printf("tout l'espace est libre\n\n");


    // Initialisation d’un nouveau CPU avec taille mémoire 1024 pour tests de pile
    CPU *cp = cpu_init(1024);

    printf("\n===Test push cpu===n");
    printf("\n");

    // Test de la fonction push_value : empile la valeur 40
    int resultat_push = push_value(cp,40);
    if(resultat_push == 0){
        printf("Le push a reussit pour value = 40\n");
        printf("\n");
        // Affiche les segments de mémoire alloués après le push
        afficher(cp->memory_handler->allocated);
    }
    else{
        printf("Le push a echouer\n");
        printf("\n");
    }

    // Test de la fonction pop_value : dépile une valeur dans dest
    int dest = 0;
    int resultat_pop = pop_value(cp,&dest);
    if(resultat_pop == 0){
        printf("pop reussi \n");
        printf("valeur de dest = %d\n",dest);
        printf("\n");
    }
    else{
        printf("erreur pop echouer\n");
    }

    printf("===Deuxieme appel a pop===\n\n");
    // Deuxième appel à pop pour tester la gestion d'erreur de pile vide
    int erreurpourlod = pop_value(cp,&dest);

    if(erreurpourlod != 0){
        printf("\nLe Segment SS n'a qu'un seul élément donc erreur pour un deuxieme appel a pop\n\n");
    }

    printf("===Test de handle_instruction===\n\n");

    // Test de handle_instruction avec les instructions PUSH et POP
    Instruction *instru = malloc(sizeof(Instruction));
    int s = 40;
    int dddd = 24;
    printf("Avant TOUS: src=%d, dest=%d\n", s, dddd);

    // Instruction PUSH
    instru->mnemonic = "PUSH";
    printf("\n===TEST PUSH===\n");
    printf("Avant PUSH: src=%d, dest=%d\n", s, dddd);
    handle_instruction(cp, instru, &s, &dddd);

    // Instruction POP
    instru->mnemonic = "POP";
    if(strcmp(instru->mnemonic,"MOV") == 0){
    }
    printf("\n===TEST POP===\n");
    printf("Avant POP: dest=%d\n", dddd);
    handle_instruction(cp, instru, &s, &dddd); 
    printf("Après POP: dest=%d\n", dddd);

    // Libération de la mémoire utilisée pour l'instruction et destruction du CPU
    free(instru);
    cpu_destroy(cp);


    printf("---------------------------------------------------------------");
	printf("\nTEST DE L'EXERCICE 8\n");
	printf("---------------------------------------------------------------\n");
	printf("\n");

    // Initialisation pour les tests d'allocation segment ES
    printf("===Création du CPU===\n");
    CPU* cpu4 = setup_test_environment();
    ParserResult* result4 = parse("parser.tkt");
    if (cpu4==NULL){printf("erreur lors de la création de CPU");}
    else printf("Le  CPU est bien crée");

    resolve_constants(result4);
    allocate_code_segment(cpu4, result4->code_instructions, result4->code_count);


    //===alloc_es_segment===
    printf("\n===alloc_es_segment===\n");

    *(int*)hashmap_get(cpu4->context, "BX") = 1;

    int retour_alloc = alloc_es_segment(cpu4);
    if (retour_alloc == 0) {
        printf("Erreur : allocation du segment ES a échoué.\n");
    } else {
        printf("Allocation du segment ES réussie.\n");
    }


    //===segment_override_addressing===
    printf("\n===segment_override_addressing===\n");

    void* resultat_override = segment_override_addressing(cpu4, "[ES:BX]");
    if (resultat_override == NULL) {
        printf("Erreur : échec du segment override.\n");
    } else {
        printf("Segment override effectué correctement.\n");
    }


    //===find_free_address_strategy===
    printf("\n===find_free_address_strategy===\n");

    int adresse_libre = find_free_address_strategy(cpu4->memory_handler, 50, 0);
    if (adresse_libre == -1) {
        printf("Aucune adresse libre trouvée pour 50 octets.\n");
    } else {
        printf("Adresse libre trouvée à l'index : %d\n", adresse_libre);
    }


    //===free_es_segment===
    printf("\n===free_es_segment===\n");

    int es_avant = *(int*)hashmap_get(cpu4->context, "ES");
    printf("Valeur de ES avant libération : %d\n", es_avant);

    free_es_segment(cpu4);

    int es_apres = *(int*)hashmap_get(cpu4->context, "ES");
    printf("Valeur de ES après libération : %d\n", es_apres);
    if (es_apres == -1) {
        printf("Le segment ES a été libéré correctement.\n");
    } else {
        printf("Erreur : le segment ES n'a pas été libéré.\n");
    }


    // Libération des ressources
    printf("\n===Destruction des structures===\n");
    free_parser_result(result4);
    cpu_destroy(cpu4);
    printf("Tous les espaces mémoire ont été libérés.\n\n");

	
return 0;}


*/