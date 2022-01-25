#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define N 100000

//mpicc test.c
//mpiexec.hydra -n nombreProc ./a.out

//int main(int argc, char const *argv[]){

	/*MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	printf("\n procs %d parmis %d\n", rank, size);

	char processor[50];
	int name_len;
	MPI_Get_processor_name(processor, &name_len);
	printf("Processus %d s'execute sur %s\n", rank, processor);
	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);
	int rank, msg=22;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank==0) {
		MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
		printf("msg reçu du proc %d\n", status.MPI_SOURCE);
		MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
		printf("msg reçu du proc %d\n", status.MPI_SOURCE);
	} else {
		MPI_Send(&msg, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
	}
	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);
	MPI_Status status;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int x, y;
	if(rank==0) {
		x=10;
		y = 20;
		MPI_Send(&x, 1, MPI_INT, 1, 99, MPI_COMM_WORLD);
		MPI_Send(&y, 1, MPI_INT, 1, 88, MPI_COMM_WORLD);
	}
	if(rank==1) {
		MPI_Recv(&x, 1, MPI_INT, 0, 88, MPI_COMM_WORLD, &status);
		printf("reçu y  de proc : %d %d\n", x, status.MPI_SOURCE);
		MPI_Recv(&y, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
		printf("reçu x de proc : %d %d\n", y, status.MPI_SOURCE);
	}
	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);
	MPI_Status status;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size=3;
	int prev=(rank+1)%size;
	int suiv = (rank -1+size)%size;
	int x;

	if(rank==0) {
		x=10;
		MPI_Send(&x, 1, MPI_INT, suiv, 99, MPI_COMM_WORLD);
		MPI_Recv(&x, 1, MPI_INT, prev, 99, MPI_COMM_WORLD, &status);
		printf("rank=%d recu x=%d \n");
	} else {
		MPI_Recv(&x, 1, MPI_INT, prev, 99, MPI_COMM_WORLD, &status);
		printf("rank=%d recu x=%d\n", rank, x);
		MPI_Send(&x, 1, MPI_INT, suiv, 99, MPI_COMM_WORLD);
	}
	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);			// a compiler avec un -lm
	MPI_Status status;
	int rank, root = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char msg[8];
	if(rank==0) {
		strcpy(msg, "Bonjour");
	}

	MPI_Bcast(msg, 8, MPI_CHAR, root, MPI_COMM_WORLD);
	printf("Proc %d a recu %s\n", rank, msg);

	char c;
	MPI_Scatter(msg, 1, MPI_CHAR, &c, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	printf("proc %d a recu %c\n", rank, c);

	char msgrecolte[8];
	MPI_Gather(&c, 1, MPI_CHAR, msgrecolte, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	if(rank == 0) {
		printf("%s\n", msgrecolte);
	}

	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);			// a compiler avec un -lm
	MPI_Status status;					// calcul PI
	int P, rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	double h, xi, pi, pik;
	int i, R=N/P;
	h=(double) 1.0/N;
	pi=0.0;
	pik=0.0;

	for(i=rank*R; i < (rank+1)*R; i++) {
		xi = h * ((double) i-0.5);
		pik += sqrt(1 - xi * xi);
	}
	pik = pik * 4 * h;
	MPI_Reduce(&pik, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if(rank==0) {
		printf("%.62f\n", pi);
	}
	MPI_Finalize();*/
//---------------------------------------------------------
	/*MPI_Init(&argc, &argv);
	MPI_Status status;
	int rank;
	int n = 3;
	int A[n][n] = [[2, 3, 0], [1, 4, 1], [-1, 5, 3]];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &A);

	MPI_Scatter(msg, 1, MPI_CHAR, &c, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	printf("proc %d a recu %c\n", rank, c);

	MPI_Gather(&c, 1, MPI_CHAR, msgrecolte, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

	if(rank==0) {
		printf("%.62f\n", pi);
	}
	MPI_Finalize();*/
//---------------------------------------------------------
//}







