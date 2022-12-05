#include<stdio.h>
#include<stdlib.h>
#include "lista.h"
#include "pilha.h"
#include <sys/time.h>

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
//Tempo total de execucao em segundos
int max_exec_time_secs = MAX_EXEC_TIME * 3600;
int horas = 0;
int minutos = 0;
long segundos = 0;

//Funcao para auxiliar na busca em profundidade no grafo.
//Verifica se o ultimo elemento da lista L consegue chegar no destino atraves de uma aresta que os conecta diretamente
//Respeitando os sentidos das arestas, o fluxo na aresta eh computado e retornado
//Se nao for possivel percorrer o caminho (capacidade cheia), retorna -1
int busca_prof(Lista* origem, int id_destino, int f){
    Lista* b;
    int f_aux = 0;
    //Percorre lista origem procurando elemento destino
    b = busca(origem,id_destino);

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

int main(int argc, char* argv[]){

    //Variaveis para utilizacao em loops
    int i, j;
    //Para aferir o tempo de execução durante programa
    struct timeval start, end;
    long seq_seconds, rec_seconds;
    long seq_micros, rec_micros;

    //Faz a leitura da lista de adjacencias do grafo
    //INSERIR AQUI LEITURA DE ARQUIVO TXT

    //DEFINICAO DA INSTANCIA DE TESTES V0:
    //numero de nos incluindo S e T
    int N = 5;
    int S = 0;
    int T = N-1;

    Lista* adj_list[N];
    
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

    //lst_imprime(adj_list[S]);

    //DECLARACAO DAS VARIAVEIS LOCAIS

    //Vetor V que indica se o no ja foi visitado no caminho atual. Se 1, foi visitado. Inicializa como 0 a cada iteracao do algoritmo.
    int V[N];

    //Vetor nei que indica quantos vizinhos tem cada no
    int nei[N];
    for(i = 0; i<N;i++){
        nei[i] = lst_conta(adj_list[i]);
    }
    //Contador de vizinhos que foram retirados da pilha numa mesma iteracao
    int nei_pop;

    //Verificadores do sentido percorrido na aresta A
    int sent_A; //identifica qual foi o sentido (1 se direto: 1->2; 0 se inverso: 2->1)
    //Identificadores de sentido cheio na aresta A
    int cheio_A1 = 0;
    int cheio_A2 = 0;

    //Guarda caminho inverso percorrido durante uma iteracao do algoritmo (de T a S)
    Lista* L = lst_cria();

    //Lista auxiliar
    Lista* l = lst_cria();

    //Para busca em profundidade no grafo
    Pilha* p = pilha_cria();

    //Define fluxo total que se deseja alcancar
    int R = 12;
    //Variavel que acumula o fluxo acumulado a cada iteracao
    int r = 0;
    //Variavel que armazena o fluxo maximo possivel no caminho atual
    int f = 10000;
    //Variavel auxiliar de fluxo
    int f_aux = f;
    //Variavel auxiliar de sentido
    int s_aux = -1;
    //Variavel de controle para verificar se percorreu iniciando de S ate T passando pela aresta A
    int control;

    //Declara extremos da aresta a
    int a[2] = {A_1,A_2};

    //Contador de pilha vazia (CONDICAO DE PARADA)
    int PV = 0;

    //LOOP PRINCIPAL (executa algoritmo ate que r>=R ou o tempo se esgote. Caso PV>PV_MAX, tambem interrompe execucao.)
    gettimeofday(&start, NULL);
    //gettimeofday(&end, NULL);
    printf("INICIO DO ALGORITMO\n\n");
    while(r<R && ((end.tv_sec - start.tv_sec) <= max_exec_time_secs)){
        //****ADICIONAR CONDICAO DE PARADA POR TEMPO AQUI
        gettimeofday(&end, NULL);
        horas = ((end.tv_sec - start.tv_sec)/3600);
        minutos = ((end.tv_sec - start.tv_sec)/60);
        segundos = (end.tv_sec - start.tv_sec);
        printf("Tempo de execucao decorrido aprox: %d:%d:%ld\n", horas,minutos,segundos);
        //Se PV estourou o limite, significa que a pilha esvaziou muitas vezes durante as buscas. Possivelmente os caminhos ja estao bastante cheios. Encerra o programa.
        if(PV > PV_MAX)
            break;
        //Inicializa fluxo da iteracao atual
        f = 0;
        l = lst_cria();
        //Inicializa pilha com vizinhos de S (o no inicial)
        for(l = adj_list[S]; l != NULL; l = l->prox){
            pilha_push(p,l->id);
        }
        lst_libera(l);

        //Inicia sempre pelo S, logo, ativa o primeiro no.
        for(i = 0; i<N;i++){
            V[i] = 0;
        }
        V[0] = 1;

        //Apaga e recria lista L a cada iteracao/novo caminho a percorrer, iniciando sempre pelo S.
        lst_libera(L);
        L = lst_cria();
        L = lst_insere(L,0,0,0,0);

        control = 0;
        nei_pop = 0;

        //Enquanto nao visita um dos extremos da aresta A
        while((V[A_1] || V[A_2]) == 0){
            gettimeofday(&end, NULL);
            horas = ((end.tv_sec - start.tv_sec)/3600);
            minutos = ((end.tv_sec - start.tv_sec)/60);
            segundos = (end.tv_sec - start.tv_sec);
            printf("Tempo de execucao decorrido aprox: %d:%d:%ld\n", horas,minutos,segundos);

            //Se pilha vazia encerra e inicia novo caminho.
            if(pilha_vazia(p)){
                PV++;
                break;
            }
            // Se no atual eh T, entao nao passou ainda por extremos de A e o caminho nao vale.
            if(L->id == T){
                control = 1;
                break;
            }

            //Faz a busca de possivel caminho ate vizinho no topo da pilha. Se vizinho no topo ja foi visitado, libera topo
            if(V[p->prim->id] != 1){
                f_aux = busca_prof(adj_list[L->id],p->prim->id,f);
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
                l = lst_cria();
                //Pega o sentido do fluxo para armazenar no caminho L
                for(l = adj_list[L->id]; l!=NULL; l=l->prox){
                    if(l->id == p->prim->id)
                        s_aux = l->s;
                }
                lst_insere(L,0,0,s_aux,p->prim->id);
                V[L->id] = 1;
                //Desempilha o topo e atualiza vizinhos do novo no na pilha p
                pilha_pop(p);
                lst_libera(l);
                l = lst_cria();
                for(l = adj_list[L->id]; l != NULL; l = l->prox){
                    if(V[l->id] == 0)
                        pilha_push(p,l->id);
                }
                lst_libera(l);
            }
            else{
                pilha_pop(p);
            }
        }

        if(L->id == A_1 && !cheio_A1){
            //Verifica se passa fluxo do A_1 para A_2
            f_aux = busca_prof(adj_list[L->id],A_2,f);
            sent_A = 1;
        }
        else if(L->id == A_2 && !cheio_A2){
            //Verifica se passa fluxo do A_2 para A_1
            f_aux = busca_prof(adj_list[L->id],A_1,f);
            sent_A = 0;
        }

        //Se f_aux == -1 significa que a aresta A esta cheia nesse sentido, entao reinicia o loop
        if(f_aux == -1){
            if(sent_A == 1)
                cheio_A1 == 1;
            else if(sent_A == 0)
                cheio_A2 == 1;
            continue;
        }
        else{

            //Atualiza f, V, L e p
            f = f_aux;
            
            //Se flui no sentido direto da aresta A
            if(sent_A == 1){
                lst_insere(L,0,0,1,A_2);
                V[A_2] = 1;
                //Desempiha numero de vizinhos que a aresta origem (A_1 ou A_2) tinha
                for(i = 0; i < nei[A_1]; i++){
                    pilha_pop(p);
                }
            }
            
            //Se flui no sentido inverso da aresta A
            if(sent_A == 0){
                lst_insere(L,0,0,0,A_1);
                V[A_1] = 1;
                //Desempiha numero de vizinhos que a aresta origem (A_1 ou A_2) tinha
                for(i = 0; i < nei[A_2]; i++){
                    pilha_pop(p);
                }
            }

            //Pega os novos vizinhos do no destino (que ainda nao foram visitados)
            l = lst_cria();
            for(l = adj_list[L->id]; l != NULL; l = l->prox){
                if(V[l->id] == 0)
                    pilha_push(p,l->id);
            }
            lst_libera(l);

            //Enquanto nao visita T...
            while(V[T] == 0){

                //Se pilha vazia encerra e inicia novo caminho.
                if(pilha_vazia(p)){
                    PV++;
                    break;
                }

                //Faz a busca de possivel caminho ate vizinho no topo da pilha. Se vizinho no topo ja foi visitado, libera topo
                if(V[p->prim->id] != 1){
                    f_aux = busca_prof(adj_list[L->id],p->prim->id,f);
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
                    l = lst_cria();
                    //Pega o sentido do fluxo para armazenar no caminho L
                    for(l = adj_list[L->id]; l!=NULL; l=l->prox){
                        if(l->id == p->prim->id)
                            s_aux = l->s;
                    }
                    lst_insere(L,0,0,s_aux,p->prim->id);
                    V[L->id] = 1;
                    //Desempilha o topo e atualiza vizinhos do novo no na pilha p
                    pilha_pop(p);
                    lst_libera(l);
                    l = lst_cria();
                    for(l = adj_list[L->id]; l != NULL; l = l->prox){
                        if(V[l->id] == 0)
                            pilha_push(p,l->id);
                    }
                    lst_libera(l);
                }
                else{
                    pilha_pop(p);
                }
            }
        }

        //Apos percorrer o grafo, se nao incorreu em nenhuma condicao de parada:
        //Checa se variavel de controle eh 0, o que significa que nao achou T antes de A_1 ou A_2.
        if(control == 0){
            //Checa se o caminho eh valido
            if(lst_check(L,S,A_1,A_2,T)){

                //Se sim, acumula fluxo em r
                r = r + f;
                //E atualiza as capacidades das arestas do grafo na lista de adjacencias, com base no fluxo consumido no caminho L
                l = lst_cria();
                for(l=L;l->prox!=NULL;l=l->prox){
                    
                    //Atualiza os fluxos no sentido original do caminho
                    adj_list[l->prox->id] = lst_atualiza(adj_list[l->prox->id],f,l->id,1);
                    //Depois atualiza o mesmo fluxo, mas no sentido oposto na lista de adjacencias
                    adj_list[l->id] = lst_atualiza(adj_list[l->id],f,l->prox->id,0);
                    
                }
                lst_libera(l);

            }
        }

    }
    printf("FIM DO ALGORITMO\n\n");

    if(r >= R){
        printf("Sim, eh possivel passar fluxo minimo %d pela aresta A (%d - %d), partindo de S e chegando em T.", R, A_1, A_2);
        return(1);
    }
    else if(minutos > MAX_EXEC_TIME){
        printf("Tempo limite de execucao excedido (%d minutos).\n", minutos);
        printf("Nao foi possivel determinar a resposta do problema (assume-se negativa).\n");
        return(0);
    }
    else if(PV > PV_MAX){
        printf("A pilha esvaziou no minimo %d vezes, o que implica que o algoritmo teve dificuldades em encontrar caminhos viaveis antes de ser capaz de transmitir fluxo minimo R de S a T passando por A.\nLogo, provavelmente a resposta para o problema eh negativa.", PV_MAX);
        return(0);
    }

}