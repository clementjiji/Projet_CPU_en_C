#ifndef ex4
#define ex4
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "ex3.h"
#include "ex1.h"
#include "ex2.h"


typedef struct {
    MemoryHandler * memory_handler ; // Gestionnaire de memoire
    HashMap * context ; // Registres (AX, BX, CX, DX)
    HashMap * constant_pool;
} CPU ;


void cpu_destroy(CPU *cpu);
CPU *cpu_init(int memory_size);
void* store(MemoryHandler *handler, const char *segment_name, int pos, void *data);
void* load(MemoryHandler *handler, const char *segment_name, int pos);
void allocate_variables(CPU *cpu, Instruction** data_instructions,int data_count);
void print_data_segment(CPU *cpu);
#endif 
