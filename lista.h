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
        printf("----\n");
        printf("Fluxo Atual = %d\n",p->f_atual);
        printf("Fluxo Maximo = %d\n",p->f_max);
        printf("Sentido = %d\n",p->s);
        printf("Id. do no = %d\n",p->id);
    }
}

//Checa se lista inicia com elemento S e termina com T, passando por elemento A1 e A2 (nao necessariamente nessa ordem)
int lst_check(Lista* lst, int S, int A1, int A2, int T){
    Lista* p;
    int simS = 0, simA1 = 0, simA2 = 0, simT = 0;
    //Checa ultimo elemento primeiro (ordem inversa)
    if(lst->id == T)
        simT = 1;
    else
        return 0;

    //Checa elemento por elemento ate encontrar A antes de S, senao retorna 0
    for(p=lst;p!=NULL;p=p->prox){
        if(p->id == S && ((simA1 || simA2) == 0))
            return 0;
        else if(p->id == A1)
            simA1 = 1;
        else if(p->id == A2)
            simA2 = 1;
        else if(p->id == S && p->prox == NULL)
            simS = 1;
    }

    if(simS && simA1 && simA2 && simT)
        return 1;
    else
        return 0;
}

//Atualiza dados na lista de acordo com os sinais e o fluxo encaminhado
Lista* lst_atualiza(Lista* lst, int f, int destino, int s){
    Lista* p;
    int s_aux = 0;

    //Checa elemento por elemento, atualizacao normal
    for(p=lst;p!=NULL;p=p->prox){
        if(p->id == destino){
            s_aux = p->s;
            if(s_aux == 1){
                if(s == 1)
                    p->f_atual = p->f_atual + f;
                else
                    p->f_atual = p->f_atual - f;
            }
            else if(s_aux == 0){
                if(s == 1)
                    p->f_atual = p->f_atual - f;
                else
                    p->f_atual = p->f_atual + f;
            }
            break;
        }
    }

    return lst;
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