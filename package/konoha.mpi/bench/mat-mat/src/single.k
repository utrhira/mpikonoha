using konoha.mpi.*;

N =     480;

world = MPIComm.WORLD;
myid = world.getRank();
numprocs = world.getSize();

void myMatMat(double[] c, double[] a, double[] b, int n);

void main(String[] args) {
    a = new double[N,N];
    b = new double[N,N];
    c = new double[N,N];

    /* matrix generation --------------------------*/
    for(int j=0; j<N; j++) {
        for(int i=0; i<N; i++) {
            a[j,i] = Float.random();
            b[j,i] = Float.random();
            c[j,i] = 0.0;
        }
    } /* end of matrix generation --------------------------*/

    /* Start of mat-vec routine ----------------------------*/
    world.barrier();
    t1 = MPI.getWtime();

    myMatMat(c, a, b, N);

    world.barrier();
    t2 = MPI.getWtime();
    t0 =  t2 - t1;
    t_w = new float[1];
    world.reduce((MPIData)t0, (MPIData)t_w, 1, MPIOp.MAX, 0);
    /* End of mat-vec routine --------------------------- */

    if (myid == 0) {
        OUT.println("N  = "+N);
        OUT.println("Mat-Mat time  = "+"%f"(t_w[0])+" [sec.]");

        d_mflops = 2.0 * (double)N * (double)N * (double)N / t_w[0];
        d_mflops = d_mflops * 1.0e-6;
        OUT.println(" "+"%f"(d_mflops)+" [MFLOPS]");
    }
}

void myMatMat(double[] c, double[] a, double[] b, int n)
{
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            cidx = i * n + j;
            for(int k=0; k<n; k++) {
                //c[i,j] += a[i,k] * b[k,j];
                c[cidx] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}
