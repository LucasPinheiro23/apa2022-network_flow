#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

#include "lista.h"
#include "pilha.h"
#include "randomize.h"


//Define limiar de pilha vazia (CONDICAO DE PARADA)
#define PV_MAX 1000000
//Define tempo maximo de execucao do algoritmo, em minutos (CONDICAO DE PARADA)
#define MAX_EXEC_TIME 1000
//Define valor maximo de capacidade por aresta a ser considerada nesse problema
#define MAX_CAP 10000

//Define metodo forca bruta (1) ou guloso (2)
#define METHOD 1
//Define IDs dos extremos da aresta A
#define A_1 1
#define A_2 2
//Valor de R a ser alcancado
#define TARGET 3

//DECLARACAO VARIAVEIS GLOBAIS

//Tempo total de execucao em segundos
int max_exec_time_secs = MAX_EXEC_TIME * 60;
int horas = 0;
int minutos = 0;
long segundos = 0;

/* Funcao para auxiliar na busca em profundidade no grafo.
   Verifica se o ultimo elemento da lista L consegue chegar no destino atraves de uma aresta que os conecta diretamente
   Respeitando os sentidos das arestas, o fluxo na aresta eh computado e retornado
   Se nao for possivel percorrer o caminho (capacidade cheia), retorna -1 */
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
}

