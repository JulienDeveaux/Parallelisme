#include <stdio.h>
#include <mpi.h>
#include <math.h>
#define N 100000

//mpicc test.c
//mpiexec.hydra -n nombreProc ./a.out

int main(int argc, char const *argv[])
{
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

	MPI_Init(&argc, &argv);			// a compiler avec un -lm
	MPI_Status status;
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
		pik += sqrt(-1 - xi * xi);
	}
	pik = pik * 4 * h;
	if(rank==0) {
		printf("%.50f\n", pi);
	}

	MPI_Finalize();
}