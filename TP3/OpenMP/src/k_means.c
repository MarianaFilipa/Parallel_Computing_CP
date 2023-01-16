#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int N = 10000000;
int K = 4;
int N_THREADS = 4;

struct point
{
    float x;
    float y;
    int nCluster;
};

struct centroid
{
    float x;
    float y;
};

// Function where the array of points and the array of centroids are populated:
void init(struct point allpoints[N], struct centroid allcentroids[K])
{
    srand(10);
    for (int i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        allpoints[i].nCluster = -1;
    }
    for (int j = 0; j < K; j++)
    {
        allcentroids[j].x = allpoints[j].x;
        allcentroids[j].y = allpoints[j].y;
    }
}

float determineDistance(struct point p, struct centroid c)
{
    float diff_x = (p.x) - (c.x);
    float diff_y = (p.y) - (c.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

/* Será que o ganho em tempo/speed up compensa ter este bocado de código paralelizado*/
void initClusters(float *SumX, float *SumY, int *size)
{
    /*#pragma omp single
        {
            for (int i = 0; i < K; i++)
            {
    #pragma omp task
                SumX[i] = 0;
    #pragma omp task
                SumY[i] = 0;
    #pragma omp task
                size[i] = 0;
            }
    #pragma omp taskwait
        }*/

#pragma omp for // schedule(dynamic)
    for (int i = 0; i < K; i++)
    {
        SumX[i] = 0;
        SumY[i] = 0;
        size[i] = 0;
    }
#pragma omp barrier
}

void divideSums(float *SumX, float *SumY, int *size, int *sizeAux, struct centroid *allcentroids)
{
#pragma omp for simd
    // Calculates the mean of the coordinates of all the centroids
    for (int i = 0; i < K; i++)
    {
        SumX[i] = SumX[i] / sizeAux[i];
        SumY[i] = SumY[i] / sizeAux[i];
    }
#pragma omp barrier
#pragma omp for
    for (int i = 0; i < K; i++)
    {
        size[i] = sizeAux[i];
        allcentroids[i].x = SumX[i];
        allcentroids[i].y = SumY[i];
    }

/*#pragma omp single
    {
        for (int i = 0; i < K; i++)
        {
#pragma omp task
            {
                allcentroids[i].x = SumX[i] / sizeAux[i];
            }
#pragma omp task
            {
                allcentroids[i].y = SumY[i] / sizeAux[i];
            }
#pragma omp task
            {
                size[i] = sizeAux[i];
            }
        }
#pragma omp taskwait
    }*/
#pragma omp barrier
}

// Poderá haver misses
void addValues(float *SumX, float *SumY, int *size, int nCluster, float x, float y)
{
    SumX[nCluster] += x;
    SumY[nCluster] += y;
    size[nCluster]++;
}

// Function where we determine if a point should change the cluster it is currently assign to
int update_cluster_points(struct point *allpoints, struct centroid *allcentroids, int *size)
{
    // Variáveis Partilhadas
    // Cuidado com data races para estes arrays
    float SumX[K], SumY[K];
    int sizeAux[K];
    int points_changed = 0;

#pragma omp parallel num_threads(N_THREADS)
    {
        // Variáveis privadas para cada thread
        float diff_temp, diff;
        int newCluster = -1;

        initClusters(SumX, SumY, sizeAux); // Inicializa os valores dos arrays partilhados a 0.
                                           //  Espera que todos os valores dos arrays partilhados estejam a 0

#pragma omp for reduction(+ \
                          : SumX, SumY, sizeAux, points_changed) // Atribui a cada thread um conjunto de iterações do ciclo for
        for (int i = 0; i < N; i++)
        {
            diff = 100;

            for (int j = 0; j < K; j++) // calculates the distance between a certain point and all the centroids
            {
                diff_temp = determineDistance(allpoints[i], allcentroids[j]); // calculates the distance

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
            // Cada thread faz esta soma nas suas cópias
            addValues(SumX, SumY, sizeAux, newCluster, allpoints[i].x, allpoints[i].y);
        } // Implicit barrier after for
#pragma omp barrier
        divideSums(SumX, SumY, size, sizeAux, allcentroids);
    }

    return points_changed;
}

int kmeans(int *lenClusters, struct point *array_points, struct centroid *array_centroids)
{
    int points_changed;
    int nIterations = 0;

    init(array_points, array_centroids);

    do
    {
        update_cluster_points(array_points, array_centroids, lenClusters);
        nIterations++;
    } while (nIterations != 21);

    return nIterations;
}

int main(int argc, char *argv[])
{
    if (argc == 4)
    {
        N = atoi(argv[1]);         // Número de Pontos
        K = atoi(argv[2]);         // Número de Clusters
        N_THREADS = atoi(argv[3]); // Número de Threads
    }
    else if (argc == 3)
    {
        N = atoi(argv[1]); // Número de Pontos
        K = atoi(argv[2]); // Número de Clusters
        N_THREADS = 1;     // Número de Threads
    }

    struct point *array_points = malloc(sizeof(struct point) * N);          // Array with all the points of this program
    struct centroid *array_centroids = malloc(sizeof(struct centroid) * K); // Array with all the centroids
    int *lenClusters = malloc(sizeof(int) * K);

    int nIterations = kmeans(lenClusters, array_points, array_centroids);

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    printf("Iterations: %d\n", nIterations - 1);
    free(array_points);
    free(array_centroids);
    free(lenClusters);
    return 0;
}
