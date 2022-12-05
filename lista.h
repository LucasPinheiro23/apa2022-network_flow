#ifndef LISTA_H
#define LISTA_H

typedef struct lista Lista;
struct lista{
    int f_atual; //Fluxo atual
    int f_max; //Fluxo maximo
    int s; //sentido do fluxo
    int id; //Identificacao do vertice
    Lista* prox;
};

//Cria lista vazia
Lista* lst_cria(void){
    return NULL;
}

//Insere elemento na lista
Lista* lst_insere(Lista* lst, int f_atual, int f_max, int s, int id){
    Lista* novo = (Lista*) malloc(sizeof(Lista));
    
    novo->f_atual = f_atual;
    novo->f_max = f_max;
    novo->s = s;
    novo->id = id;

    novo->prox = lst;

    return novo;
}

//Imprime lista
Lista* lst_imprime(Lista* lst){
    Lista* p;
    for(p=lst;p!=NULL;p=p->prox){
        printf("Fluxo Atual = %d\n",p->f_atual);
        printf("Fluxo Maximo = %d\n",p->f_max);
        printf("Sentido = %d\n",p->s);
        printf("Id. do no = %d\n",p->id);
    }
}

//Conta elementos na lista
int lst_conta(Lista* lst){
    Lista* p;
    int c = 0;
    for(p=lst;p!=NULL;p=p->prox){
        c++;
    }
    return c;
}

//Retorna 1 se vazia ou 0 se nao vazia
int lst_vazia (Lista* lst){
    return (lst == NULL);
}

//Busca pelo ID e retorna ponteiro para elemento na lista
Lista* busca(Lista* lst, int v){
    Lista* p;
    for(p = lst; p != NULL; p = p->prox){
        if(p->id == v){
            return p;
        }
    }

    return NULL;
}

//Libera lista na memória
void lst_libera(Lista* lst){
    Lista* p = lst;
    while(p!=NULL){
        Lista* t = p->prox;
        free(p);
        p = t;
    }
}

#endif