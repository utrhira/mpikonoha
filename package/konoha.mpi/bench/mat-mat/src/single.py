import random, numpy
from mpi4py import MPI

N =     480

world = MPI.COMM_WORLD
myid = world.Get_rank()
numprocs = world.Get_size()

def main():
    a = range(N*N)
    b = range(N*N)
    c = range(N*N)

    # matrix generation --------------------------
    for j in xrange(0, N):
        for i in xrange(0, N):
            a[j*N+i] = random.random()
            b[j*N+i] = random.random()
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
        d_mflops = 2.0 * N * N * N / t_w
        d_mflops = d_mflops * 1.0e-6
        print N,"[dim.],","%f" % t_w,"[sec.],","%f" % d_mflops,"[Mflops]"


def myMatMat(c, a, b, n):
    for i in xrange(0, n):
        for j in xrange(0, n):
            cidx = i * n + j
            for k in xrange(0, n):
                c[cidx] += a[i * n + k] * b[k * n + j]

if __name__ == '__main__':
    main()
