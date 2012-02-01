#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#ifndef  N
#define  N      256
#endif
#define  DEBUG  0
#define  EPS    1.0e-18

/* Please define the matrices in here */
static double  A[N][N];
static double  B[N][N];
static double  C[N][N];

#ifdef BTRANS
static double tB[N][N];
#endif

int     myid, numprocs;

void MyMatMat(double [N][N], double [N][N], double [N][N], int);

int main(int argc, char* argv[])
{
	double  t0, t1, t2, t_w;
	double  dc_inv, d_mflops;

	int     ierr;
	int     i, j;
	int     iflag, iflag_t;
	int     j_start, j_end;

	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	/* matrix generation --------------------------*/
	if (DEBUG == 1) {
		for(j=0; j<N; j++) {
			for(i=0; i<N; i++) {
				A[j][i] = 1.0;
				B[j][i] = 1.0;
				C[j][i] = 0.0;
			}
		}
	} else {
		srand(1);
		dc_inv = 1.0/(double)RAND_MAX;
		for(j=0; j<N; j++) {
			for(i=0; i<N; i++) {
				A[j][i] = rand()*dc_inv;
				B[j][i] = rand()*dc_inv;
				C[j][i] = 0.0;
			}
		}
	} /* end of matrix generation --------------------------*/

	/* Start of mat-vec routine ----------------------------*/
	ierr = MPI_Barrier(MPI_COMM_WORLD);
	t1 = MPI_Wtime();

	MyMatMat(C, A, B, N);

	ierr = MPI_Barrier(MPI_COMM_WORLD);
	t2 = MPI_Wtime();
	t0 =  t2 - t1;
	ierr = MPI_Reduce(&t0, &t_w, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	/* End of mat-vec routine --------------------------- */

	if (myid == 0) {

		printf("N  = %d \n",N);
		printf("Mat-Mat time  = %lf [sec.] \n",t_w);

		d_mflops = 2.0*(double)N*(double)N*(double)N/t_w;
		d_mflops = d_mflops * 1.0e-6;
		printf(" %lf [MFLOPS] \n", d_mflops);
	}

	if (DEBUG == 1) {
		/* Verification routine ----------------- */
		iflag = 0;
		j_start = N/numprocs * myid;
		j_end = j_start + N/numprocs;
		for(j=0; j<N; j++) {
			for(i=0; i<N; i++) {
				if (fabs(C[j][i] - (double)N) > EPS) {
					printf(" Error! in ( %d , %d ) th argument. \n",j, i);
					iflag = 1;
					break;
				}
			}
		}
		/* ------------------------------------- */

		MPI_Reduce(&iflag, &iflag_t, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		if (myid == 0) {
			if (iflag_t == 0) printf(" OK! \n");
		}

	}

	ierr = MPI_Finalize();

	return 0;
}

void MyMatMat(double C[N][N], double A[N][N], double B[N][N], int n)
{
	int  i, j, k;
	for(i=0; i<n; i++) {
		for(j=0; j<n; j++) {
			for(k=0; k<n; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}
