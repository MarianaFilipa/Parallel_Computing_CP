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

#define N 10000000
#define K 4

typedef struct point
{
    float x;
    float y;
    int nCluster;
} Point;

typedef struct array_points
{
    Point arrays[N];
} Array_points;

typedef struct array_centroids
{
    Point centroids[K];
} Array_centroids;

typedef struct Cluster
{
    int *array_pos;
    int centroid; // Pos of centroid in the array
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
}*/

void init(Array_points *novo)
{
}

void determine_new_centroid(Cluster c)
{
    printf("Hello");
}

// Vai dar raia, não dá para saber em que cluster estava:
void update_cluster_points(Array_points *allpoints, Array_centroids *allcentroids, Cluster *clusters)
{
}

void main()
{
    Cluster clusters[K];
    int points_changed;

    Array_points *array;

    // clusters = inicializa(); // Já temos 4 * (conjuntos de pontos e um centroide)
    init(array);

        do
    {
        points_changed = 0;
        for (int i = 0; i < K; i++)
        {
            determine_new_centroid(clusters[i]);
        }

        // update_cluster_points();

    } while (points_changed != 0);
}