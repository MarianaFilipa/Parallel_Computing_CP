#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N = 10000000;
int K = 4;
int N_THREADS = 1;

struct point
{
    float x;
    float y;
};

// struct point array_points[N];
// int nCluster[N];
//  Function where the array of points and the array of centroids are populated:
//  O(N+K)
void init(struct point *allpoints, int *nCluster, float *centroid_X, float *centroid_Y)
{
    srand(10);
    // Add random values to all the points in the array
    int i;
    for (i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        nCluster[i] = 0;
    }

    // Assign the first four point to a centroid:
    int j;
    for (j = 0; j < K; j++)
    {
        centroid_X[j] = allpoints[j].x;
        centroid_Y[j] = allpoints[j].y;
    }
}

// Function where we calculate the distance between a point an a certain centroid:
//  O(1)
float determineDistance(struct point p, float centroid_X, float centroid_Y)
{
    float diff_x = (p.x) - (centroid_X);
    float diff_y = (p.y) - (centroid_Y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
// O(N*K)
int update_cluster_points(struct point *allpoints, int *nCluster, float *centroid_X, float *centroid_Y)
{
    int points_changed = 0; // counts the number of points that changed cluster
    float diff_temp;

    // For each point, verifies if the point should change cluster
    int i;
    for (i = 0; i < N; i++)
    {
        int newCluster;
        float diff = 100;

        // calculates the distance between a certain point and all the centroids
        int j;
        int arr[K];

        float dif_temp;

        for (j = 0; j < K; j++)
        {
            // calculates the distance
            dif_temp = determineDistance(allpoints[i], centroid_X[j], centroid_Y[j]);

            // verifies if it should change cluster or not:
            if (dif_temp < diff)
            {
                diff = dif_temp;
                newCluster = j;
            }
        }

        // If the current cluster is not the one to which the point is closer to, then point changes cluster
        if (nCluster[i] != newCluster)
        {
            nCluster[i] = newCluster;
            points_changed++;
        }
    }

    return points_changed;
}

// Calculates the mean of the coordenates of all the points that are in a cluster
// O(2*K + N)
void determine_new_centroid(int *size, struct point *allpoints, int *nClusters, float *centroid_X, float *centroid_Y)
{
    float SumX[K];
    float SumY[K];

    // change the coordenates of all the centroids to (0,0)
    int i;
    for (i = 0; i < K; i++)
    {
        SumX[i] = 0;
        SumY[i] = 0;
        size[i] = 0;
    }

    // For each point, add its coordinates to the coordinates of a certain center
    for (i = 0; i < N; i++)
    {
        int nCluster = nClusters[i];
        SumX[nCluster] += allpoints[i].x;
        SumY[nCluster] += allpoints[i].y;
        size[nCluster]++;
    }

    // Calculates the mean of the coordinates of all the centroids
    for (i = 0; i < K; i++)
    {
        centroid_X[i] = SumX[i] / size[i];
    }
    for (i = 0; i < K; i++)
    {
        centroid_Y[i] = SumY[i] / size[i];
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
    int *nCluster = malloc(sizeof(int) * N);

    // Array with all the centroids
    float array_centroids_X[K];
    float array_centroids_Y[K];

    int lenClusters[K];

    clock_t start, end;
    start = clock();

    int points_changed;

    init(array_points, nCluster, array_centroids_X, array_centroids_Y);

    points_changed = update_cluster_points(array_points, nCluster, array_centroids_X, array_centroids_Y);

    int nIterations = 0;
    do
    {
        determine_new_centroid(lenClusters, array_points, nCluster, array_centroids_X, array_centroids_Y);
        points_changed = update_cluster_points(array_points, nCluster, array_centroids_X, array_centroids_Y);
        nIterations++;
    } while (points_changed != 0);

    end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids_X[i], array_centroids_Y[i], lenClusters[i]);
    }
    printf("%f\n", timeSpent);

    free(nCluster);
    free(array_points);

    return;
}

// Com Vect: 13778330489
// Sem Vect: 13789678778