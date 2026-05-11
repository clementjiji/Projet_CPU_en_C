#ifndef ex5
#define ex5
 #include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "ex3.h"
#include "ex1.h"
#include "ex2.h"
#include "ex4.h"

int matches ( const char * pattern , const char * string );
void *immediate_addressing(CPU *cpu, const char *operand);
void *register_addressing(CPU *cpu, const char *operand);
void *memory_direct_addressing(CPU *cpu, const char *operand);
void *register_indirect_addressing(CPU *cpu, const char *operand);
void handle_MOV(CPU* cpu, void* src, void* dest);
CPU * setup_test_environment();
void *resolve_addressing(CPU *cpu, const char *operand);




#endif 
