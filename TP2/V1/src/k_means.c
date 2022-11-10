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
    // Allows us to associate this point with a certain cluster
    // It's equivalent to the pos of a certain centroid
    int nCluster;
};

// Array with all the points of this program
// struct point *array_points;
// Array with all the centroids
// struct point *array_centroids;

// Function where the array of points and the array of centroids are populated:
// O(N+K)
void init(struct point *allpoints, struct point *allcentroids)
{
    srand(10);
    // Add random values to all the points in the array
    int i;
    for (i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        allpoints[i].nCluster = 0;
    }

    // Assign the first four point to a centroid:
    int j;
    for (j = 0; j < K; j++)
    {
        allcentroids[j].x = allpoints[j].x;
        allcentroids[j].y = allpoints[j].y;
    }
}

// Function where we calculate the distance between a point an a certain centroid:
//  O(1)
float determineDistance(struct point p, struct point centroid)
{
    float diff_x = (p.x) - (centroid.x);
    float diff_y = (p.y) - (centroid.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
// O(N*K)
int update_cluster_points(struct point *allpoints, struct point *allcentroids)
{
    int points_changed = 0; // counts the number of points that changed cluster
    float diff_temp;

    // For each point, verifies if the point should change cluster

    int i;

    //#pragma omp parallel num_threads(N_THREADS)
    {
        //#pragma omp for
        for (i = 0; i < N; i++)
        {
            // calculates the distance between a certain point and all the centroids
            float arr[K];

            int j;
            for (j = 0; j < K; j++)
            {
                // calculates the distance
                arr[j] = determineDistance(allpoints[i], allcentroids[j]);
            }

            float diff = arr[0];
            int newCluster = 0;
            for (j = 1; j < K; j++)
            {
                // verifies if it should change cluster or not:
                if (arr[j] < diff)
                {
                    diff = arr[j];
                    newCluster = j;
                }
            }

            // If the current cluster is not the one to which the point is closer to, then point changes cluster
            if (allpoints[i].nCluster != newCluster)
            {
                allpoints[i].nCluster = newCluster;
                points_changed++;
            }
        }
    }

    return points_changed;
}

// Calculates the mean of the coordenates of all the points that are in a cluster
// O(2*K + N)
void determine_new_centroid(int size[K], struct point *allpoints, struct point *allcentroids)
{
    // change the coordenates of all the centroids to (0,0)

    int i;

    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = 0;
        allcentroids[i].y = 0;
        size[i] = 0;
    }
    // Tratar data race
    // For each point, add its coordinates to the coordinates of a certain center

    struct point aux;
    aux.x = 0;
    aux.y = 0;

    for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i].nCluster;
        aux.x = allpoints[i].x;
        aux.y = allpoints[i].y;
        // allcentroids[nCluster].x += allpoints[i].x; // data race
        // allcentroids[nCluster].y += allpoints[i].y; // data race
        // data race
        {
            allcentroids[nCluster].x += aux.x;
            allcentroids[nCluster].y += aux.y;
            size[nCluster]++;
        }
    }
// Calculates the mean of the coordinates of all the centroids
#pragma omp parallel num_threads(N_THREADS)
    {

#pragma omp for
        for (i = 0; i < K; i++)
        {
            allcentroids[i].x = allcentroids[i].x / size[i];
            allcentroids[i].y = allcentroids[i].y / size[i];
        }
    }
    return;
}

void main(int argc, char *argv[])
{
    if (argc == 4)
    {
        // Número de Pontos
        N = atoi(argv[1]);
        // Número de Clusters
        K = atoi(argv[2]);
        // Número de threads
        N_THREADS = atoi(argv[3]);
    }
    else if (argc == 3)
    {
        // Número de Pontos
        N = atoi(argv[1]);
        // Número de Clusters
        K = atoi(argv[2]);
        // Número de Threads
        N_THREADS = 1;
    }

    // Array with all the points of this program
    struct point *array_points = malloc(sizeof(struct point) * N);
    // Array with all the centroids
    struct point *array_centroids = malloc(sizeof(struct point) * K); //[K]; //

    clock_t start, end;
    start = clock();

    int points_changed;
    int lenClusters[K];

    init(array_points, array_centroids);

    points_changed = update_cluster_points(array_points, array_centroids);

    int nIterations = 0;
    do
    {
        points_changed = update_cluster_points(array_points, array_centroids);
        determine_new_centroid(lenClusters, array_points, array_centroids);
        nIterations++;
    } while (nIterations != 21);

    end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    printf("%f\n", timeSpent);

    free(array_centroids);
    free(array_points);

    return;
}