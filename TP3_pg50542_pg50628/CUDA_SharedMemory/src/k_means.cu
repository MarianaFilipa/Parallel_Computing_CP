#include "k_means.h"

int N = 10000000;
int K = 4;
int N_THREADS = 100;
int N_BLOCKS = 100000;

using namespace std;

struct point // 12 bytes por ponto
{
    float x;
    float y;
    int nCluster;
};

// Function where the array of points and the array of centroids are populated:
// This function will be executed by the host
void init(struct point *allpoints, struct point *allcentroids, int K, int N)
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
    cout << endl
         << "Sequential CPU execution (init): " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds" << endl
         << endl;
}

// Function that is used by the functions of the device (auxiliar function)
__device__ float determineDistance(struct point p, struct point c)
{
    float diff_x = (p.x) - (c.x);
    float diff_y = (p.y) - (c.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

__device__ void addValuesShared(float *SumX, float *SumY, int *size, int nCluster, float x, float y)
{
    atomicAdd(&SumX[nCluster], x);
    atomicAdd(&SumY[nCluster], y);
    atomicAdd(&size[nCluster], 1);
}

__device__ void addValues(float *SumX, float *SumY, int *size, float *SumXShared, float *SumYShared, int *sizeShared)
{
    int lid = threadIdx.x;
    if (0 <= lid && lid < 4)
    {
        atomicAdd(&SumX[lid], SumXShared[lid]);
        atomicAdd(&SumY[lid], SumYShared[lid]);
        atomicAdd(&size[lid], sizeShared[lid]);
    }
}

/*
    Objetivo: utilizar shared memory, uma vez que esta é mais rápida do que a memória global
*/

// Function where we determine if a point should change the cluster it is currently assign to
__global__ void update_cluster_points(struct point *allpoints, struct point *allcentroids, int K, int N, float *SumX, float *SumY, int *size) // int *points_changed,
{
    float diff_temp;     // Variável privada
    int newCluster = -1; // Variável privada
    float diff;          // Variável privada

    int id = blockIdx.x * blockDim.x + threadIdx.x;
    int lid = threadIdx.x; // local thread id within a block

    __shared__ struct point sharedCentroids[4]; // Bloco com os 4 centroids que é partilhado pelos pontos
    __shared__ float SumXShared[4];
    __shared__ float SumYShared[4];
    __shared__ int sizeShared[4];

    if (0 <= lid && lid < 4)
    {
        sharedCentroids[lid] = allcentroids[lid];
        SumXShared[lid] = 0;
        SumYShared[lid] = 0;
        sizeShared[lid] = 0;
    }
    __syncthreads(); // Wait for all threads within a block

    if (id >= 0 && id < N)
    {
        diff = 100;
        // calculates the distance between a certain point and all the centroids
        for (int j = 0; j < K; j++)
        {
            // calculates the distance
            diff_temp = determineDistance(allpoints[id], sharedCentroids[j]); //, sharedCentroidsY[j]

            if (diff_temp < diff)
            {
                diff = diff_temp;
                newCluster = j;
            }
        }
        if (allpoints[id].nCluster != newCluster)
        {
            allpoints[id].nCluster = newCluster; // não deve ter data races
            // atomicAdd(points_changed, 1);
        }
        addValuesShared(SumXShared, SumYShared, sizeShared, newCluster, allpoints[id].x, allpoints[id].y);
    }

    __syncthreads(); // Wait for all threads within a block
    addValues(SumX, SumY, size, SumXShared, SumYShared, sizeShared);
}

__global__ void initialize_sums_and_size(int *size, float *SumX, float *SumY, int K)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id >= 0 && id < K)
    {
        SumX[id] = 0;
        SumY[id] = 0;
        size[id] = 0;
    }
}

__global__ void mean_sums(int *size, float *SumX, float *SumY, struct point *allcentroids, int K)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id >= 0 && id < K)
    {
        allcentroids[id].x = SumX[id] / size[id]; // não deve haver data races
        allcentroids[id].y = SumY[id] / size[id]; // não deve haver data races
    }
}

__global__ void print_data(int K, float *SumX, float *SumY, int *size)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id == 0)
    {
        for (int i = 0; i < K; i++)
        {
            printf("%f - ", SumX[i]);
            printf("%f - ", SumY[i]);
            printf("%d\n", size[i]);
        }
    }
}

