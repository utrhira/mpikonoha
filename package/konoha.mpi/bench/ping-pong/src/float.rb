# -*- coding: utf-8 -*-
# *                  pong.c Generic Benchmark code
# *               Dave Turner - Ames Lab - July of 1994+++
# *
# *  Most Unix timers can't be trusted for very short times, so take this
# *  into account when looking at the results.  This code also only times
# *  a single message passing event for each size, so the results may vary
# *  between runs.  For more accurate measurements, grab NetPIPE from
# *  http://www.scl.ameslab.gov/ .
# *

def main
    max_rate = 0.0
    min_latency = 1.0 * 10**6

    world = MPI::Comm::WORLD
    nprocs = world.size()
    myproc = world.rank()

    if nprocs != 2 then exit(1) end
    other_proc = (myproc + 1) % 2

    puts "Hello from " + "%d" % myproc + " of " + "%d" % nprocs
    world.barrier()

    # Timer accuracy test

    t0 = MPI::wtime()
    t1 = MPI::wtime()

    while t1 == t0 do t1 = MPI::wtime() end

    if myproc == 0 then
        puts "Timer accuracy of ~"+"%f" % ((t1 - t0) * 1000000)+" usecs"
    end

# * Communications between nodes 
# *   - Blocking sends and recvs
# *   - No guarantee of prepost, so might pass through comm buffer
# *

    world.barrier()
    for n in 0 .. 18
        size = 8 * (2 ** n)
        if size > 1048576 then break end
        a = Array.new(size/8)
        # b = Array.new(size/8)
        for i in 0 .. (size/8)
            a[i] = i.to_f
        end

        last = size / 8 - 1

        world.barrier()
        t0 = MPI::wtime()
        if myproc == 0 then

            world.send(a, other_proc, 0)
            b, stat = world.recv(other_proc, 0)

        else

            b, stat = world.recv(other_proc, 0)
            b[0] += 1.0
            if last != 0 then
                b[last] += 1.0
            end
            world.send(b, other_proc, 0)

        end
        t1 = MPI::wtime()
        time = 1.0 * 10**6 * (t1 - t0)
        world.barrier()

        if b[0] != 1.0 or b[last] != (last + 1).to_f then
            puts "ERROR - b[0] = "+ "%f" % b[0]+ " b[" + "%d" % last + "] = " + "%f" % b[last]
            exit(1)
        end

        for i in 1 .. (last-1)
            if b[i] != i.to_f then
                puts "ERROR - b[" + "%d" % i + "] = " + "%f" % b[i]
            end
        end

        if myproc == 0 and time > 0.000001 then
            puts " " + "%7d" % size + " bytes took " + " %9.0f" % time + " usec (" + "%8.3f" % (2.0 * size / time) + " MB/sec)"
            if (2 * size / time) > max_rate then max_rate = 2 * size / time end
            if (time / 2) < min_latency then min_latency = time / 2 end
        elsif myproc == 0 then
            puts " " + "%7d" % size + " bytes took less than the timer accuracy"
        end
    end

    if myproc == 0 then
        puts "\n Max rate = " + "%f" % max_rate + " MB/sec  Min latency = " + "%f" % min_latency + "usec"
    end
    return
    
# * Async communications
# *   - Prepost receives to guarantee bypassing the comm buffer
# *

    world.barrier()
    if myproc == 0 then puts "\n  Asynchronous ping-pong\n" end

    for n in 0 .. 18
        size = 8 * (2 ** n)
        if size > 1024 then break end
        a = Array.new(size/8)
        # b = Array.new(size/8)
        for i in 0 .. (size/8)
            a[i] = i.to_f
        end

        last = size / 8 - 1

        request = world.irecv(other_proc, 0)
        world.barrier()
        t0 = MPI::wtime()

        if myproc == 0 then

            world.send(a, other_proc, 0)
            request.wait
            b = request.object

        else

            request.wait
            b = request.object

            b[0] += 1.0
            if last != 0 then
                b[last] += 1.0
            end

            world.send(b, other_proc, 0)
        end

        t1 = MPI::wtime()

        time = 1.0 * 10**6 * (t1 - t0)
        world.barrier()

        if b[0] != 1.0 or b[last] != (last + 1).to_f then
            puts "ERROR - b[0] = " + "%f" % b[0] + " b[" + "%d" % last + "] = " + "%f" % b[last]
        end

        for i in 1 .. (last - 1)
            if b[i] != i.to_f then
                print "ERROR - b[" + "%d" % i + "] = " + "%f" % b[i]
            end
        end
        if myproc == 0 and time > 0.000001 then
            puts " " + "%7d" % size + " bytes took " + "%9.0f" % time + " usec (" + "%8.3f" % (2.0 * size / time) + " MB/sec)"
            if 2 * size / time > max_rate then max_rate = 2 * size / time end
            if time / 2 < min_latency then min_latency = time / 2 end
        elsif myproc == 0 then
            puts " " + "%7d" % size + " bytes took less than the timer accuracy"
        end
    end

# * Bidirectional communications
# *   - Prepost receives to guarantee bypassing the comm buffer
# *

    world.barrier()
    if myproc == 0 then puts "\n  Bi-directional asynchronous ping-pong\n" end

    for n in 0 .. 18
        size = 8 * (2 ** n)
        # if size > 1048576 then break end
        if size > 1024 then break end
        a = Array.new(size/8)
        # b = Array.new(size/8)
        for i in 0 .. (size/8)
            a[i] = i.to_f
        end

        last = size / 8 - 1

        request_b = world.irecv(other_proc, 0)
        request_a = world.irecv(other_proc, 0)

        world.barrier()

        t0 = MPI::wtime()

        world.send(a, other_proc, 0)
        request_b.wait
        b = request_b.object

        b[0] += 1.0
        if last != 0 then
            b[last] += 1.0
        end

        world.send(b, other_proc, 0)
        request_a.wait
        a = request_a.object

        t1 = MPI::wtime()
        time = 1.0 * 10**6 * (t1 - t0)
        world.barrier()

        if a[0] != 1.0 or a[last] != (last + 1).to_f then
            puts "ERROR - a[0] = " + "%f" % a[0] + " a[" + "%d" % last + "] = " + "%f" % a[last]
        end
        for i in 1 .. (last - 1)
            if a[i] != i.to_f then
                puts "ERROR - a[" + "%d" % i + "] = " + "%f" % a[i]
            end
        end
        if myproc == 0 and time > 0.000001 then
            puts " " + "%7d" % size + " bytes took " + "%9.0f" % time + " usec (" + "%8.3f" % (2.0 * size / time) + " MB/sec)"
            if 2 * size / time > max_rate then max_rate = 2 * size / time end
            if time / 2 < min_latency then min_latency = time / 2 end
        elsif myproc == 0 then
            puts " " + "%7d" % size + " bytes took less than the timer accuracy"
        end
    end

    if myproc == 0 then
        puts "\n Max rate = " + "%f" % max_rate + " MB/sec  Min latency = " + "%f" % min_latency + "usec"
    end
end

if __FILE__ == $0
    main()
end
