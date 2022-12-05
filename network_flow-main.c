#include<stdio.h>
#include<stdlib.h>
#include "lista.h"
#include "pilha.h"

//Define limiar de pilha vazia (CONDICAO DE PARADA)
#define PV_MAX 3
//Define tempo maximo de execucao do algoritmo, em minutos (CONDICAO DE PARADA)
#define MAX_EXEC_TIME 10
//Define metodo forca bruta (1) ou guloso (2)
#define METHOD 1
//Define IDs dos extremos da aresta A
#define A_1 1
#define A_2 2

//DECLARACAO VARIAVEIS GLOBAIS
//
//

//Funcao para auxiliar na busca em profundidade no grafo.
//Verifica se o ultimo elemento da lista L consegue chegar no destino atraves de uma aresta que os conecta diretamente
//Respeitando os sentidos das arestas, o fluxo na aresta eh computado e retornado
//Se nao for possivel percorrer o caminho (capacidade cheia), retorna -1
int busca_prof(Lista* origem, Lista* destino, int f){
    Lista* b;
    int f_aux = 0;
    //Percorre lista origem procurando elemento destino
    b = busca(origem,destino->id);

    //Se encontra, entao verifica capacidade disponivel
    if(!lst_vazia(b)){
        //Verifica se o sentido eh positivo
        if(b->s == 1){
            //Se sim, calcula a diferenca de fluxo maximo e atual
            f_aux = b->f_max - b->f_atual;
            //Se a diferenca for positiva, pode passar a diferenca de fluxo nessa aresta
            if(f_aux > 0){
                //Se a diferenca for menor que o fluxo maximo atual, atualiza o maximo desse caminho
                if(f_aux < f){
                    f = f_aux;
                }
                //Se nao, apenas retorna o mesmo fluxo maximo atual
                return f;
            }
            //Retorna -1 caso nao haja como passar mais fluxo por esse caminho
            else{
                return -1;
            }
        }
        //Se o sentido for negativo
        else{
            //Verifica o fluxo atual, se for positivo, entao pode remover fluxo da aresta
            f_aux = b->f_atual;
            if(f_aux > 0){
                //Se fluxo atual na aresta for menor que o fluxo f do caminho, entao f deve ser atualizado
                if(f_aux < f){
                    f = f_aux;
                }
                return f;
            }
            //Retorna -1 caso nao haja como passar mais fluxo por esse caminho
            else{
                return -1;
            }
        }
    }
    
    //Se ha capacidade disponivel, retorna novo fluxo maximo
    //Se nao, retorna -1
}

