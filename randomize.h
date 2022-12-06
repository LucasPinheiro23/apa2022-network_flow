#ifndef RANDOMIZE_H
#define RANDOMIZE_H
#include <stdlib.h>
#include <sys/time.h>

//Funcao para gerar numero inteiro aleatorio de 1 ate max
int randomize(int max)
{
    srand(time(NULL));
    return (rand() % max) + 1;
}

/* Arruma os N elementos de um vetor em ordem aleatoria.
   So eh efetivo quando N eh muito menor que RAND_MAX.*/
void shuffle(int* array, int n)
{
    srand(time(NULL));
    if (n > 1) 
    {
        int i;
        for (i = 0; i < n - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }

}

#endif