void main(int argc, char* argv[]){

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
    adj_list[1] = lst_insere(adj_list[1],0,12,0,3);
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

    //lst_imprime(adj_list[T]);

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
    Lista* L;

    //Lista auxiliar
    Lista* l;

    //Para busca em profundidade no grafo
    Pilha* p;

    //Define fluxo total que se deseja alcancar
    int R = TARGET;
    //Variavel que acumula o fluxo acumulado a cada iteracao
    int r = 0;
    //Variavel que armazena o fluxo maximo possivel no caminho atual
    int f;
    //Variavel auxiliar de fluxo
    int f_aux;
    //Variavel auxiliar de sentido
    int s_aux = -1;
    //Variaveis auxiliares do algoritmo guloso
    int c_max = MAX_CAP; //Capacidade maxima a cada iteracao
    int id_sel; //Id selecionado a cada iteracao
    int* nei_gula; //Vetor dinamico para armazenar ids dos nos ja inseridos na pilha do guloso
    int control_gula = 0; //Variavel de controle do guloso
    int control_gula2 = 0; //Variavel de controle do guloso

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
        minutos = ((end.tv_sec - start.tv_sec)/60) - horas*60;
        segundos = (end.tv_sec - start.tv_sec) - minutos*60;
        printf("Tempo de execucao decorrido aprox: %d:%d:%ld\n", horas,minutos,segundos);
        //Se PV estourou o limite, significa que a pilha esvaziou muitas vezes durante as buscas. Possivelmente os caminhos ja estao bastante cheios. Encerra o programa.
        if(PV > PV_MAX)
            break;
        //Inicializa fluxo da iteracao atual
        f = MAX_CAP;
        l = lst_cria();
        p = pilha_cria();

        //Inicia sempre pelo S, logo, ativa o primeiro no.
        for(i = 0; i<N;i++){
            V[i] = 0;
        }
        V[0] = 1;

        //Recria lista L a cada iteracao/novo caminho a percorrer, iniciando sempre pelo S.
        L = lst_cria();
        L = lst_insere(L,0,0,-1,0); //O primeiro sentido eh -1 apenas para destacar

        //FORCA BRUTA
        if(METHOD == 1){
            //Inicializa pilha com vizinhos de S (o no inicial)
            for(l = adj_list[S]; l != NULL; l = l->prox){
                pilha_push(p,l->id);
            }
            //Embaralha os vizinhos de S (GARANTE A ALEATORIEDADE NO METODO BRUTE FORCE)
            if(nei[L->id] > 1)
                pilha_shuffle(p,nei[L->id]);
        }
        //GULOSO
        else if(METHOD == 2){
            nei_gula = (int*) malloc(nei[S]*sizeof(int));
            nei_gula[0] = -100; //controle
            //Insere os vizinhos de S na pilha em ordem crescente de capacidade de fluxo disponivel
            for(i = 0; i < nei[S]; i++){
                c_max = MAX_CAP;
                control_gula2 = 0;
                for(l = adj_list[S]; l != NULL; l = l->prox){

                    control_gula = 0;
                    
                    //Nao executa na primeira iteracao pois nei_gula nao esta preenchido
                    if(nei_gula[0] != -100){
                        for(j = 0; j < nei[S]; j++){
                            if(l->id == nei_gula[j]){
                                control_gula = 1;
                                break;
                            }
                        }

                        if(control_gula == 1)
                            continue;
                    }

                    if((l->f_max-l->f_atual) < c_max && l->s == 1){
                        c_max = (l->f_max-l->f_atual);
                        id_sel = l->id;
                        control_gula2 = 1;
                    }
                    else if(l->f_atual > 0 && l->f_atual < c_max && l->s == 0){
                        c_max = l->f_atual;
                        id_sel = l->id;
                        control_gula2 = 1;
                    }
                }
                if(pilha_vazia(p) && control_gula2){
                    pilha_push(p,id_sel);
                    //Marca ID no vetor do guloso para que nao seja mais selecionado
                    nei_gula[i] = id_sel;
                }
                else if(control_gula2){
                    pilha_push(p,id_sel);
                    //Marca ID no vetor do guloso para que nao seja mais selecionado
                    nei_gula[i] = id_sel;
                }
            }

            //Libera nei_gula
            free(nei_gula);
        }

        control = 0;
        nei_pop = 0;

        //Enquanto nao visita um dos extremos da aresta A
        while((V[A_1] || V[A_2]) == 0){
            gettimeofday(&end, NULL);
            horas = ((end.tv_sec - start.tv_sec)/3600);
            minutos = ((end.tv_sec - start.tv_sec)/60) - horas*60;
            segundos = (end.tv_sec - start.tv_sec) - minutos*60;
            
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
                    //Se visitou todos os vizinhos do no atual, retorna para o elemento anterior no caminho (backtracking)
                    if(nei_pop == nei[L->id]){
                        L = L->prox;
                        nei_pop = 0;
                    }
                    continue;
                }
                //Se o caminho eh viavel, atualiza fluxo maximo do caminho atual, e atualiza V, L e p
                f = f_aux;
                l = lst_cria();
                //Pega o sentido do fluxo para armazenar no caminho L
                for(l = adj_list[L->id]; l!=NULL; l=l->prox){
                    if(l->id == p->prim->id){
                        s_aux = l->s;
                        break;
                    }
                }
                L = lst_insere(L,0,0,s_aux,p->prim->id);
                V[L->id] = 1;
                //Desempilha o topo e atualiza vizinhos do novo no na pilha p
                pilha_pop(p);
                //FORCA BRUTA
                if(METHOD == 1){
                    l = lst_cria();
                    for(l = adj_list[L->id]; l != NULL; l = l->prox){
                        pilha_push(p,l->id);
                    }
                    //Embaralha os vizinhos inseridos na pilha (GARANTE A ALEATORIEDADE NO METODO BRUTE FORCE)
                    if(nei[L->id] > 1)
                        pilha_shuffle(p,nei[L->id]);
                }
                //GULOSO
                else if(METHOD == 2){
                    nei_gula = (int*) malloc(nei[L->id]*sizeof(int));
                    nei_gula[0] = -100; //controle
                    //Insere os vizinhos do novo no na pilha em ordem crescente de capacidade de fluxo disponivel
                    for(i = 0; i < nei[L->id]; i++){
                        c_max = MAX_CAP;
                        control_gula2 = 0;
                        for(l = adj_list[L->id]; l != NULL; l = l->prox){

                            control_gula = 0;
                            
                            //Nao executa na primeira iteracao pois nei_gula nao esta preenchido
                            if(nei_gula[0] != -100){
                                for(j = 0; j < nei[L->id]; j++){
                                    if(l->id == nei_gula[j] || V[l->id] == 1){
                                        control_gula = 1;
                                        break;
                                    }
                                }

                                if(control_gula == 1)
                                    continue;
                            }

                            if((l->f_max-l->f_atual) < c_max && l->s == 1 && V[l->id] != 1){
                                c_max = (l->f_max-l->f_atual);
                                id_sel = l->id;
                                control_gula2 = 1;
                            }
                            else if(l->f_atual > 0 && l->f_atual < c_max && l->s == 0 && V[l->id] != 1){
                                c_max = l->f_atual;
                                id_sel = l->id;
                                control_gula2 = 1;
                            }
                        }
                        if(pilha_vazia(p) && control_gula2){
                            pilha_push(p,id_sel);
                            //Marca ID no vetor do guloso para que nao seja mais selecionado
                            nei_gula[i] = id_sel;
                        }
                        else if(control_gula2){
                            pilha_push(p,id_sel);
                            //Marca ID no vetor do guloso para que nao seja mais selecionado
                            nei_gula[i] = id_sel;
                        }
                    }

                    //Libera nei_gula
                    free(nei_gula);
                }
            }
            else{
                pilha_pop(p);
                // //Verifica se ainda existem vizinhos a visitar antes de continuar o loop
                nei_pop++;
                //Se visitou todos os vizinhos do no atual, retorna para o elemento anterior no caminho (backtracking)
                if(nei_pop == nei[L->id]){
                    L = L->prox;
                    nei_pop = 0;
                }
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
                L = lst_insere(L,0,0,1,A_2);
                V[A_2] = 1;
                //Desempiha
                pilha_pop(p);
            }
            
            //Se flui no sentido inverso da aresta A
            if(sent_A == 0){
                L = lst_insere(L,0,0,0,A_1);
                V[A_1] = 1;
                //Desempiha
                pilha_pop(p);
            }

            //FORCA BRUTA
            if(METHOD == 1){
                //Pega os novos vizinhos do no destino (que ainda nao foram visitados)
                l = lst_cria();
                for(l = adj_list[L->id]; l != NULL; l = l->prox){
                    pilha_push(p,l->id);
                }
                //Embaralha os vizinhos inseridos na pilha (GARANTE A ALEATORIEDADE NO METODO BRUTE FORCE)
                if(nei[L->id] > 1)
                    pilha_shuffle(p,nei[L->id]);
            }
            //GULOSO
            else if (METHOD == 2){
                nei_gula = (int*) malloc(nei[L->id]*sizeof(int));
                nei_gula[0] = -100;
                    //Insere os vizinhos do novo no na pilha em ordem crescente de capacidade de fluxo disponivel
                    for(i = 0; i < nei[L->id]; i++){
                        c_max = MAX_CAP;
                        control_gula2 = 0;
                        for(l = adj_list[L->id]; l != NULL; l = l->prox){

                            control_gula = 0;
                            
                            //Nao executa na primeira iteracao pois nei_gula nao esta preenchido
                            if(nei_gula[0] != -100){
                                for(j = 0; j < nei[L->id]; j++){
                                    if(l->id == nei_gula[j] || V[l->id] == 1){
                                        control_gula = 1;
                                        break;
                                    }
                                }

                                if(control_gula == 1)
                                    continue;
                            }

                            if((l->f_max-l->f_atual) < c_max && l->s == 1 && V[l->id] != 1){
                                c_max = (l->f_max-l->f_atual);
                                id_sel = l->id;
                                control_gula2 = 1;
                            }
                            else if(l->f_atual > 0 && l->f_atual < c_max && l->s == 0 && V[l->id] != 1){
                                c_max = l->f_atual;
                                id_sel = l->id;
                                control_gula2 = 1;
                            }
                        }
                        if(pilha_vazia(p) && control_gula2){
                            pilha_push(p,id_sel);
                            //Marca ID no vetor do guloso para que nao seja mais selecionado
                            nei_gula[i] = id_sel;
                        }
                        else if(control_gula2){
                            pilha_push(p,id_sel);
                            //Marca ID no vetor do guloso para que nao seja mais selecionado
                            nei_gula[i] = id_sel;
                        }
                    }

                    //Libera nei_gula
                    free(nei_gula);
                }
            
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
                        if(nei_pop == nei[L->id]){
                            //Limita o backtracking ate uma das extremidades da aresta A
                            if(L->id == A_1 || L->id == A_2){
                                control = 1;
                                break;
                            }
                            //Se visitou todos os vizinhos do no atual, retorna para o elemento anterior no caminho (backtracking)
                            L = L->prox;
                            nei_pop = 0;
                        }
                        continue;
                    }
                    //Se o caminho eh viavel, atualiza fluxo maximo do caminho atual, e atualiza V, L e p
                    f = f_aux;
                    l = lst_cria();
                    //Pega o sentido do fluxo para armazenar no caminho L
                    for(l = adj_list[L->id]; l!=NULL; l=l->prox){
                        if(l->id == p->prim->id){
                            s_aux = l->s;
                            break;
                        }
                    }
                    L = lst_insere(L,0,0,s_aux,p->prim->id);
                    V[L->id] = 1;
                    //Desempilha o topo e atualiza vizinhos do novo no na pilha p
                    pilha_pop(p);
                    
                    //Se chegou em T, nem precisa atualizar os vizinhos pois nao havera proxima iteracao
                    if(L->id != T){
                        //FORCA BRUTA
                        if(METHOD == 1){
                            l = lst_cria();
                            for(l = adj_list[L->id]; l != NULL; l = l->prox){
                                pilha_push(p,l->id);
                            }
                            //Embaralha os vizinhos inseridos na pilha (GARANTE A ALEATORIEDADE NO METODO BRUTE FORCE)
                            if(nei[L->id] > 1)
                                pilha_shuffle(p,nei[L->id]);
                        }
                        //GULOSO
                        else if (METHOD == 2){
                            nei_gula = (int*) malloc(nei[L->id]*sizeof(int));
                            nei_gula[0] = -100; //controle
                            //Insere os vizinhos do novo no na pilha em ordem crescente de capacidade de fluxo disponivel
                            for(i = 0; i < nei[L->id]; i++){
                                c_max = MAX_CAP;
                                control_gula2 = 0;
                                for(l = adj_list[L->id]; l != NULL; l = l->prox){

                                    control_gula = 0;
                                        
                                    //Nao executa na primeira iteracao pois nei_gula nao esta preenchido
                                    if(nei_gula[0] != -100){
                                        for(j = 0; j < nei[L->id]; j++){
                                            if(l->id == nei_gula[j] || V[l->id] == 1){
                                                control_gula = 1;
                                                break;
                                            }
                                        }

                                        if(control_gula == 1)
                                            continue;
                                    }

                                    if((l->f_max-l->f_atual) < c_max && l->s == 1 && V[l->id] != 1){
                                        c_max = (l->f_max-l->f_atual);
                                        id_sel = l->id;
                                        control_gula2 = 1;
                                    }
                                    else if(l->f_atual > 0 && l->f_atual < c_max && l->s == 0 && V[l->id] != 1){
                                        c_max = l->f_atual;
                                        id_sel = l->id;
                                        control_gula2 = 1;
                                    }
                                }
                                if(pilha_vazia(p) && control_gula2){
                                    pilha_push(p,id_sel);
                                    //Marca ID no vetor do guloso para que nao seja mais selecionado
                                    nei_gula[i] = id_sel;
                                }
                                else if(control_gula2){
                                    pilha_push(p,id_sel);
                                    //Marca ID no vetor do guloso para que nao seja mais selecionado
                                    nei_gula[i] = id_sel;
                                }
                            }

                            //Libera nei_gula
                            free(nei_gula);
                        }
                    }
                }
                else{
                    pilha_pop(p);
                    //Verifica se ainda existem vizinhos a visitar antes de continuar o loop
                    nei_pop++;
                    //Se visitou todos os vizinhos do no atual, retorna para o elemento anterior no caminho (backtracking)
                    if(nei_pop == nei[L->id]){
                        L = L->prox;
                        nei_pop = 0;
                    }
                        
                }
            }
        }

        //Apos percorrer o grafo, se nao incorreu em nenhuma condicao de parada:
        //Checa se variavel de controle eh 0, o que significa que nao achou T antes de A_1 ou A_2, e que nao incorreu no limite de backtracking apos aresta A.
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

            }
        }

    }
    printf("FIM DO ALGORITMO\n---------\n\n");

    printf("RESPOSTA DO PROBLEMA:\n\n");

    if(r >= R){
        printf("Sim, eh possivel passar fluxo minimo %d pela aresta A (%d - %d), partindo de S e chegando em T.\n\n\n", R, A_1, A_2);
    }
    else if(minutos >= MAX_EXEC_TIME){
        printf("Tempo limite de execucao excedido (%d minutos).\n", MAX_EXEC_TIME);
        printf("Nao foi possivel determinar a resposta do problema (assume-se negativa).\n");
        printf("Fluxo maximo alcancado: %d.\n",r);
        printf("Quantidade de esvaziamentos de pilha nas buscas: %d.\n\n\n",PV);
    }
    else if(PV > PV_MAX){
        printf("A pilha esvaziou no minimo %d vezes, o que implica que o algoritmo teve dificuldades em encontrar caminhos viaveis antes de ser capaz de transmitir fluxo minimo R de S a T passando por A.\nLogo, provavelmente a resposta para o problema eh negativa.\n", PV_MAX);
        printf("Fluxo maximo alcancado: %d.\n\n\n",r);
    }

    printf("Tempo de execucao total: %d:%d:%ld\n\n\n", horas, minutos, segundos);

    lst_libera(L);
    pilha_libera(p);
    
    for(i = 0; i < N; i++){
        lst_libera(adj_list[i]);
    }
}