import random, numpy
from mpi4py import MPI

N =     480

world = MPI.COMM_WORLD
myid = world.Get_rank()
numprocs = world.Get_size()
np = N/numprocs

def main():
    a = range(np*N)
    b = range(N*np)
    c = range(np*N)

    # matrix generation --------------------------
    for j in xrange(0, np):
        for i in xrange(0, N):
            a[j*N+i] = random.random()
            b[i*np+j] = random.random()
            c[j*N+i] = 0.0

    # end of matrix generation --------------------------

    # Start of mat-vec routine ----------------------------
    world.Barrier()
    t1 = MPI.Wtime()

    myMatMat(c, a, b, N)

    world.Barrier()
    t2 = MPI.Wtime()
    t0 =  numpy.array(t2 - t1, 'd')
    t_w = numpy.array(0.0, 'd')
    world.Reduce([t0, MPI.DOUBLE], [t_w, MPI.DOUBLE], op=MPI.MAX, root=0)
    # End of mat-vec routine ---------------------------

    if myid == 0:
        print "N  =",N
        print "Mat-Mat time  =","%f" % t_w,"[sec.]"
        d_mflops = 2.0 * N * N * N / t_w
        d_mflops = d_mflops * 1.0e-6
        print "","%f" % d_mflops,"[MFLOPS]"


def myMatMat(c, a, b, n):
    np = n / numprocs
    b_x = numpy.array(b, 'd')
    b_t = numpy.zeros(n*np, 'd')

    dest = (numprocs-1) if (myid == 0) else (myid-1)
    src  = 0 if (myid == numprocs-1) else (myid+1)
    i_start = myid * np
    i_end = i_start + np
    for l in xrange(0, numprocs):
        # np x np
        j_start = np * ((myid+l)%numprocs)
        for i in xrange(0, np):
            for j in xrange(0, np):
                cidx = i * n + (j_start + j)
                for k in xrange(0, n):
                    c[cidx] += a[i * n + k] * b_x[k * np + j]

        # communication
        if l != numprocs -1:
            if myid % 2 == 0:
                world.Send([b_x, MPI.DOUBLE], dest=dest, tag=l)
                world.Recv([b_t, MPI.DOUBLE], source=src, tag=l+numprocs)
            else:
                world.Recv([b_t, MPI.DOUBLE], source=src, tag=l)
                world.Send([b_x, MPI.DOUBLE], dest=dest, tag=l+numprocs)

            # copy B_T to B
            for i in xrange(0, n):
                for j in xrange(0, np):
                    bidx = i * np + j
                    b_x[bidx] = b_t[bidx]


if __name__ == '__main__':
    main()