/*calcul temps de chaque processus : */
/*
#include <time.h>
...
int tab[N];
double top_debut, top_fin, temps;
top_debut = MPI_Wtime();
if(rank == 0) {
	MPI_Send(tab, N, MPI_INT, 1, 99, MPI_COMM_WORLD);
}
if(rank == 1) {
	MPI_Recv(tab, N, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
}
top_fin = MPI_Wtime();
temps = top_fin - top_debut;
*/






/*
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define N 100000

int main(int argc, char** argv){

    MPI_Init(&argc, &argv);
    int P, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Status status;

    double h, xi, pik, top_debut, top_fin;
    int R = N/P;
    h = (double) 1.0 / N;
    pik = 0.0;

    top_debut=MPI_Wtime();

    for(int i = rank * R; i < (rank + 1) * R; i++){
        xi = h * ((double) i - 0.5);
        pik += sqrt(1 - xi * xi);
    }
    pik = pik * 4 * h;

    if(rank!=0) {
        MPI_Send(&pik, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
    } else {
        double pi=0.0, somme;
        for(int i=1; i<P; i++) {
            MPI_Recv(&somme, 1, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &status);
            pi +=somme;
        }
        pi+=pik;
        printf("%f\n", pi);
        top_fin=MPI_Wtime();
        double temps = top_fin - top_debut;
        printf("%f\n", temps);
    }

    //printf("%.50f\n", pik);
    MPI_Finalize();
}
*/


int main(int argc , char** argv){
    MPI_Init(&argc,&argv);

    srand(time(NULL));
    double debut,fin;
    MPI_Status status;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int r=2,n=size*r;
    int A[n][n], b[n][n],c[n],Ai[r][n],bloc[r],btemp[r],cloc[r],rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if(rank == 0){
        debut = MPI_Wtime();
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                A[i][j]=rand()%3;
                printf("%d ",A[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        for(int i=0;i<n;i++){
        	for(int j = 0; j < n; j++) {
            	b[i][j]=rand()%3;
                printf("%d ",b[i][j]);
        	}
            printf("\n");
        }
        printf("\n");
    }

    MPI_Datatype ligne,Blocligne;
    MPI_Type_contiguous(n*r, MPI_INT, &Blocligne);
    MPI_Type_contiguous(r, MPI_INT, &ligne);
    MPI_Type_commit(&Blocligne);
    MPI_Type_commit(&ligne);

    MPI_Scatter(&A[0][0],1,Blocligne,&Ai[0][0],1,Blocligne,0,MPI_COMM_WORLD);
    MPI_Scatter(&b[0][0],1,ligne,&bloc[0],1,ligne,0,MPI_COMM_WORLD);

    int pred = (rank-1+size)%size;
    int suiv = (rank+1)%size;
    printf("rank : %d | suiv : %d | pred %d \n",rank,suiv,pred);
    for(int i=0;i<n*n;i++){
        cloc[i][j]=0;
    }

    for(int t=0;t<size;t++){
        int index = (rank-t+size)%size;
        for(int i=0;i<r;i++){
            for (int k = 0; k < r; k++)
            {
                cloc[i] += Ai[i][index*r+k]*bloc[k];
            }
        }
        if(rank==0){
            MPI_Send(&bloc[0], 1,ligne,suiv,99,MPI_COMM_WORLD);
            MPI_Recv(&bloc[0], 1,ligne,pred,99,MPI_COMM_WORLD,&status);
        } else {
            MPI_Recv(&btemp[0], 1,ligne,pred,99,MPI_COMM_WORLD,&status);
            MPI_Send(&bloc[0], 1,ligne,suiv,99,MPI_COMM_WORLD);
            for(int j=0;j<r;j++){
                //printf("rank : %d | bloc : %d \n",rank,bloc[j]);
                bloc[j]=btemp[j];
            }
        }
    }
    

    MPI_Gather(&cloc[0],r,MPI_INT,&c[0],r,MPI_INT,0,MPI_COMM_WORLD);

    if(rank == 0){
        fin = MPI_Wtime();
        for(int i=0;i<n;i++){
            printf("%d ",c[i] );
        }
        printf("\n");
        fin = fin - debut;
        printf("Temps d'exécution : %f \n",fin);
    }

    MPI_Finalize();
}