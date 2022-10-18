#include <stdio.h>
#include <stdlib.h>

// Criar amostras e iniciar os clusters -> algoritmo enunciado

// Calcular o centroide (ou centro geométrico) de cada "cluster"

// Atribuir cada amostra ao "cluster" mais próximo usando a distância euclidiana

// Repetir até não haverem mais pontos a mudar de cluster.

/*
Questões:
1. Como contabilizar se há pontos a mudar de sitio
2. Como guardar os "clusters"
3. Como guardar as "amostras"
4. Como guardar o centroide
5. Calcular distância

*/

#define N 10000000;
#define K 4;

struct Point
{
    float x;
    float y;
} Point;

struct Cluster
{
    Point center;
    Point values[N]; // Demasiados valores
    int size;
} Cluster;

/*

void inicializa(...) {
    ...
    srand(10);
    for(int i = 0; i < numero_de_pontos; i++) {
        <<ponto_i_coordenada_x>> = (float) rand() / RAND_MAX;
        <<ponto_i_coordenada_y>> = (float) rand() / RAND_MAX;
    }
    for(int i = 0; i < numero_de_clusters; i++) {
        <<cluster_i_coordenada_x>> = <<ponto_i_coordenada_x>>
        <<cluster_i_coordenada_y>> = <<ponto_i_coordenada_y>>
    }
}

int main (){
    int points_changed;
    clusters = inicializa(); //Já temos 4 * (conjuntos de pontos e um centroide)

    do{
        points_changed = 0;

        /calcular o centroide
        /mudar em função da distância

    }while(points_changed != 0);


}






*/