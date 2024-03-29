#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000
#define K 4

struct point
{
    float x;
    float y;
    // Allows us to associate this point with a certain cluster
    // It's equivalent to the pos of a certain centroid
    int nCluster;
};

// Array with all the points of this program
struct point array_points[N];
// Array with all the centroids
struct point array_centroids[K];

// Function that allows us to print out the value of a Point:
void printPoint(struct point p)
{
    printf("x = %f \n", p.x);
}

// Function where the array of points and the array of centroids are populated:
// O(N+K)
void init(struct point allpoints[N], struct point allcentroids[K])
{
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
float determineDistance(struct point p, struct point centroid)
{
    float diff_x = (p.x) - (centroid.x);
    float diff_y = (p.y) - (centroid.y);
    float aux = diff_x * diff_x + diff_y * diff_y;
    return aux;
}

// Function where we determine if a point should change the cluster it is currently assign to
// O(N*K)
int update_cluster_points(struct point allpoints[N], struct point allcentroids[K])
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
        for (j = 0; j < K; j++)
        {
            // calculates the distance
            diff_temp = determineDistance(allpoints[i], allcentroids[j]);

            // verifies if it should change cluster or not:
            if (diff_temp < diff)
            {
                diff = diff_temp;
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

    return points_changed;
}

// Calculates the mean of the coordenates of all the points that are in a cluster
// O(2*K + N)
void determine_new_centroid(int size[K], struct point allpoints[N], struct point allcentroids[K])
{
    // change the coordenates of all the centroids to (0,0)
    int i;
    for (i = 0; i < K; i++)
    {
        allcentroids[i].x = 0;
        allcentroids[i].y = 0;
        size[i] = 0;
    }

    // For each point, add its coordinates to the coordinates of a certain center
    for (i = 0; i < N; i++)
    {
        int nCluster = allpoints[i].nCluster;
        allcentroids[nCluster].x += allpoints[i].x;
        allcentroids[nCluster].y += allpoints[i].y;
        size[nCluster]++;
    }

    // Calculates the mean of the coordinates of all the centroids
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
    } while (points_changed != 0);

    end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%d\n", nIterations);
    int i;
    for (i = 0; i < K; i++)
    {
        printf("Center: (%.3f,%.3f) : Size %d \n", array_centroids[i].x, array_centroids[i].y, lenClusters[i]);
    }
    printf("%f\n", timeSpent);
    return;
}