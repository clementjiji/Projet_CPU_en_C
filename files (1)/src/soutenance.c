/* Tenter de l'integrer a votre projet. 
Le faire compiler et l'executer sans erreur ni fuite 
de memoire ;) */

#include"ex1.h"
#include"ex2.h"
#include"ex3.h"
#include"ex4.h"
#include"ex5.h"
#include"ex6.h"
#include"ex7.h"
#include"ex8.h"     /* le nom de votre fichier .h */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void afficher_seg_libres(Segment *seg){
    while(seg != NULL){
        printf("[%d, %d] ", seg->start, seg->size);
        seg = seg->next;
        if(seg != NULL)
            printf("-> ");
    }
    printf("\n");
}

int main()
{
    MemoryHandler* MH = memory_init(17); //Q2.1
    assert(create_segment(MH, "AS", 0, 4)); //Q2.3
    assert(create_segment(MH, "KS", 5, 2)); //Q2.3
    assert(create_segment(MH, "JS", 8, 6)); //Q2.3
    assert(create_segment(MH, "PS", 14, 2)); //Q2.3

    // /* Affichage de la liste chainee des segments libres*/
    afficher_seg_libres(MH->free_list);
    // /* Fin affichage */

    Segment *tmp = hashmap_get(MH->allocated, "AS"); //Q1.4
    printf("\n[%d %d] - ", tmp->start, tmp->size);
    tmp = hashmap_get(MH->allocated, "KS"); //Q1.4
    printf("[%d %d] - ", tmp->start, tmp->size);
    tmp = hashmap_get(MH->allocated, "JS"); //Q1.4
    printf("[%d %d] - ", tmp->start, tmp->size);
    tmp = hashmap_get(MH->allocated, "JS"); //Q1.4
    printf("[%d %d]\n\n", tmp->start, tmp->size);

    remove_segment(MH, "AS");
    remove_segment(MH, "KS");

    /* Affichage de la liste chainee des segments libres*/
	afficher_seg_libres(MH->free_list);
    /* Fin affichage */

    destroy_handler(MH); // (non demande dans le projet) libere la memoire occupee par la structure MemoryHandler

    return 0;
}

