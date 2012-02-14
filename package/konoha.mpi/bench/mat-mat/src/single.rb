N =     480

$world = MPI::Comm::WORLD
$myid = $world.rank()
$numprocs = $world.size()

def main
    a = Array.new(N*N)
    b = Array.new(N*N)
    c = Array.new(N*N)

    # matrix generation --------------------------
    for j in 0 .. N
        for i in 0 .. N
            a[j*N+i] = rand(0)
            b[j*N+i] = rand(0)
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
    for i in 0 .. n
        for j in 0 .. n
            cidx = i * n + j
            for k in 0 .. n
                #c[i,j] += a[i,k] * b[k,j];
                c[cidx] += a[i * n + k] * b[k * n + j]
            end
        end
    end
end

if __FILE__ == $0
  main()
end
