#include <stdio.h>
#include "mpi.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define n 4

void Scatter4(int argc, char** argv);

int main(int argc, char** argv)
{
    //PI(argc, argv);

    Scatter4(argc, argv);

    return 0;
}

void Scatter4(int argc, char** argv)
{
    double t_start, t_end;

    MPI_Init(&argc, &argv);

    int rank, p;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int r = n / p;
    int* a, *b, *c;

    if (!rank)
    {
        t_start = MPI_Wtime();

        a = malloc(n * n * sizeof(int));
        b = malloc(n * n * sizeof(int));
        c = malloc(n * n * sizeof(int));

        srand(time(NULL));

        for (int i = 0; i < n * n; i++) a[i] = rand() % 10;
        for (int i = 0; i < n * n; i++) b[i] = rand() % 10;

        printf("a: \n");
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
                printf("%d ", a[i * n + j]);
            printf("\n");
        }

        printf("---------------------\nb: \n");
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
                printf("%d ", b[i * n + j]);
            printf("\n");
        }
    }

    MPI_Datatype ligne;
    MPI_Type_contiguous(n, MPI_INT, &ligne);
    MPI_Type_commit(&ligne);

    MPI_Datatype colonne;
    MPI_Type_vector(n, 1, n, MPI_INT, &colonne);
    MPI_Type_commit(&colonne);

    MPI_Datatype col_resized;
    MPI_Type_create_resized(colonne, 0, sizeof(int), &col_resized);
    MPI_Type_commit(&col_resized);

    int ai[r][n], bloc[r][n];

    MPI_Scatter(a, r, ligne, ai  , r, ligne, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, r, col_resized, bloc, r, ligne, 0, MPI_COMM_WORLD);

    int suiv = (rank + 1) % p, prec = (rank - 1 + p) % p;
    int cloc[r][n];

    for (int j = 0; j < r; ++j)
        for (int g = 0; g < n; ++g)
            cloc[j][g] = 0;

    for (int t = 0; t < p; ++t)
    {
        int index = (rank - t + p) % p;

        for (int j = 0; j < r; ++j)
        {
            for (int k = 0; k < r; ++k)
            {
                int sum = 0;

                for (int g = 0; g < n; ++g)
                {
                    sum += ai[j][g] * bloc[k][g];
                }
                
                cloc[j][index * r + k] = sum;
            }
        }

        MPI_Status status;

        if (!rank)
        {
            MPI_Send(bloc, r, ligne, suiv, 99, MPI_COMM_WORLD);
            MPI_Recv(bloc, r, ligne, prec, 99, MPI_COMM_WORLD, &status);
        }
        else
        {
            int btemp[r][n];

            MPI_Recv(btemp, r, ligne, prec, 99, MPI_COMM_WORLD, &status);
            MPI_Send(bloc, r, ligne, suiv, 99, MPI_COMM_WORLD);

            for (int k = 0; k < r; ++k)
                for (int j = 0; j < n; ++j)
                    bloc[k][j] = btemp[k][j];
        }
    }

    MPI_Gather(cloc, r, ligne, c, r, ligne, 0, MPI_COMM_WORLD);

    if (!rank)
    {
        printf("---------------------\nc: \n");
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
                printf("%d ", c[i * n + j]);
            printf("\n");
        }

        t_end = MPI_Wtime();

        printf("Elapsed time = %fms\n", (double)((t_end - t_start)*1000));
    }

    MPI_Finalize();
}