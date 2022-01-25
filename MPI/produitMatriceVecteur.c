#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include "mpi.h"
#define n 4

void print_results(char *prompt, int a[n][n]);

int main(int argc, char *argv[])
{
    int rank, size, sum = 0;
    int a[n][n], b[n][n], c[n][n], aloc[n],bloc[n],cloc[n];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	double top_debut, top_fin, temps;
	top_debut = MPI_Wtime();

    if(rank == 0) {
    	srand(time(NULL));
		for(int i = 0; i < n-1; i++) {
			for(int j = 0; j < n-1; j++) {
				a[i][j] = rand()%3;
			}
		}
		for(int i = 0; i < n-1; i++) {
			for(int j = 0; j < n-1; j++) {
				b[i][j] = rand()%3;
			}
		}
    }
  
    MPI_Scatter(a, n*n/size, MPI_INT, aloc, n*n/size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(b, n/size, MPI_INT, bloc, n*n/size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum = sum + aloc[j] * bloc[i];
        }
        cloc[i] = sum;
        sum = 0;
    }

    MPI_Gather(cloc, n*n/size, MPI_INT, c, n*n/size, MPI_INT, 0, MPI_COMM_WORLD);
	top_fin = MPI_Wtime();
	temps = top_fin - top_debut;        
    MPI_Finalize();
    if (rank == 0) {
    	printf("matrice résultat : \n");
        for(int i = 0; i < n; i++) {
        	for(int j = 0; j < n; j++) {
        		printf("%d ", c[i][j]);
        	}
        	printf("\n");
        }
		printf("temps écoulé : %f\n", temps);
	}
}


/*int main(int argc, char** argv) {	// anneau pour n processus
	MPI_Init(&argc, &argv);
	MPI_Status status;
	int P, rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int suiv = (rank+1)%size;
	int prev = (rank -1+size)%size;
	int *A, *B, *C, Ai[n], bloc, btemp, cloc;

	if(rank == 0) {
		A = malloc(n*n*sizeof(int));
		B = malloc(n*sizeof(int));
		C = malloc(n*sizeof(int));
		A[0] = 2;
		A[1] = 1;
		A[2] = 0;
		A[3] = 4;
		A[4] = 3;
		A[5] = 2;
		A[6] = 1;
		A[7] = 1;
		A[8] = 4;
		A[9] = 3;
		A[10] = 1;
		A[11] = 2;
		A[12] = 3;
		A[13] = 0;
		A[14] = 2;
		A[15] = 0;
		B[0] = 1;
		B[1] = 3;
		B[2] = 4;
		B[3] = 1;
	}

	MPI_Datatype ligne;		//définition de la taille d'une ligne a traiter ici c'est la taille du vecteur
	MPI_Type_contiguous(n, MPI_INT, &ligne);
	MPI_Type_commit(&ligne);

	MPI_Scatter(A, 1, ligne, Ai, 1, ligne, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, 1, MPI_INT, &bloc, 1, MPI_INT, 0, MPI_COMM_WORLD);

	cloc = 0;
	for(int i = 0; i < n; i++) {
		int index = (rank-i+n)%n;
		cloc += Ai[index] * bloc;
		if(rank == 0) {
			MPI_Send(&bloc, 1, MPI_INT, suiv, 99, MPI_COMM_WORLD);
			MPI_Recv(&bloc, 1, MPI_INT, prev, 99, MPI_COMM_WORLD, &status);
		} else {
			MPI_Recv(&btemp, 1, MPI_INT, prev, 99, MPI_COMM_WORLD, &status);
			MPI_Send(&bloc, 1, MPI_INT, suiv, 99, MPI_COMM_WORLD);
			bloc = btemp;
		}
	}
	MPI_Gather(&cloc, 1, MPI_INT, C, 1, MPI_INT, 0, MPI_COMM_WORLD);


	if(rank == 0) {
		for(int i = 0; i < n; i++) {
			printf("%d\n", C[i]);
		}
	}
	return 0;
}*/


