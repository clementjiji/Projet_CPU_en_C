#ifndef ex6
#define ex6
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "ex3.h"
#include "ex1.h"
#include "ex2.h"
#include "ex4.h"
#include "ex5.h"
#include "ex6.h"
#include "ex7.h"
#include "ex8.h"


char * trim ( char * str );
int search_and_replace ( char ** str , HashMap * values ) ;
int resolve_constants(ParserResult *result);
void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count);
int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest);
int run_program(CPU *cpu);
Instruction* fetch_next_instruction(CPU *cpu);
int execute_instruction(CPU *cpu, Instruction *instr);
#endif
