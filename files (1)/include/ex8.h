#ifndef ex8
#define ex8
#include "ex2.h"
#include "ex1.h"
#include "ex3.h"
#include "ex4.h"
#include "ex5.h"
#include "ex6.h"
#include "ex7.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>


void* segment_override_addressing(CPU* cpu, const char* operand);
int find_free_address_strategy(MemoryHandler *handler, int size, int strategy);
int free_es_segment(CPU *cpu);
int alloc_es_segment(CPU *cpu);


#endif
