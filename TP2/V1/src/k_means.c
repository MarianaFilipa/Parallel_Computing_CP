#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int N = 10000000;
int K = 4;
int N_THREADS = 6;

struct point
{
    float x;
    float y;
    int nCluster;
};

// Function that allows us to print out the value of a Point:
void printPoint(struct point p)
{
    printf("x = %f \n", p.x);
}

// Function where the array of points and the array of centroids are populated:
// O(N+K)
void init(struct point allpoints[N], struct point allcentroids[K])
{
    srand(10);
    // Add random values to all the points in the array
    int i;
    for (i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        allpoints[i].nCluster = -1;
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
/*float determineDistance(float px, float py, float cx, float cy)
{
    float diff_x = (px) - (cx);
    float diff_y = (py) - (cy);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}*/

float determineDistance(struct point p, struct point c)
{
    float diff_x = (p.x) - (c.x);
    float diff_y = (p.y) - (c.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
// O(N*K)
int update_cluster_points(struct point allpoints[N], struct point allcentroids[K])
{
    int points_changed = 0; // counts the number of points that changed cluster
    float diff_temp;
    int newCluster;
    float diff;

#pragma omp parallel num_threads(N_THREADS)

#pragma omp for private(diff, newCluster, diff_temp) reduction(+ \
                                                               : points_changed) schedule(static)
    for (int i = 0; i < N; i++)
    {
        diff = 100;
        // calculates the distance between a certain point and all the centroids
        for (int j = 0; j < K; j++)
        {
            // calculates the distance
            diff_temp = determineDistance(allpoints[i], allcentroids[j]);
            /*int id = omp_get_thread_num();
            printf("T%d: i%d, j%d, dist: %f\n", id, i, j, diff_temp);*/

            // verifies if it should change cluster or not:
            if (diff_temp < diff)
            {
                diff = diff_temp;
                newCluster = j;
            }
        }
        // int id = omp_get_thread_num();
        // printf("T%d: i%d\n", id, i);

        // If the current cluster is not the one to which the point is closer to, then point changes cluster
        if (allpoints[i].nCluster != newCluster)
        {
            allpoints[i].nCluster = newCluster;
            points_changed++;
        }
    }

    // printf("PC%d\n", points_changed);
    return points_changed;
}

// Calculates the mean of the coordenates of all the points that are in a cluster
// O(2*K + N)
void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{

    float SumX[K];
    float SumY[K];
    int sizeA[K];

#pragma omp parallel num_threads(N_THREADS)
    {
// change the coordenates of all the centroids to (0,0)
#pragma omp for
        for (int i = 0; i < K; i++)
        {
            SumX[i] = 0;
            SumY[i] = 0;
            sizeA[i] = 0;
        }

// For each point, add its coordinates to the coordinates of a certain center
#pragma omp barrier
#pragma omp for firstprivate(SumX, SumY, sizeA)
        for (int i = 0; i < N; i++)
        {
            int nCluster = allpoints[i].nCluster;
            SumX[nCluster] += allpoints[i].x;
            SumY[nCluster] += allpoints[i].y;
            sizeA[nCluster]++;
        }

// Calculates the mean of the coordinates of all the centroids
#pragma omp barrier
#pragma omp for
        for (int i = 0; i < K; i++)
        {
            allcentroids[i].x = SumX[i] / sizeA[i];
            allcentroids[i].y = SumY[i] / sizeA[i];
            size[i] = sizeA[i];
        }
    }

    return;
}

/*void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{
    float SumX[K];
    float SumY[K];
    int sizeA[K];

#pragma omp parallel num_threads(N_THREADS)
    {
        // change the coordenates of all the centroids to (0,0)
        for (int i = 0; i < K; i++)
        {
            SumX[i] = 0;
            SumY[i] = 0;
            sizeA[i] = 0;
        }

        // For each point, add its coordinates to the coordinates of a certain center
        for (int i = 0; i < N; i++)
        {
            int nCluster = allpoints[i].nCluster;
            SumX[nCluster] += allpoints[i].x;
            SumY[nCluster] += allpoints[i].y;
            sizeA[nCluster]++;
        }

        // Calculates the mean of the coordinates of all the centroids
        for (int i = 0; i < K; i++)
        {
            allcentroids[i].x = SumX[i] / sizeA[i];
            allcentroids[i].y = SumY[i] / sizeA[i];
            size[i] = sizeA[i];
        }
    }
    return;
}*/

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

    clock_t start, end;
    start = clock();

    // Array with all the points of this program
    struct point *array_points = malloc(sizeof(struct point) * N);
    // Array with all the centroids
    struct point array_centroids[K];
    int points_changed;
    int lenClusters[K];

    init(array_points, array_centroids);
    points_changed = update_cluster_points(array_points, array_centroids);

    int nIterations = 0;
    do
    {
        determine_new_centroid(lenClusters, array_points, array_centroids);
        points_changed = update_cluster_points(array_points, array_centroids);
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
    free(array_points);
    return;
}