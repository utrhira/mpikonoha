#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define  N      480

int     myid, numprocs, np;

void myMatMat(double [N/numprocs][N], double [N/numprocs][N], double [N][N/numprocs], int);

int main(int argc, char* argv[])
{
	double  t0, t1, t2, t_w;
	double  dc_inv, d_mflops;

	int     i, j;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	np   = N/numprocs;

	double   A[np][N];
	double   B[N][np];
	double   C[np][N];

	/* matrix generation --------------------------*/
	srand(1);
	dc_inv = 1.0/(double)RAND_MAX;
	for(j=0; j<np; j++) {
		for(i=0; i<N; i++) {
			A[j][i] = rand()*dc_inv;
			B[i][j] = rand()*dc_inv;
			C[j][i] = 0.0;
		}
	} /* end of matrix generation --------------------------*/

	/* Start of mat-vec routine ----------------------------*/
	MPI_Barrier(MPI_COMM_WORLD);
	t1 = MPI_Wtime();

	myMatMat(C, A, B, N);

	MPI_Barrier(MPI_COMM_WORLD);
	t2 = MPI_Wtime();
	t0 =  t2 - t1;
	MPI_Reduce(&t0, &t_w, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	/* End of mat-vec routine --------------------------- */

	if (myid == 0) {
		d_mflops = 2.0*(double)N*(double)N*(double)N/t_w;
		d_mflops = d_mflops * 1.0e-6;
		printf("%d [dim.], %lf [sec.], %lf [Mflops]\n",N, t_w, d_mflops);
	}

	MPI_Finalize();
	return 0;
}

void myMatMat(double C[N/numprocs][N], double A[N/numprocs][N], double B[N][N/numprocs], int n)
{
	int i, j, k, l;
	int dest, src;
	MPI_Status istat;
	int i_start, i_end;
	int j_start;
	double B_T[N][np];

	dest = (myid == 0) ? (numprocs-1) : (myid-1);
	src  = (myid == numprocs-1) ?   0 : (myid+1);
	i_start = myid * np;
	i_end = i_start + np;
	for (l = 0; l < numprocs; l++) {
		/* np x np */
		j_start = np * ((myid+l)%numprocs);
		for (i = 0; i < np; i++) {
			for (j = 0; j < np; j++) {
				for (k = 0; k < n; k++) {
					C[i][j_start+j] += A[i][k] * B[k][j];
				}
			}
		}
		/* communication */
		if (l != numprocs -1) {
			if (myid % 2 == 0) {
				MPI_Send(B,   np * n, MPI_DOUBLE, dest,          l, MPI_COMM_WORLD);
				MPI_Recv(B_T, np * n, MPI_DOUBLE,  src, l+numprocs, MPI_COMM_WORLD, &istat);
			} else {
				MPI_Recv(B_T, np * n, MPI_DOUBLE,  src,          l, MPI_COMM_WORLD, &istat);
				MPI_Send(B,   np * n, MPI_DOUBLE, dest, l+numprocs, MPI_COMM_WORLD);
			}
			/* copy B_T to B*/
			for (i = 0; i < n; i++) {
				for (j = 0; j < np; j++) {
					B[i][j] = B_T[i][j];
				}
			}
		}
	}
}
