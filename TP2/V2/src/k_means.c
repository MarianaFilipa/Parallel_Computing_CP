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

// Function where the array of points and the array of centroids are populated:
// O(N+K)
void init(struct point *allpoints, int *nClustersPoint, struct point *allcentroids)
{
    srand(10);
    // Add random values to all the points in the array
    int i;
    for (i = 0; i < N; i++)
    {
        allpoints[i].x = (float)rand() / RAND_MAX;
        allpoints[i].y = (float)rand() / RAND_MAX;
        // nClustersPoint[i] = 0; // Aqui deixará de haver localidade espacial:
    }

    for (i = 0; i < N; i++)
    {
        nClustersPoint[i] = 0; // Aqui deixará de haver localidade espacial:
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
int update_cluster_points(struct point *allpoints, int *nClustersPoint, struct point *allcentroids)
{
    int points_changed = 0; // counts the number of points that changed cluster
    // For each point, verifies if the point should change cluster
    int i;
    for (i = 0; i < N; i++)
    {
        float dif_temp;
        float diff = 100;
        int newCluster;
        int j;
        for (j = 0; j < K; j++)
        {
            // calculates the distance
            dif_temp = determineDistance(allpoints[i], allcentroids[j]);

            // verifies if it should change cluster or not:
            if (dif_temp < diff)
            {
                diff = dif_temp;
                newCluster = j;
            }
        }

        // If the current cluster is not the one to which the point is closer to, then point changes cluster
        if (nClustersPoint[i] != newCluster)
        {
            nClustersPoint[i] = newCluster;
            points_changed++;
        }
    }

    return points_changed;
}

// Calculates the mean of the coordenates of all the points that are in a cluster
// O(2*K + N)
void determine_new_centroid(int *size, int *nClustersPoint, struct point *allpoints, struct point *allcentroids)
{
    // change the coordenates of all the centroids to (0,0)
    int i;
    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = 0;
        allcentroids[i].y = 0;
        size[2 * i] = 0;
        size[2 * i + 1] = 0;
    }

    for (i = 0; i < N; i++)
    {
        int nCluster = nClustersPoint[i]; // isto deixa de ter localidade espacial, mas poderá ter localidade temporal
        size[2 * nCluster]++;
        size[2 * nCluster + 1]++;
        allcentroids[nCluster].x += allpoints[i].x;
        allcentroids[nCluster].y += allpoints[i].y;
    }

    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = allcentroids[i].x / size[2 * i];
        allcentroids[i].y = allcentroids[i].y / size[2 * i + 1];
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
    int *nClustersPoint = malloc(sizeof(int) * N);

    // Array with all the centroids
    struct point array_centroids[K]; //= malloc(sizeof(struct point) * K); //[K]; //

    init(array_points, nClustersPoint, array_centroids);

    int points_changed;
    int lenClusters[2 * K];

    points_changed = update_cluster_points(array_points, nClustersPoint, array_centroids);

    int nIterations = 0;

    do
    {
        determine_new_centroid(lenClusters, nClustersPoint, array_points, array_centroids);
        points_changed = update_cluster_points(array_points, nClustersPoint, array_centroids);
        nIterations++;
    } while (nIterations != 21);

    printf("%d\n", nIterations);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[2 * i]);
    }

    // free(array_centroids);
    free(array_points);
    free(nClustersPoint);

    return;
}

//         N : 10000000
// (nothing)    #I:     755508                    #CC :    1058209  #Misses:   14616
// N+K => 10000000 + 4              10000004
// (init)       #I: 1475055311                    #CC :  690121411  #Misses: 2859322
// N*(K+K-1) =                      70000000
// (+update)    #I: 2625516241  (+1200000000)     #CC : 1170873519  #Misses: 4792886
// N+2K =                           10000008
// (+determine) #I: 2745717765   (+100000000)     #CC : 1249480432  #Misses: 6625070

// SECALHAR A FUNÇÃO QUE TEM MAIS INSTRUÇÕES É A UPDATE
// É NESSE O NÚMERO DE CICLOS E DE INSTRUÇÕES AUMENTO MAIS DRÁSTICAMENTE

// A função de determine tem quase tantas instruções como o código em si...

// Sem vect: 24118764798
// Com vect: 15481816999
