#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

void free_array_points(Point *array_points)
{
    int i = 0;
    for (i = 0; i < N; i++)
    {
        free(array_points[i]);
    }
    free(array_points);
}

void free_array_centroids(Point *array_centroids)
{
    int i = 0;
    for (i = 0; i < K; i++)
    {
        free(array_centroids[i]);
    }
    free(array_centroids);
}

// Function where the array of points and the array of centroids are populated:
void init(Point *array_points, Point *array_centroids)
{
    // Create new points:
    int i;
    for (i = 0; i < N; i++)
    {
        array_points[i] = (Point)malloc(sizeof(struct point));
        array_points[i]->x = (float)rand() / RAND_MAX;
        array_points[i]->y = (float)rand() / RAND_MAX;
        array_points[i]->nCluster = -1;
    }

    // Assign each point to a cluster:
    int j;
    for (j = 0; j < K; j++)
    {
        array_centroids[j] = (Point)malloc(sizeof(struct point));
        array_centroids[j]->x = array_points[j]->x;
        array_centroids[j]->y = array_points[j]->y;
        array_centroids[j]->nCluster = -1;
    }
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
    int i;
    for (i = 0; i < N; i++)
    {
        int newCluster;
        float diff = 100;

        // calcula a distância para cada um dos centroids
        int j;
        for (j = 0; j < K; j++)
        {
            // determina a distância entre cada ponto e cada cluster
            diff_temp = determineDistance(allpoints[i], allcentroids[j]);
            // verifica se deve mudar o número do cluster ou não...
            if (diff_temp < diff)
            {
                diff = diff_temp;
                newCluster = j;
            }
        }

        if (allpoints[i]->nCluster != newCluster)
        {
            allpoints[i]->nCluster = newCluster;
            points_changed++;
        }
    }
    return points_changed;
}

void determine_new_centroid(int size[4], Point *allpoints, Point *allcentroids, Cluster *c)
{
    int i;
    for (i = 0; i < K; i++)
    {
        allcentroids[i]->x = 0;
        allcentroids[i]->y = 0;
        size[i] = 0;
        // printPoint(allcentroids[i]);
    }
    for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i]->nCluster;
        allcentroids[nCluster]->x += allpoints[i]->x;
        allcentroids[nCluster]->y += allpoints[i]->y;
        size[nCluster]++;
    }

    for (i = 0; i < K; i++)
    {
        allcentroids[i]->x = allcentroids[i]->x / size[i];
        allcentroids[i]->y = allcentroids[i]->y / size[i];
    }
    return;
}

void main()
{
    clock_t start, end;
    start = clock();

    Cluster clusters[K];
    int points_changed;

    Point *array_points = createArrayPoints();
    Point *centroids = createArrayCentroids();
    init(array_points, centroids);

    points_changed = update_cluster_points(array_points, centroids, clusters);
    int nIterations = 0;
    int lenClusters[4];
    do
    {
        nIterations++;
        determine_new_centroid(lenClusters, array_points, centroids, clusters);
        points_changed = update_cluster_points(array_points, centroids, clusters);

    } while (points_changed != 0);

    end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", centroids[i]->x, centroids[i]->y, lenClusters[i]);
    }
    printf("%f\n", timeSpent);

    free_array_centroids(centroids);
    free_array_points(array_points);

    // gcc -O2 kmeans.c -o kmeans -lm
}