N =     480

$world = MPI::Comm::WORLD
$myid = $world.rank()
$numprocs = $world.size()
$np = N/$numprocs

def main
    a = Array.new($np*N)
    b = Array.new(N*$np)
    c = Array.new($np*N)

    # matrix generation --------------------------
    for j in 0 .. $np
        for i in 0 .. N
            a[j*N+i] = rand(0)
            b[i*$np+j] = rand(0)
            c[j*N+i] = 0.0
        end
    end
    # end of matrix generation --------------------------

    # Start of mat-vec routine ----------------------------
    $world.barrier()
    t1 = MPI::wtime()

    myMatMat(c, a, b, N)

    $world.barrier()
    t2 = MPI::wtime()
    t0 =  t2 - t1
    t_w = $world.reduce(t0, MPI::Op::MAX, 0)
    # End of mat-vec routine ---------------------------

    if $myid == 0 then
        d_mflops = 2.0 * N * N * N / t_w
        d_mflops = d_mflops * 1.0e-6
        puts "%d" % N + " [dim.], " + "%f" % t_w + " [sec.], " + "%f" % d_mflops + " [Mflops]"
    end
end

def myMatMat(c, a, b, n)
    b_t = Array.new(n*$np)
    dest = ($myid == 0) ? ($numprocs-1) : ($myid-1)
    src  = ($myid == $numprocs-1) ?   0 : ($myid+1)
    i_start = $myid * $np
    i_end = i_start + $np
    for l in 0 .. $numprocs
        # np x np
        j_start = $np * (($myid+l)%$numprocs)
        for i in 0 .. $np
            for j in 0 .. $np
                cidx = i * n + (j_start + j)
                for k in 0 .. n
                    # c[i, j_start+j] += a[i, k] * b[k, j]
                    c[cidx] += a[i * n + k] * b[k * $np + j]
                end
            end
        end
        # communication
        if l != $numprocs -1 then
            if $myid % 2 == 0 then
                $world.send(b, dest, l);
                b_t, stat = $world.recv(src, l+$numprocs);
            else
                b_t, stat = $world.recv(src, l);
                $world.send(b, dest, l+$numprocs);
            end
            # copy B_T to B
            for i in 0 .. n
                for j in 0 .. $np
                    # b[i, j] = b_t[i, j];
                    bidx = i * $np + j
                    b[bidx] = b_t[bidx]
                end
            end
        end
    end
end

if __FILE__ == $0
    main()
end
