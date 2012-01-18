using konoha.math.*;
using konoha.mpi.*;

class Complex(double re, double im);

DIR_FORWARD = 0;
DIR_BACKWARD = 1;

double starttime;

void rearrange(int size, Complex[] data) {

    /* テーブルサイズ測定 */
    a = 1;
    b = size >> 1;
    while (a < b) {
        a <<= 1;
        b >>= 1;
    }

    table = new int[a];

    /* テーブルデータ生成 */
    table[0] = 0;
    a = 1;
    b = size >> 1;
    while (a < b) {
        for (i = 0; i < a; i++)
            table[a + i] = table[i] + b;

        a <<= 1;
        b >>= 1;
    }

    /* 並び換え */
    if (a == b) { /* 桁数が奇数の場合 */
        for (int i = 1; i < a; i++) {
            for (int j = 0; j < i; j++) {
                n1 = table[i] + j;
                n2 = i + table[j];
                if (n1 > n2) {
                    tmp = data[n1];
                    data[n1] = data[n2];
                    data[n2] = tmp;

                    tmp = data[n1 + a];
                    data[n1 + a] = data[n2 + a];
                    data[n2 + a] = tmp;
                }
            }
        }
    } else { /* 桁数が偶数の場合 */
        for (i = 1; i < a; i++) {
            for (j = 0; j < i; j++) {
                n1 = table[i] + j;
                n2 = i + table[j];
                if (n1 > n2) {
                    tmp = data[n1];
                    data[n1] = data[n2];
                    data[n2] = tmp;
                }
            }
        }
    }
}


/* w[k]=exp(2 * pi * k * n / N) の計算 */
void calc_w(int size, Complex[] w, int dir) {
    if (dir == DIR_FORWARD) {
        for (int i = 0; i <= size / 4; i++) {
            w[i].re = Math.cos(i * 2 * Math.PI / size);
            w[size / 4 - i].im = w[size / 4 + i].im = w[size / 2 - i].re = -w[i].re;
        }
    } else {
        for (int i = 0; i <= size / 4; i++) {
            w[size / 4 - i].im = w[size / 4 + i].im = w[i].re = Math.cos(i * 2 * Math.PI / size);
            w[size / 2 - i].re = -w[i].re;
        }
    }
}

void fft(int size, int block_size, Complex[] data, Complex[] w) {
    for (int n = 2; n <= block_size; n <<= 1) {
        for (int j = 0; j < block_size; j += n) {
            for (int i = 0; i < n / 2; i++) {
                tmp = new Complex();
                int k = i * (size / n);
                tmp.re = data[j + n/2 + i].re * w[k].re - data[j + n/2 + i].im * w[k].im;
                tmp.im = data[j + n/2 + i].re * w[k].im + data[j + n/2 + i].im * w[k].re;

                data[j + n/2 + i].re = data[j + i].re - tmp.re;
                data[j + n/2 + i].im = data[j + i].im - tmp.im;
                data[j + i].re += tmp.re;
                data[j + i].im += tmp.im;
            }
        }
    }
}

void fft_one(int size, int block_size, Complex[] data1, Complex[] data2, int n, int start, Complex[] w) {
    //complex *exp;

    for (int i = 0; i < block_size / 2; i++) {
        tmp = new Complex();
        int k = (start + i) * (size / n);
        tmp.re = data2[i].re * w[k].re - data2[i].im * w[k].im;
        tmp.im = data2[i].re * w[k].im + data2[i].im * w[k].re;

        data2[i].re = data1[i].re - tmp.re;
        data2[i].im = data1[i].im - tmp.im;
        data1[i].re += tmp.re;
        data1[i].im += tmp.im;
    }
}

void normalize(int size, Complex[] data, int block_size) {
    for (int i = 0; i < block_size; i++) {
        data[i].re = data[i].re / size;
        data[i].im = data[i].im / size;
    }
}

int fft_mpi(int size, Complex[] data, int dir, int myid, int numprocs, MPIComm comm) {
    block_size = size / numprocs;

    /* sizeとnumprocsは２のべきのみ */
    if ((size & (size - 1)) != 0 ||
        numprocs & (numprocs - 1) != 0)
        return -1;

    w = new Complex[size / 2 + 1];

    calc_w(size, w, dir);
    if (myid == 0) { /* 親PE */
        print "calc_w end:" + (MPI.getWtime() - starttime);

        /* 並び替えた上で送信 */
        rearrange(size, data);

        print "rearrange end:" + (MPI.getWtime() - starttime);

        for (int i = 1; i < numprocs; i++) {
            comm.send((MPIData)data + (i * block_size), 2 * block_size, i, 0);
        }

        print "Data send:"+ (MPI.getWtime() - starttime);
    } else { /* 子PE */
        data = new Complex[block_size * 3 / 2];

        /* データの受信 */
        comm.recv((MPIData)data, 2 * block_size, 0, 0);
    }

    /* それぞれ担当部分を計算 */
    if (dir == DIR_BACKWARD)
        normalize(size, data, block_size);

    fft(size, block_size, data, w);

    if (myid == 0)
        print "block data translated:" + (MPI.getWtime() - starttime);

    /* データを交換して計算 */
    data2 = data[(block_size / 2) to -1];
    data_bak = data[block_size to -1];
    for (int i = 1; i < numprocs; i <<= 1) {
        if ((myid & i) == 0) { /* 後半部分を交換 */
            comm.recv((MPIData)data_bak, block_size, myid + i, 0);
            comm.send((MPIData)data2, block_size, myid + i, 1);

            tmp = data2;
            data2 = data_bak;
            data_bak = tmp;
        } else { /* 前半部分を交換 */
            comm.send((MPIData)data, block_size, myid - i, 0);
            comm.recv((MPIData)data, block_size, myid - i, 1);
        }
        fft_one(size, block_size, data, data2, block_size * 2 * i, (myid % (i * 2)) * block_size / 2, w);
    }
    /* データの統合 */
    if (myid == 0) {
        print "translate end "+ (MPI.getWtime() - starttime);

        if (numprocs > 1 && data2[0] != data[size / 2]) {
            for (int j = 0; j < block_size/2; j++) {
                data[size/2 + j] = data2[j];
            }
        }

        for (int i = 1; i < numprocs; i++) {
            comm.recv((MPIData)data + (i * block_size / 2), block_size, i, 0);
            comm.recv((MPIData)data + (size / 2 + i * block_size / 2), block_size, i, 1);
        }
    } else {
        comm.send((MPIData)data, block_size, 0, 0);
        comm.send((MPIData)data2, block_size, 0, 1);
    }
    return 0;
}

N = 0x1000;

int main(String[] args)
{
    Complex[] data;
    double hash = 0.0;

    world = MPIComm.WORLD;
    myid = world.rank;
    numprocs = world.size;

    if (myid == 0) {
        data = new Complex[N];
        for (int i = 0; i < N; i++) {
            data[i].re = (double)Int.random() / (Int.MAX / 2) - 1;
            data[i].im = (double)Int.random() / (Int.MAX / 2) - 1;
            hash += data[i].re + data[i].im;
        }
    }

    /* 速度測定 */
    if (myid == 0)
        starttime = MPI.getWtime();

//  for (i = 0; i < 100; i++)
    fft_mpi(N, data, DIR_FORWARD, myid, numprocs, world);

    if (myid == 0) {
        print "" + N + "\t" + numprocs + "\t" + (MPI.getWtime() - starttime);
    }
    return 0;
}
