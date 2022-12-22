#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <cuda.h>
#include <chrono>

#include "k_means.h"


#define N  10000;
#define K  4;
#define N_THREADS  100;
#define N_BLOCKS  100;

using namespace std;

struct point
{
    float x;
    float y;
    int nCluster;
};

// Function where the array of points and the array of centroids are populated:
void init(struct point allpoints[N], struct point allcentroids[K])
{
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

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

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << endl << "Sequential CPU execution (init): " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds" << endl << endl;
}

float determineDistance(struct point p, struct point c)
{
    float diff_x = (p.x) - (c.x);
    float diff_y = (p.y) - (c.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
__global__ 
int update_cluster_points(struct point *allpoints, struct point *allcentroids, int points_changed)
{

        float diff_temp;
        int newCluster = -1;
        float diff;
        int id = blockIdx.x * blockDim.x + threadIdx.x;

        if (id => 0 && id < N)
        {
            diff = 100;
            // calculates the distance between a certain point and all the centroids
            for (int j = 0; j < K; j++)
            {
                // calculates the distance
                diff_temp = determineDistance(allpoints[id], allcentroids[j]);

                if (diff_temp < diff)
                {
                    diff = diff_temp;
                    newCluster = j;
                }
            }
            if (allpoints[id].nCluster != newCluster)
            {
                allpoints[id].nCluster = newCluster;
                atomicAdd(&points_changed, 1);
            }
        }
    __syncthreads();

}

__global__ 
void determine_new_centroid(int *size, struct point *allpoints, struct point *allcentroids)
{
    __device__ float SumX[K];
    __device__ float SumY[K];
    __device__ int sizeA[K];

    int id = blockIdx.x * blockDim.x + threadIdx.x;

    if (id => 0 && id < K)
    {
        SumX[id] = 0;
        SumY[id] = 0;
        sizeA[id] = 0;
    }
__syncthreads();

    if (id => 0 && id < N)
    {
        int nCluster = allpoints[id].nCluster;
        atomicAdd(&SumX[nCluster], allpoints[id].x);
            // SumX[nCluster] += allpoints[id].x; // possíveis data races
        atomicAdd(&SumY[nCluster], allpoints[id].y);
            // SumY[nCluster] += allpoints[id].y; // possíveis data races
        atomicAdd(&sizeA[nCluster], 1);
            // sizeA[nCluster]++; // possíveis data races
    }

__syncthreads();

    // Calculates the mean of the coordinates of all the centroids
    if (id => 0 && id < K)
    {
        allcentroids[id].x = SumX[id] / sizeA[id]; // não deve haver data races
        allcentroids[id].y = SumY[id] / sizeA[id]; // não deve haver data races
        size[id] = sizeA[id];                      // não deve haver data races
    }

    return;
}

int kmeans(int lenClusters[K], struct point array_points[N], struct point array_centroids[K], int *dlenClusters, struct point *darray_points, struct point *darray_centroids)
{

    int points_changed;
    int nIterations = 0;

    init(array_points, array_centroids);
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    cudaMemcpy (darray_points, array_points, sizeof(struct point) * N, cudaMemcpyHostToDevice);
    cudaMemcpy (darray_centroids, array_centroids, sizeof(struct point) * K, cudaMemcpyHostToDevice);

    points_changed = update_cluster_points <<< N_THREADS, N_BLOCKS >>> (darray_points, darray_centroids);
    do
    {
        determine_new_centroid <<< N_THREADS, N_BLOCKS >>> (dlenClusters, darray_points, darray_centroids);
        update_cluster_points <<< N_THREADS, N_BLOCKS >>> (darray_points, darray_centroids, points_changed);
        nIterations++;
    } while (nIterations != 21);

    cudaMemcpy (array_centroids, darray_centroids, sizeof(struct point) * K, cudaMemcpyDeviceToHost);
    cudaMemcpy (lenClusters, dlenClusters, sizeof(int) * K, cudaMemcpyDeviceToHost);

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << endl << "Sequential CPU execution (while): " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds" << endl << endl;


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

    struct point *darray_points;    // Array with all the points of this program
    struct point *darray_centroids; // Array with all the centroids
    int *dlenClusters;

    struct point *array_points = malloc(sizeof(struct point) * N);    // Array with all the points of this program
    struct point *array_centroids = malloc(sizeof(struct point) * K); // Array with all the centroids
    int *lenClusters = malloc(sizeof(int) * K);


    cudaMalloc ((void**) &darray_points, sizeof(struct point) * N);
    cudaMalloc ((void**) &darray_centroids, sizeof(struct point) * K);
    cudaMalloc ((void**) &dlenClusters, sizeof(int) * K);
    checkCUDAError("mem allocation");


    startKernelTime ();
    int nIterations = kmeans(lenClusters, array_points, array_centroids, dlenClusters, darray_points, darray_centroids);
    stopKernelTime ();
    checkCUDAError("kernel invocation");

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    printf("Iterations: %d\n", nIterations - 1);

    free(array_points);
    free(array_centroids);
    free(lenClusters);

    cudaFree(darray_points);
    cudaFree(darray_centroids);
    cudaFree(dlenClusters);
    checkCUDAError("mem free");
    return 0;
}