/*int main(int argc, char** argv){
	MPI_Init(&argc, &argv);
	int *A, b[n], *c, rank, p;		//A la matrice, B le vecteur, C le résultat
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int r = n / p;
	int clocal[r];
	int Ai[r][n];
	double top_debut, top_fin, temps;
	top_debut = MPI_Wtime();

	if(rank == 0) {		//génération a et b
		A = malloc(n*n*sizeof(int));
		c = malloc(n*sizeof(int));
		srand(time(NULL));
		for(int i = 0; i < n*n; i++) {
			A[i] = rand()%3;
		}
		for(int i = 0; i < n; i++) {
			b[i] = rand()%3;
		}
	}

	MPI_Datatype ligne;		//définition de la taille d'une ligne a traiter ici c'est la taille du vecteur
	MPI_Type_contiguous(n, MPI_INT, &ligne);
	MPI_Type_commit(&ligne);

	//distrib des données
	MPI_Scatter(A, r, ligne, Ai, r, ligne, 0, MPI_COMM_WORLD); // chaque processus reçoit une ligne de a   A, rligne, Ai, rligne et Ai est un tab de [ligne][col]
	MPI_Bcast(b, 1, ligne, 0, MPI_COMM_WORLD); // chaque processus get b a mult
	//calcul
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < n; j++) {
			clocal[i] += Ai[i][j]*b[j];				//bloc de r coeff de C
		}
	}
	MPI_Gather(&clocal[0], r, MPI_INT, c, r, MPI_INT, 0, MPI_COMM_WORLD); // on met dans c le resultat de chaque ligne
	top_fin = MPI_Wtime();
	temps = top_fin - top_debut;

	if(rank == 0) {
		printf("Resultat : \n");
		for(int i = 0; i < n; i++) {
			printf("c : %d\n", c[i]);
		}
		printf("temps : %f\n", temps);
	}
	MPI_Type_free(&ligne);
	MPI_Finalize();
}*/


/*int main(int argc, char** argv){			// pour n machine c'est efficace
	MPI_Init(&argc, &argv);
	int *A, b[n], *c, Ai[n], clocal, rank;		//A la matrice, B le vecteur, C le résultat
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0) {		//génération a et b
		A = malloc(n*n*sizeof(int));
		c = malloc(n*sizeof(int));
		srand(time(NULL));
		for(int i = 0; i < n*n; i++) {
			A[i] = rand()%3;
		}
		for(int i = 0; i < n; i++) {
			b[i] = rand()%3;
		}

		/*A[0] = 2;
		A[1] = 1;
		A[2] = 0;
		A[3] = 4;
		A[4] = 3;
		A[5] = 2;
		A[6] = 1;
		A[7] = 1;
		A[8] = 4;
		A[9] = 3;
		A[10] = 1;
		A[11] = 2;
		A[12] = 3;
		A[13] = 0;
		A[14] = 2;
		A[15] = 0;
		b[0] = 1;
		b[1] = 3;
		b[2] = 4;
		b[3] = 1;*//*
	}

	MPI_Datatype ligne;		//définition de la taille d'une ligne a traiter ici c'est la taille du vecteur
	MPI_Type_contiguous(n, MPI_INT, &ligne);
	MPI_Type_commit(&ligne);

	//distrib des données
	MPI_Scatter(A, 1, ligne, Ai, 1, ligne, 0, MPI_COMM_WORLD); // chaque processus reçoit une ligne de a   A, rligne, Ai, rligne et Ai est un tab de [ligne][col]
	MPI_Bcast(b, 1, ligne, 0, MPI_COMM_WORLD); // chaque processus get b a mult

	//calcul
	clocal = 0;		//tab de r entiers
	for(int i = 0; i < n; i++) {
		clocal += Ai[i]*b[i];				//bloc de r coeff de C
	}
	MPI_Gather(&clocal, 1, MPI_INT, c, 1, MPI_INT, 0, MPI_COMM_WORLD); // on met dans c le resultat de chaque ligne 	//rentier, int, c, rentier et r = n/p avec p le nb de machines

	if(rank == 0) {
		for(int i = 0; i < n; i++) {
			printf("c : %d\n", c[i]);
		}
	}

	MPI_Finalize();
}*/

/*
MPI_Datatype colonne;
MPI_Type_vector(n, 1, n, MPI_INT, &colonne);
MPI_Typecommit(&colonne);

MPI_Datatype col_resized;
MPI_Type_create_resized(colonne, 0, sizeof(int), &col_resized);
MPI_Type_commit(&col_resized);
*/