/*__syncthreads(); //__syncthreads sincroniza apenas as threads do mesmo bloco
                 // Não encontrei nenhuma solução para sincronizar todas as threads*/

__global__ void sum_all_points(struct point *allpoints, int *size, float *SumX, float *SumY, int N)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    if (id >= 0 && id < N)
    {
        int nCluster = allpoints[id].nCluster;
        atomicAdd(&SumX[nCluster], allpoints[id].x);
        // SumX[nCluster] += allpoints[id].x; // possíveis data races
        atomicAdd(&SumY[nCluster], allpoints[id].y);
        // SumY[nCluster] += allpoints[id].y; // possíveis data races
        atomicAdd(&size[nCluster], 1);
        // sizeA[nCluster]++; // possíveis data races
    }
}

int kmeans(int *lenClusters, struct point *array_points, struct point *array_centroids, int K, int N)
{
    // -------- Sequencial ----------
    int nIterations = 0;

    init(array_points, array_centroids, K, N);                            // Initialize array points & centroids
    chrono::steady_clock::time_point begin = chrono::steady_clock::now(); // Start Clock

    struct point *darray_points; // Array with all the points of this program, for the device memory
    cudaMalloc((void **)&darray_points, sizeof(struct point) * N);
    cudaMemcpy(darray_points, array_points, sizeof(struct point) * N, cudaMemcpyHostToDevice); // Copy points to device

    struct point *darray_centroids; // Array with all the centroids, for the device memory
    cudaMalloc((void **)&darray_centroids, sizeof(struct point) * K);
    cudaMemcpy(darray_centroids, array_centroids, sizeof(struct point) * K, cudaMemcpyHostToDevice); // Copy centroids to device

    int *dlenClusters;
    cudaMalloc((void **)&dlenClusters, sizeof(int) * K);

    float *SumX;
    cudaMalloc((void **)&SumX, sizeof(float) * K);

    float *SumY;
    cudaMalloc((void **)&SumY, sizeof(float) * K);
    checkCUDAError("Memory Allocation");

    // initialize_sums_and_size<<<N_BLOCKS, N_THREADS>>>(dlenClusters, SumX, SumY, K); // Coloca os SumX a 0
    // cudaDeviceSynchronize();
    // update_cluster_points<<<N_BLOCKS, N_THREADS>>>(darray_points, darray_centroids, K, N, SumX, SumY, dlenClusters); //&points_changed,
    // mean_sums<<<N_BLOCKS, N_THREADS>>>(dlenClusters, SumX, SumY, darray_centroids, K);
    // checkCUDAError("error first update");
    do
    {
        // -------- Parallel ----------
        initialize_sums_and_size<<<N_BLOCKS, N_THREADS>>>(dlenClusters, SumX, SumY, K); // initialize SumX, SumY and size
        cudaDeviceSynchronize();
        update_cluster_points<<<N_BLOCKS, N_THREADS>>>(darray_points, darray_centroids, K, N, SumX, SumY, dlenClusters); // assign points to cluster and sums
        cudaDeviceSynchronize();
        mean_sums<<<N_BLOCKS, N_THREADS>>>(dlenClusters, SumX, SumY, darray_centroids, K); // mean of values of sum, update SumX and SumY
        cudaDeviceSynchronize();
        nIterations++;
    } while (nIterations != 21);

    checkCUDAError("k_means While Cycle");

    cudaMemcpy(array_centroids, darray_centroids, sizeof(struct point) * K, cudaMemcpyDeviceToHost);
    cudaMemcpy(lenClusters, dlenClusters, sizeof(int) * K, cudaMemcpyDeviceToHost);
    checkCUDAError("Memory copy to host");

    cudaFree(darray_points);
    cudaFree(darray_centroids);
    cudaFree(dlenClusters);
    cudaFree(SumX);
    cudaFree(SumY);
    checkCUDAError("mem free");

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << endl
         << "Execution K_Means:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " microseconds" << endl
         << endl;

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

    struct point *array_points = (struct point *)malloc(sizeof(struct point) * N);    // Array with all the points of this program
    struct point *array_centroids = (struct point *)malloc(sizeof(struct point) * K); // Array with all the centroids
    int *lenClusters = (int *)malloc(sizeof(int) * K);

    startKernelTime();
    int nIterations = kmeans(lenClusters, array_points, array_centroids, K, N);
    stopKernelTime();
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

    return 0;
}