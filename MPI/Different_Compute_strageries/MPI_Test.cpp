#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <sys/time.h> //gettimeofday
#include <bits/stdc++.h>
using namespace std;
//进程数
int NUMS_THREADS = 8;
int n;
//方阵
const int N = 512;
float A[N][N];
typedef long long LARGER_INTERGER;
//初始化
void init() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < i; j++)
            A[i][j] = 0;
        A[i][i] = 1.0;
        for (int j = i + 1; j < N; j++)
            A[i][j] = (i + j) % 100;
    }
    for (int i = 0; i < N; i++) {
        int k1 = rand() % N;
        int k2 = rand() % N;
        for (int j = 0; j < N; j++) {
            A[i][j] += A[0][j];
            A[k1][j] += A[k2][j];
        }
    }
}
void Print() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
}
void Normal_Gauss() {
    for (int k = 0; k < n; k++) {
        for (int j = k + 1; j < n; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i = k + 1; i < n; i++) {
            for (int j = k + 1; j < n; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0.0;
        }
    }
    return;
}
int main(int argc, char* argv[]) {

    cin >> n;
    cout << "Scale= " << n << " " << endl;
    init();
    long long head, tail, freq;
    double tm_normal = 0;
    cout << endl;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    Normal_Gauss();
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    //cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
    tm_normal = double((tail - head) * 1000.000 / freq);
    cout << tm_normal << endl;
    std::cout << "Normal_Gauss time: " << std::fixed << std::setprecision(2) << tm_normal << "ms\n" << endl;;


    int myid = 0;
    MPI_Init(&argc, &argv);//MPI进行必要的初始化工作
    MPI_Comm_size(MPI_COMM_WORLD, &NUMS_THREADS);//设置进程数为n_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//识别调用进程的rank，值从0到size-1

    //初始化矩阵A
    init();

    //计时开始
    //double time_start = MPI_Wtime();
    double tm_MPI;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    int tmp = (N - N % NUMS_THREADS) / NUMS_THREADS;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    //逐行作为主行元素，进行初等行变换
    for (int k = 0; k < N; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < N; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
            for (int j = 0; j < NUMS_THREADS; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], N, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], N, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                for (int j = k + 1; j < N; j++) {
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < N; i++) {
                for (int j = k + 1; j < N; j++) {
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
    }
    if (myid == 0) {
        //计时结束
        //double time_end = MPI_Wtime();
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        tm_MPI = double((tail - head) * 1000.000 / freq);
        cout << tm_MPI << endl;
        std::cout << "MPI_Gauss time: " << std::fixed << std::setprecision(2) << tm_MPI << "ms\n" << endl;;
    }
    MPI_Finalize();

    //尝试多次算数取平均值！！！


    




    return 0;
}
