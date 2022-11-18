#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int N = 10000000;
int K = 4;
int N_THREADS = 1;

struct point
{
    float x;
    float y;
    int nCluster;
};

// Function where the array of points and the array of centroids are populated:
void init(struct point allpoints[N], struct point allcentroids[K])
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

float determineDistance(struct point p, struct point c)
{
    float diff_x = (p.x) - (c.x);
    float diff_y = (p.y) - (c.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
int update_cluster_points(struct point allpoints[N], struct point allcentroids[K])
{
    int points_changed = 0;

#pragma omp parallel num_threads(N_THREADS)
    {
        float diff_temp;
        int newCluster = -1;
        float diff;
#pragma omp for reduction(+ \
                          : points_changed)
        for (int i = 0; i < N; i++)
        {
            diff = 100;
            // calculates the distance between a certain point and all the centroids
            for (int j = 0; j < K; j++)
            {
                // calculates the distance
                diff_temp = determineDistance(allpoints[i], allcentroids[j]);

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
    }
    return points_changed;
}

/*void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{
    float SumX[K];
    float SumY[K];
    int sizeA[K];

#pragma omp parallel num_threads(N_THREADS)
    {
        for (int i = 0; i < K; i++)
        {
            SumX[i] = 0;
            SumY[i] = 0;
            sizeA[i] = 0;
        }
#pragma omp barrier
#pragma omp for reduction(+ \
                          : SumX, SumY, sizeA)
        for (int i = 0; i < N; i++)
        {
            int nCluster = allpoints[i].nCluster;
            SumX[nCluster] += allpoints[i].x;
            SumY[nCluster] += allpoints[i].y;
            sizeA[nCluster]++;
        }
    }
    // Calculates the mean of the coordinates of all the centroids
    for (int i = 0; i < K; i++)
    {
        allcentroids[i].x = SumX[i] / sizeA[i];
        allcentroids[i].y = SumY[i] / sizeA[i];
        size[i] = sizeA[i];
    }

    return;
}*/

void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{
    float SumX[K];
    float SumY[K];
    int sizeA[K];

#pragma omp parallel num_threads(N_THREADS)
    {
#pragma omp for
        for (int i = 0; i < K; i++)
        {
            SumX[i] = 0;
            SumY[i] = 0;
            sizeA[i] = 0;
        }
#pragma omp barrier
#pragma omp for reduction(+ \
                          : SumX, SumY, sizeA)
        for (int i = 0; i < N; i++)
        {
            int nCluster = allpoints[i].nCluster;
            SumX[nCluster] += allpoints[i].x;
            SumY[nCluster] += allpoints[i].y;
            sizeA[nCluster]++;
        }
#pragma omp barrier
#pragma omp for
        // Calculates the mean of the coordinates of all the centroids
        for (int i = 0; i < K; i++)
        {
            allcentroids[i].x = SumX[i] / sizeA[i];
            allcentroids[i].y = SumY[i] / sizeA[i];
            size[i] = sizeA[i];
        }
    }
    return;
}

int kmeans(int lenClusters[K], struct point array_points[N], struct point array_centroids[K])
{

    int points_changed;
    int nIterations = 0;

    init(array_points, array_centroids);
    points_changed = update_cluster_points(array_points, array_centroids);
    do
    {
        determine_new_centroid(lenClusters, array_points, array_centroids);
        points_changed = update_cluster_points(array_points, array_centroids);
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

    struct point *array_points = malloc(sizeof(struct point) * N);    // Array with all the points of this program
    struct point *array_centroids = malloc(sizeof(struct point) * K); // Array with all the centroids
    int *lenClusters = malloc(sizeof(int) * K);

    int nIterations = kmeans(lenClusters, array_points, array_centroids);

    printf("%d\n", nIterations - 1);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    free(array_points);
    free(array_centroids);
    free(lenClusters);
    return 0;
}