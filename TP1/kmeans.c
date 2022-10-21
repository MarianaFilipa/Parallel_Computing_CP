#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
} * Point;

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

// Function that allows use to print out the value of a Point:
void printPoint(Point p)
{
    printf("x = %f / y = %f / nCluster = %d \n", p->x, p->y, p->nCluster);
}

// Function that returns a new array of Points:
Point *createArrayPoints(void)
{
    Point *array_points = malloc(sizeof(Point) * N);
    return array_points;
}

// Function that returns a new array of Centroids:
Point *createArrayCentroids(void)
{
    Point *array_centroids = malloc(sizeof(Point) * K);
    return array_centroids;
}

// Function where the array of points and the array of centroids are populated:
void init(Point *array_points, Point *array_centroids)
{
    // Create new points:
    for (int i = 0; i < N; i++)
    {
        array_points[i] = (Point)malloc(sizeof(struct point));
        array_points[i]->x = (float)rand() / RAND_MAX;
        array_points[i]->y = (float)rand() / RAND_MAX;
        array_points[i]->nCluster = -1;
    }

    // Assign each point to a cluster:
    for (int j = 0; j < K; j++)
    {
        array_centroids[j] = (Point)malloc(sizeof(struct point));
        array_centroids[j]->x = array_points[j]->x;
        array_centroids[j]->y = array_points[j]->y;
        array_centroids[j]->nCluster = -1;
    }
}

void determine_new_centroid(Cluster c)
{
    printf("Hello");
}

float determineDistance(Point p, Point centroid)
{
    float diff_x = (p->x) - (centroid->x);
    float diff_y = (p->y) - (centroid->y);
    float aux = pow(diff_x, 2) + pow(diff_y, 2);

    return sqrtf(aux);
}

// Vai dar raia, não dá para saber em que cluster estava:
int update_cluster_points(Point *allpoints, Point *allcentroids, Cluster *clusters)
{
    int points_changed = 0;
    float diff_temp;
    for (int i = 0; i < N; i++)
    {

        int current_cluster = allpoints[i]->nCluster;
        float diff;
        if (current_cluster == -1)
        {
            diff = 100;
        }
        else
        {
            diff = determineDistance(allpoints[i], allcentroids[current_cluster]);
            printf("%f\n", diff);
        }

        // calcula a distância para cada um dos centroids
        for (int j = 0; j < K; j++)
        {
            // determina a distância entre cada ponto e cada cluster
            diff_temp = determineDistance(allpoints[i], allcentroids[j]);
            // verifica se deve mudar o número do cluster ou não...
            if (diff_temp < diff && j != current_cluster)
            {
                diff = diff_temp;
                points_changed++;
                allpoints[i]->nCluster = j;
            }
        }
    }
    return points_changed;
}

void main()
{
    Cluster clusters[K];
    int points_changed;

    Point *array_points = createArrayPoints();
    Point *centroids = createArrayCentroids();
    init(array_points, centroids);

    points_changed = update_cluster_points(array_points, centroids, clusters);
    printf("%d\n", points_changed);

    for (int i = 0; i < 4; i++)
    {
        printf("Centroid: ");
        printPoint(centroids[i]);
    }
    for (int i = 0; i < 8; i++)
    {
        printf("Point: ");
        printPoint(array_points[i]);
    }

    do
    {
        points_changed = 0;
        for (int i = 0; i < K; i++)
        {
            // determine_new_centroid(clusters[i]);
        }

        // points_changed = update_cluster_points(array_points, centroids, clusters);

    } while (points_changed != 0);

    // gcc -O2 kmeans.c -o kmeans -lm
}