using konoha.mpi.*;

N =     480;

world = MPIComm.WORLD;
myid = world.getRank();
numprocs = world.getSize();
np   = N/numprocs;

void myMatMat(double[] c, double[] a, double[] b, int n);

void main(String[] args) {
    double[] a = new double[np, N];
    double[] b = new double[N, np];
    double[] c = new double[np, N];

    /* matrix generation --------------------------*/
    for(int j=0; j<np; j++) {
        for(int i=0; i<N; i++) {
            a[j,i] = Float.random();
            b[i,j] = Float.random();
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
    b_t = new double[n, np];
    dest = (myid == 0) ? (numprocs-1) : (myid-1);
    src  = (myid == numprocs-1) ?   0 : (myid+1);
    i_start = myid * np;
    i_end = i_start + np;
    for (int l = 0; l < numprocs; l++) {
        /* np x np */
        j_start = np * ((myid+l)%numprocs);
        for (int i = 0; i < np; i++) {
            for (int j = 0; j < np; j++) {
                cidx = i * n + (jstart + j);
                for (int k = 0; k < n; k++) {
                    //c[i, jstart+j] += a[i, k] * b[k, j];
                    c[cidx] += a[i * n + k] * b[k * np + j];
                }
            }
        }
        /* communication */
        if (l != numprocs -1) {
            if (myid % 2 == 0) {
                world.send((MPIData)b, np * n, dest, l);
                world.recv((MPIData)b_t, np * n, src, l+numprocs);
            } else {
                world.recv((MPIData)b_t, np * n, src, l);
                world.send((MPIData)b, np * n, dest, l+numprocs);
            }
            /* copy B_T to B*/
            for (i = 0; i < n; i++) {
                for (j = 0; j < np; j++) {
                    //b[i, j] = b_t[i, j];
                    bidx = i * np + j;
                    b[bidx] = b_t[bidx];
                }
            }
        }
    }
}
