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
        print "N  =",N
        print "Mat-Mat time  =","%f" % t_w,"[sec.]"
        d_mflops = 2.0 * N * N * N / t_w
        d_mflops = d_mflops * 1.0e-6
        print "","%f" % d_mflops,"[MFLOPS]"


def myMatMat(c, a, b, n):
    a_x = numpy.array(a, 'd').reshape(n, n)
    b_x = numpy.array(b, 'd').reshape(n, n)
    c_x = numpy.dot(a_x, b_x)
    c[:] = []
    c.extend(c_x.tolist())

if __name__ == '__main__':
    main()