void main(int argc, char* argv[]){

    //Variaveis para utilizacao em loops
    int i, j;

    //Faz a leitura da lista de adjacencias do grafo
    //INSERIR AQUI LEITURA DE ARQUIVO TXT

    //DEFINICAO DA INSTANCIA DE TESTES V0:
    //numero de nos incluindo S e T
    int N = 5;
    int S = 0;
    int T = N-1;

    Lista* adj_list = (Lista*) malloc(N*sizeof(int));
    
    for(i = 0; i < N; i++){
        adj_list[i] = lst_cria();
    }

    adj_list[S] = lst_insere(adj_list[S],0,12,1,1);
    adj_list[S] = lst_insere(adj_list[S],0,5,1,2);
    adj_list[1] = lst_insere(adj_list[1],0,12,0,0);
    adj_list[1] = lst_insere(adj_list[1],0,10,1,2);
    adj_list[1] = lst_insere(adj_list[1],0,8,1,T);
    adj_list[2] = lst_insere(adj_list[2],0,5,0,0);
    adj_list[2] = lst_insere(adj_list[2],0,10,0,1);
    adj_list[2] = lst_insere(adj_list[2],0,1,1,T);
    adj_list[2] = lst_insere(adj_list[2],0,3,1,3);
    adj_list[3] = lst_insere(adj_list[3],0,10,0,2);
    adj_list[3] = lst_insere(adj_list[3],0,12,1,1);
    adj_list[3] = lst_insere(adj_list[3],0,2,1,T);
    adj_list[T] = lst_insere(adj_list[T],0,8,0,1);
    adj_list[T] = lst_insere(adj_list[T],0,1,0,2);
    adj_list[T] = lst_insere(adj_list[T],0,2,0,3);
    //FIM DA DEFINICAO DA INSTANCIA DE TESTES V0

    lst_imprime(adj_list[S]);

    //DECLARACAO DAS VARIAVEIS LOCAIS

    //Vetor V que indica se o no ja foi visitado no caminho atual. Se 1, foi visitado. Inicializa como 0 a cada iteracao do algoritmo.
    int V[N];
    for(i = 0; i<N;i++){
        V[i] = 0;
    }

    //Vetor nei que indica quantos vizinhos tem cada no
    int nei[N];
    for(i = 0; i<N;i++){
        nei[i] = lst_conta(adj_list[i]);
    }
    //Contador de vizinhos que foram retirados da pilha numa mesma iteracao
    int nei_pop;

    //Guarda caminho inverso percorrido durante uma iteracao do algoritmo (de T a S)
    Lista* L = lst_cria();

    //Guarda a capacidade maxima do caminho atual
    int max_cap = 0;

    //Para busca em profundidade no grafo
    Pilha* p = pilha_cria();

    //Define fluxo total que se deseja alcancar
    int R = 12;
    //Variavel que armazena o fluxo acumulado a cada iteracao
    int r = 0;
    //Variavel que armazena o fluxo maximo possivel no caminho atual
    int f = 10000;
    //Variavel auxiliar de fluxo
    int f_aux = f;
    //Variavel de controle para verificar se percorreu iniciando de S ate T passando pela aresta A
    int control = 0;

    //Declara extremos da aresta a
    int a[2] = {A_1,A_2};

    //Contador de pilha vazia (CONDICAO DE PARADA)
    int PV = 0;

    //****ADICIONAR CRONOMETRO AQUI
    while(r<R){
        //****ADICIONAR CONDICAO DE PARADA POR TEMPO AQUI
        //Se PV estourou o limite, significa que a pilha esvaziou muitas vezes durante as buscas. Possivelmente os caminhos ja estao bastante cheios. Encerra o programa.
        if(PV > PV_MAX)
            break;
        //Inicializa fluxo da iteracao atual
        f = 0;
        Lista* l = lst_cria();
        //Inicializa pilha com vizinhos de S (o no inicial)
        for(l = adj_list[S]; l < NULL; l = l->prox){
            pilha_push(p,l->id);
        }
        lst_libera(l);

        //Inicia sempre pelo S, logo, ativa o primeiro no.
        V[0] = 1;

        //Apaga e recria lista L a cada iteracao/novo caminho a percorrer, iniciando sempre pelo S.
        lst_libera(L);
        Lista* L = lst_cria();
        L = lst_insere(L,0,0,0,0);

        control = 0;
        nei_pop = 0;

        //Enquanto nao visita um dos extremos da aresta A
        while((V[A_1] || V[A_2]) == 0){
            //Se pilha vazia encerra e inicia novo caminho. Se no atual eh T, entao nao passou ainda por extremos de A e o caminho nao vale.
            if(pilha_vazia(p))
                break;
            if(L->id == T){
                control = 1;
                break;
            }
            //Faz a busca de possivel caminho ate vizinho no topo da pilha. Se vizinho no topo ja foi visitado, libera topo
            if(V[p->prim] != 1){
                f_aux = busca_prof(adj_list[L->id],p->prim,f);
                //Se f_aux == -1 significa que a aresta esta cheia, entao desempilha e reinicia o loop
                if(f_aux == -1){
                    pilha_pop(p);
                    //Verifica se ainda existem vizinhos a visitar antes de continuar o loop
                    nei_pop++;
                    if(nei_pop == nei[L->id])
                        //Se visitou todos os vizinhos do no atual, retorna para o elemento anterior no caminho (backtracking)
                        L = L->prox;
                    continue;
                }
                //Se o caminho eh viavel, atualiza fluxo maximo do caminho atual, e atualiza V, L e p
                f = f_aux;
                lst_insere(L,0,0,0,p->prim);
                V[L->id] = 1;
                //Desempilha o topo e atualiza vizinhos do novo no na pilha p
                pilha_pop(p);
                Lista* l = lst_cria();
                for(l = adj_list[L->id]; l < NULL; l = l->prox){
                    pilha_push(p,l->id);
                }
                lst_libera(l);
            }
            else{
                pilha_pop(p);
            }
            

        }

    }

}