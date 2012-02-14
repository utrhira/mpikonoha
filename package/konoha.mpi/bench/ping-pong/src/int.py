# *                  pong.c Generic Benchmark code
# *               Dave Turner - Ames Lab - July of 1994+++
# *
# *  Most Unix timers can't be trusted for very short times, so take this
# *  into account when looking at the results.  This code also only times
# *  a single message passing event for each size, so the results may vary
# *  between runs.  For more accurate measurements, grab NetPIPE from
# *  http://www.scl.ameslab.gov/ .
# *
import numpy, math
from mpi4py import MPI

def main():
    max_rate = 0.0
    min_latency = 10e6

    world = MPI.COMM_WORLD
    nprocs = world.Get_size()
    myproc = world.Get_rank()

    if nprocs != 2: exit(1)
    other_proc = (myproc + 1) % 2

    print "Hello from",myproc,"of",nprocs
    world.Barrier()

    # Timer accuracy test

    t0 = MPI.Wtime()
    t1 = MPI.Wtime()

    while t1 == t0: t1 = MPI.Wtime()

    if myproc == 0:
        print "Timer accuracy of ~"+"%f" % ((t1 - t0) * 1000000)+" usecs"

# * Communications between nodes 
# *   - Blocking sends and recvs
# *   - No guarantee of prepost, so might pass through comm buffer
# *

    world.Barrier()
    for n in xrange(18):
        size = 8 * int(math.pow(2, n))
        if size > 1048576: break
        a = numpy.zeros(size/8, 'l')
        b = numpy.zeros(size/8, 'l')
        for i in xrange(0, size/8):
            a[i] = long(i)

        last = size / 8 - 1

        world.Barrier()
        t0 = MPI.Wtime()
        if myproc == 0:

            world.Send([a, MPI.LONG], other_proc)
            world.Recv([b, MPI.LONG], other_proc)

        else:

            world.Recv([b, MPI.LONG], other_proc)
            b[0] += 1
            if last != 0:
                b[last] += 1

            world.Send([b, MPI.LONG], other_proc)

        t1 = MPI.Wtime()
        time = 1.e6 * (t1 - t0)
        world.Barrier()

        if b[0] != 1 or b[last] != long(last + 1):
            print "ERROR - b[0] =", b[0], "b[" + "%d" % last + "] =", b[last]
            exit(1)

        for i in xrange(1, last-1):
            if b[i] != long(i):
                print "ERROR - b[" + "%d" % i + "] =", b[i]

        if myproc == 0 and time > 0.000001:
            print "","%7d" % size, "bytes took", "%9.0f" % time, "usec ("+"%8.3f" % (2.0 * size / time), "MB/sec)"
            if (2 * size / time) > max_rate: max_rate = 2 * size / time
            if (time / 2) < min_latency: min_latency = time / 2
        elif myproc == 0:
            print "", "%7d" % size, "bytes took less than the timer accuracy"

# * Async communications
# *   - Prepost receives to guarantee bypassing the comm buffer
# *

    world.Barrier()
    if myproc == 0: print "\n  Asynchronous ping-pong\n"

    for n in xrange(18):
        size = 8 * int(math.pow(2, n))
        if size > 1048576: break
        a = numpy.zeros(size/8, 'l')
        b = numpy.zeros(size/8, 'l')
        for i in xrange(0, size/8):
            a[i] = long(i)

        last = size / 8 - 1

        request = world.Irecv([b, MPI.LONG], other_proc)
        world.Barrier()
        t0 = MPI.Wtime()

        if myproc == 0:

            world.Send([a, MPI.LONG], other_proc)
            request.Wait()

        else:

            request.Wait()

            b[0] += 1
            if last != 0:
                b[last] += 1

            world.Send([b, MPI.LONG], other_proc)

        t1 = MPI.Wtime()

        time = 1.e6 * (t1 - t0)
        world.Barrier()

        if b[0] != 1 or b[last] != long(last + 1):
            print "ERROR - b[0] =", b[0], "b[" + "%d" % last + "] =", b[last]

        for i in xrange(1, last - 1):
            if b[i] != long(i):
                print "ERROR - b[" + "%d" % i + "] =", b[i]
        if myproc == 0 and time > 0.000001:
            print "", "%7d" % size, "bytes took", "%9.0f" % time, "usec (" + "%8.3f" % (2.0 * size / time), "MB/sec)"
            if 2 * size / time > max_rate: max_rate = 2 * size / time
            if time / 2 < min_latency: min_latency = time / 2
        elif myproc == 0:
            print "", "%7d" % size, "bytes took less than the timer accuracy"

# * Bidirectional communications
# *   - Prepost receives to guarantee bypassing the comm buffer
# *

    world.Barrier()
    if myproc == 0: print "\n  Bi-directional asynchronous ping-pong\n"

    for n in xrange(18):
        size = 8 * int(math.pow(2, n))
        if size > 1048576: break
        a = numpy.zeros(size/8, 'l')
        b = numpy.zeros(size/8, 'l')
        for i in xrange(0, size/8):
            a[i] = long(i)

        last = size / 8 - 1

        request_b = world.Irecv([b, MPI.LONG], other_proc)
        request_a = world.Irecv([a, MPI.LONG], other_proc)

        world.Barrier()

        t0 = MPI.Wtime()

        world.Send([a, MPI.LONG], other_proc)
        request_b.Wait()

        b[0] += 1
        if last != 0:
            b[last] += 1

        world.Send([b, MPI.LONG], other_proc)
        request_a.Wait()

        t1 = MPI.Wtime()
        time = 1.e6 * (t1 - t0)
        world.Barrier()

        if a[0] != 1 or a[last] != long(last + 1):
            print "ERROR - a[0] =", a[0], "a[" + "%d" % last + "] =", a[last]
        for i in xrange(1, last - 1):
            if a[i] != long(i):
                print "ERROR - a[" + "%d" % i + "] =", a[i]
        if myproc == 0 and time > 0.000001:
            print "", "%7d" % size, "bytes took", "%9.0f" % time, "usec (" + "%8.3f" % (2.0 * size / time), "MB/sec)"
            if 2 * size / time > max_rate: max_rate = 2 * size / time
            if time / 2 < min_latency: min_latency = time / 2
        elif myproc == 0:
            print "", "%7d" % size, "bytes took less than the timer accuracy"

    if myproc == 0:
        print "\n Max rate =", max_rate, "MB/sec  Min latency =", min_latency, "usec"


if __name__ == '__main__':
    main()
