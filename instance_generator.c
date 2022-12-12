#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

#include "lista.h"
#include "randomize.h"

//Define a capacidade maxima possivel por aresta
#define F_MAX 15

//Junta dois arquivos
void appendFiles(char source[],
                 char destination[])
{
    // declaring file pointers
    FILE *fp1, *fp2;
 
    // opening files
    fp1 = fopen(source, "a+");
    fp2 = fopen(destination, "a+");
 
    // If file is not found then return.
    if (!fp1 && !fp2) {
        printf("Unable to open/"
               "detect file(s)\n");
        return;
    }
 
    char buf[100];
 
    // explicitly writing "\n"
    // to the destination file
    // so to enhance readability.
    // fprintf(fp2, "\n");
 
    // writing the contents of
    // source file to destination file.
    while (!feof(fp1)) {
        fgets(buf, sizeof(buf), fp1);
        fprintf(fp2, "%s", buf);
    }
 
    rewind(fp2);
 
    // printing contents of
    // destination file to stdout.
    while (!feof(fp2)) {
        fgets(buf, sizeof(buf), fp2);
    }
}


//Funcao para gerar as instancias dos grafos para utilizar no algoritmo de fluxo por caminho restrito.
//Gera instancia baseada em um numero predeterminado de nos
int inst_gen(int N, char* filename){

    //Para aferir o tempo de execução durante programa
    struct timeval start, end;
    long seq_seconds, rec_seconds;
    long seq_micros, rec_micros;

    // criando a variável ponteiro para o arquivo
    FILE* arq;

    char f[sizeof(filename)+4];
    strcpy(f,filename);
    strcat(f,".txt");
    arq = fopen(f,"w");

    //testando se o arquivo foi realmente criado
    if(arq == NULL)
    {
        printf("Erro na abertura do arquivo!");
        exit(-1);
    }

    srand(time(NULL));

    // int N; //Numero de nos da instancia
    int Mi; //Numero de arestas conectadas a determinado no i
    int Mi_total = 0; //Contador de numero de arestas do grafo
    int A1, A2; //Extremidades da aresta A
    int i, j; //Indices dos loops
    int T = 0; //Verifica se o ultimo no (indice N-1) ja foi alcancado

    // printf("Entre com o numero de nos da instancia: ");
    // scanf("%d", &N);

    //Escreve N na primeira linha do arquivo
    // fprintf(arq,"%d",N);
    // fprintf(arq,"%s","\n");

    Lista* adj_list[N]; //Lista de adjacencias que sera gerada
    int V[N]; //Vetor que indica se determinado no ja foi adicionado como vizinho de outro (para evitar multigrafo)
    int rand_id; //id gerado aleatoriamente para selecionar proximo no
    int rand_aux = -1;
    int aux;
    
    for(i = 0; i < N; i++){
        adj_list[i] = lst_cria();
        V[i] = 0;
    }

    for(i = 0; i < (N-1); i++){
        //Para cada no, gera um numero aleatorio de arestas entre 1 e 3. O no S deve ter no minimo 2 e no maximo 4.
        if(i==0)
            Mi = randomize(2,4);
        else
            Mi = randomize(1,3);

        Mi_total = Mi_total + Mi;
        j = 0;
        while(j < Mi){
            
            gettimeofday(&start, NULL);
            do{
                gettimeofday(&end, NULL);
                rand_id = randomize((i+1),(N-1));
            }while((V[rand_id] == 1) && ((end.tv_sec - start.tv_sec) <= 30));

            if((end.tv_sec - start.tv_sec) >= 30)
                return 0;

            aux = randomize(1,F_MAX);
            //Adiciona aresta do no i ao vizinho identificado com rand_id, com capacidade maxima aleatoria determinada por F_MAX
            adj_list[i] = lst_insere(adj_list[i],0,aux,1,rand_id);
            //Escreve no arquivo
            fprintf(arq, "%d", i); //ORIGEM
            fprintf(arq, "%s", ","); //Separador
            fprintf(arq, "%d", 0); //CAPACIDADE INICIAL
            fprintf(arq, "%s", ","); //Separador
            fprintf(arq, "%d", aux); //CAPACIDADE MAXIMA
            fprintf(arq, "%s", ","); //Separador
            fprintf(arq, "%d", 1); //SENTIDO
            fprintf(arq, "%s", ","); //Separador
            fprintf(arq, "%d", rand_id); //DESTINO
            fprintf(arq, "%s", "\n"); //Pula linha

            V[rand_id] = 1;
            j++;
            if(rand_id == (N-1))
                T = 1;

            if(randomize(0,1) && i != 0 && i != (N-1) && rand_id != (N-1))
                A1 = i;
                A2 = rand_id;
        }

        //Zera V para proximo elemento
        for(j = 0; j < N; j++){
            V[j] = 0;
        }
    }

    //Se adicionou todas as arestas, mas nao alcancou T, criar nova aresta do ultimo no visitado para T.
    if(T == 0){
        aux = randomize(1,F_MAX);
        adj_list[i] = lst_insere(adj_list[i],0,aux,1,(N-1));
        //Escreve no arquivo
        fprintf(arq, "%d", i); //ORIGEM
        fprintf(arq, "%s", ","); //Separador
        fprintf(arq, "%d", 0); //CAPACIDADE INICIAL
        fprintf(arq, "%s", ","); //Separador
        fprintf(arq, "%d", aux); //CAPACIDADE MAXIMA
        fprintf(arq, "%s", ","); //Separador
        fprintf(arq, "%d", 1); //SENTIDO
        fprintf(arq, "%s", ","); //Separador
        fprintf(arq, "%d", (N-1)); //DESTINO
        fprintf(arq, "%s", "\n"); //Pula linha
        Mi_total = Mi_total + 1;
    }

    // for(i = 0; i < N; i++){
    //     printf("No %d:\n",i);
    //     lst_imprime(adj_list[i]);
    //     printf("\n####\n");
    // }

    fclose(arq);

    // Arquivo auxiliar
    FILE* arq2;

    arq2 = fopen("aux.txt","w");

    fprintf(arq2,"%d",N);
    fprintf(arq2,"%s","\n");
    fprintf(arq2,"%d",Mi_total);
    fprintf(arq2,"%s","\n");
    fprintf(arq,"%d",A1);
    fprintf(arq,"%s","\n");
    fprintf(arq,"%d",A2);
    fprintf(arq,"%s","\n");

    fclose(arq2);

    appendFiles(f,"aux.txt");

    remove(f);
    rename("aux.txt",f);

    return 1;
}

//Permite gerar tantas instancias quanto especificado por argc
void main(int argc, char* argv[]){

    int Ni = 100; //Multiplicador
    int N = 1000; //N inicial
    int i, count = 0;
    char name[22];

    // time_t t;
    // struct tm *tm = localtime(&t);

    while(count <= 2){

        sprintf(name,"%d",N);

        if(inst_gen(N,name)){
            printf("Gerou instancia %d...",N);
            count = count++;
            N = N * Ni;
        }
        else{
            printf("Estouro de tempo\n");
        }
    }

}