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

struct point
{
    float x;
    float y;
    int nCluster;
};
// Size struct = 12

struct point array_points[N];
struct point array_centroids[K];

// Function that allows use to print out the value of a Point:
void printPoint(struct point p)
{
    printf("x = %f \n", p.x);
}

// Function that returns a new array of Points:
// O: 1
/*Point *createArrayPoints(void)
{
    Point *array_points = malloc(sizeof(Point) * N);
    return array_points;
}*/

/*void free_array_points(Point *array_points)
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

// O: 1
//  Function that returns a new array of Centroids:
Point *createArrayCentroids(void)
{
    Point *array_centroids = malloc(sizeof(Point) * K);
    return array_centroids;
}*/

// Function where the array of points and the array of centroids are populated:

// O = N+K;
void init(struct point allpoints[N], struct point allcentroids[K])
{
    // Create new points:
    int i;
    for (i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        allpoints[i].nCluster = -1;
    }

    // Assign each point to a cluster:
    int j;
    for (j = 0; j < K; j++)
    {
        allcentroids[j].x = allpoints[j].x;
        allcentroids[j].y = allpoints[j].y;
    }
}

// O = 1
float determineDistance(struct point p, struct point centroid)
{
    float diff_x = (p.x) - (centroid.x);
    float diff_y = (p.y) - (centroid.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// O = N*K
int update_cluster_points(struct point allpoints[N], struct point allcentroids[K])
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

        if (allpoints[i].nCluster != newCluster)
        {
            allpoints[i].nCluster = newCluster;
            points_changed++;
        }
    }
    return points_changed;
}

// O = 2*K + N
void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{
    int i;
    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = 0;
        allcentroids[i].y = 0;
        size[i] = 0;
    }
    for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i].nCluster;
        allcentroids[nCluster].x += allpoints[i].x;
        allcentroids[nCluster].y += allpoints[i].y;
        size[nCluster]++;
    }

    /*allcentroids[0]->x = allcentroids[0]->x / size[0];
    allcentroids[0]->y = allcentroids[0]->y / size[0];
    allcentroids[1]->x = allcentroids[1]->x / size[1];
    allcentroids[1]->y = allcentroids[1]->y / size[1];
    allcentroids[2]->x = allcentroids[2]->x / size[2];
    allcentroids[2]->y = allcentroids[2]->y / size[2];
    allcentroids[3]->x = allcentroids[3]->x / size[3];
    allcentroids[3]->y = allcentroids[3]->y / size[3];*/

    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = allcentroids[i].x / size[i];
        allcentroids[i].y = allcentroids[i].y / size[i];
    }
    return;
}

void main()
{
    clock_t start, end;
    start = clock();

    int points_changed = -1;

    // 1
    // float array_points[N]; //= createArrayPoints();
    // 1
    // Point centroids[K]; //= createArrayCentroids();
    // N+K

    init(array_points, array_centroids);
    int lenClusters[K];

    // N*K
    points_changed = update_cluster_points(array_points, array_centroids);

    // It = 39
    int nIterations = 0;
    do
    {
        // 2K+N
        determine_new_centroid(lenClusters, array_points, array_centroids);
        // N*K
        points_changed = update_cluster_points(array_points, array_centroids);
        nIterations++;
    } while (points_changed != 0);

    end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    // K
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    printf("%f\n", timeSpent);

    // free_array_centroids(centroids);
    // free_array_points(array_points);

    // gcc -O2 kmeans.c -o kmeans -lm
    return;
}

// Usar o -funroll-loops não justifica muito e aplicar a vetorização também não justificou muito