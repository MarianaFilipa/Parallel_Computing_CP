#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 10000000
#define K 4

typedef struct point
{
    float x;
    float y;
    int nCluster;
} * Point;

typedef struct centroid
{
    float x;
    float y;
} * Centroid;

// Function that returns a new array of Points:
// O: 1
Point *createArrayPoints(void)
{
    Point *array_points = malloc(sizeof(Point) * N);
    return array_points;
}

// O: 1
//  Function that returns a new array of Centroids:
Centroid *createArrayCentroids(void)
{
    Centroid *array_centroids = malloc(sizeof(Centroid) * K);
    return array_centroids;
}

// Function where the array of points and the array of centroids are populated:

// O = N+K;
void init(Point *array_points, Centroid *array_centroids)
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
        array_centroids[j] = (Centroid)malloc(sizeof(struct centroid));
        array_centroids[j]->x = array_points[j]->x;
        array_centroids[j]->y = array_points[j]->y;
    }
}

// O = 1
float determineDistance(Point p, Centroid centroid)
{
    float diff_x = (p->x) - (centroid->x);
    float diff_y = (p->y) - (centroid->y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// O = N*K
int update_cluster_points(Point *allpoints, Centroid *allcentroids)
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

// O = 2*K + N
void determine_new_centroid(int size[4], Point *allpoints, Centroid *allcentroids)
{
    int i;
    Centroid *aux = malloc(sizeof(Centroid) * 4);

    for (i = 0; i < K; i++)
    {
        aux[i] = malloc(sizeof(struct centroid));
        aux[i]->x = 0;
        aux[i]->y = 0;
        size[i] = 0;
    }
    for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i]->nCluster;
        aux[nCluster]->x += allpoints[i]->x;
        aux[nCluster]->y += allpoints[i]->y;
        size[nCluster]++;
    }

    for (i = 0; i < K; i++)
    {
        allcentroids[i]->x = aux[i]->x / size[i];
        allcentroids[i]->y = aux[i]->y / size[i];
    }
    return;
}

void main()
{
    // clock_t start, end;
    // start = clock();

    int points_changed = -1;

    // 1
    Point *array_points = createArrayPoints();
    // 1
    Centroid *centroids = createArrayCentroids();
    // N+K
    init(array_points, centroids);
    int lenClusters[4];

    // N*K
    points_changed = update_cluster_points(array_points, centroids);

    // It = 39
    int nIterations = 0;
    do
    {
        // 2K+N
        determine_new_centroid(lenClusters, array_points, centroids);
        // N*K
        points_changed = update_cluster_points(array_points, centroids);
        nIterations++;
    } while (points_changed != 0);

    // end = clock();
    // double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    // K
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", centroids[i]->x, centroids[i]->y, lenClusters[i]);
    }
    // printf("%f\n", timeSpent);

    // gcc -O2 kmeans.c -o kmeans -lm
}