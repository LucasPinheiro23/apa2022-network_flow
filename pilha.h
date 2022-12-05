#ifndef PILHA_H
#define PILHA_H
#include "lista.h"

typedef struct pilha Pilha;
struct pilha{
    Lista* prim; /* aponta para o topo da pilha */
};
typedef struct pilha Pilha;

//Checa se pilha está vazia (retorna 1 se afirmativo)
int pilha_vazia (Pilha* p){
    if(p->prim == NULL)
        return 1;
    return 0;
}

//Cria pilha vazia
Pilha* pilha_cria (void){
    Pilha* p = (Pilha*) malloc(sizeof(Pilha));
    p->prim = NULL;
    return p;
}

//Insere elemento com ID especifico no topo da pilha
void pilha_push (Pilha* p, int id){
    Lista* n = (Lista*) malloc(sizeof(Lista));
    n->id = id;
    n->prox = p->prim;
    p->prim = n;
}

//Retorna elemento do topo da pilha, retirando-o da pilha
int pilha_pop (Pilha* p){
    Lista* t;
    int v;
    if (pilha_vazia(p)) exit(1); /* aborta programa */
    t = p->prim;
    v = t->id;
    p->prim = t->prox;
    free(t);
    return v;
}

void pilha_libera (Pilha* p){
    Lista *t, *q = p->prim;
    while (q!=NULL){
        t = q->prox;
        free(q);
        q = t;
    }
    free(p);
}

#endif