#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <cblas.h>

#define  N      480

int     myid, numprocs;

void myMatMat(double [N][N], double [N][N], double [N][N], int);

int main(int argc, char* argv[])
{
	double  t0, t1, t2, t_w;
	double  dc_inv, d_mflops;

	int     i, j;
	int     iflag, iflag_t;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	double  A[N][N];
	double  B[N][N];
	double  C[N][N];

	/* matrix generation --------------------------*/
	srand(1);
	dc_inv = 1.0/(double)RAND_MAX;
	for(j=0; j<N; j++) {
		for(i=0; i<N; i++) {
			A[j][i] = rand()*dc_inv;
			B[j][i] = rand()*dc_inv;
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

void myMatMat(double C[N][N], double A[N][N], double B[N][N], int n)
{
	/*
	  cblas.dgemm: Matrix-Matrix-Mul
	     C = beta * C + alpha * (A * B)
	         A, B, C: Matrix (2-dim array vector)
	         alpha, beta: Scalar
	*/
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
				n /* A.rows */, n /* B.rows */, n /* C.rows */,
				1.0 /* alpha */, A, n /* A.cols */, B, n /* B.cols */, 0.0 /* beta */, C, n /* C.cols */);
}
