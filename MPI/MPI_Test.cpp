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
const int N = 5010;
float A[N][N];
typedef long long LARGER_INTERGER;
long long head, tail, freq;
//初始化
void init() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++)
            A[i][j] = 0;
        A[i][i] = 1.0;
        for (int j = i + 1; j < n; j++)
            A[i][j] = (i + j) % 100;
    }
    for (int i = 0; i < n; i++) {
        int k1 = rand() % n;
        int k2 = rand() % n;
        for (int j = 0; j < n; j++) {
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
void MPI_Gauss()
{
    int myid = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &NUMS_THREADS);//设置进程数为n_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//识别调用进程的rank，值从0到size-1

    //初始化矩阵A
    init();

    //计时开始
    //double time_start = MPI_Wtime();
    double tm_MPI;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    int tmp = (n - n % NUMS_THREADS) / NUMS_THREADS;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    //逐行作为主行元素，进行初等行变换
    for (int k = 0; k < n; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < n; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
            for (int j = 0; j < NUMS_THREADS; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], N, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], n, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                for (int j = k + 1; j < n; j++) {
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < n; i++) {
                for (int j = k + 1; j < n; j++) {
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
    //MPI_Finalize();
}
void MPI_DIV_2D()
{
    int myid = 0;
    //MPI_Init(&argc, &argv);//MPI进行必要的初始化工作
    MPI_Comm_size(MPI_COMM_WORLD, &NUMS_THREADS);//设置进程数为n_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//识别调用进程的rank，值从0到size-1

    //初始化矩阵A
    init();//question:是分别调用init吗？

    //计时开始
    double ts = MPI_Wtime();

    int tmp = (N - N % NUMS_THREADS) / NUMS_THREADS;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    //逐行作为主行元素，进行初等行变换
    for (int k = 0; k < N; k++) {
        for (int i = k; i < N; i++) {
            if (i < N / 3 && k < N / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
            }
            if (i < N / 3 && k >= N * 2 / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 1, MPI_COMM_WORLD);
            }
            if (i >= N / 3 && i < N * 2 / 3 && k < N / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 2, MPI_COMM_WORLD);
            }
            if (i >= N / 3 && i < N * 2 / 3 && k >= N / 3 && k < N * 2 / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 3, MPI_COMM_WORLD);
            }
            if (i >= N / 3 && i < N * 2 / 3 && k >= N * 2 / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 4, MPI_COMM_WORLD);
            }
            if (i >= N * 2 / 3 && k < N / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 5, MPI_COMM_WORLD);
            }
            if (i >= N * 2 / 3 && k >= N / 3 && k < N * 2 / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 6, MPI_COMM_WORLD);
            }
            if (i >= N * 2 / 3 && k >= N * 2 / 3) {
                MPI_Bcast(&(A[i][k]), 1, MPI_FLOAT, 7, MPI_COMM_WORLD);
            }
            
        }
        for (int j = k + 1; j < N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int j = k + 1; j < N; j++) {
            if (k < N / 3 && j < N / 2) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
            }
            if (k < N / 3 && j >= N * 2 / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 1, MPI_COMM_WORLD);
            }
            if (k >= N / 3 && k < N * 2 / 3 && j < N / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 2, MPI_COMM_WORLD);
            }
            if (k >= N / 3 && k < N * 2 / 3 && j >= N / 3 && j < N * 2 / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 3, MPI_COMM_WORLD);
            }
            if (k >= N / 3 && k < N * 2 / 3 && j >= N * 2 / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 4, MPI_COMM_WORLD);
            }
            if (k >= N * 2 / 3 && j < N / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 5, MPI_COMM_WORLD);
            }
            if (k >= N * 2 / 3 && j >= N / 3 && j < N * 2 / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 6, MPI_COMM_WORLD);
            }
            if (k >= N * 2 / 3 && j >= N * 2 / 3) {
                MPI_Bcast(&(A[k][j]), 1, MPI_FLOAT, 7, MPI_COMM_WORLD);
            }
        }
        int i = k + 1;
        if (myid == 0) {
            if (k + 1 >= N / 3) continue;
            for (int i = k + 1; i < N / 3; i++) {
                for (int j = k + 1; j < N * 2 / 3; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 1) {
            if (k + 1 >= N / 3) continue;
            for (int i = k + 1; i < N / 3; i++) {
                for (int j = N * 2 / 3; j < N; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 2) {
            if (k + 1 >= N / 3) continue;
            for (int i = N / 3; i < N * 2 / 3; i++) {
                for (int j = k + 1; j < N / 3; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 3) {
            if (k + 1 >= N * 2 / 3) continue;
            if (k + 1 <= N / 3) {
                for (int i = N / 3; i < N * 2 / 3; i++) {
                    for (int j = N / 3; j < N * 2 / 3; j++)
                        A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
            }
            else {
                for (int i = k + 1; i < N * 2 / 3; i++) {
                    for (int j = k + 1; j < N * 2 / 3; j++)
                        A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
            }
        }
        if (myid == 4) {
            if (k + 1 >= N * 2 / 3) continue;
            for (int i = k + 1; i < N * 2 / 3; i++) {
                for (int j = N * 2 / 3; j < N; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 5) {
            if (k + 1 >= N / 3) continue;
            for (int i = N * 2 / 3; i < N; i++) {
                for (int j = k + 1; j < N / 3; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 6) {
            if (k + 1 >= N * 2 / 3) continue;
            for (int i = N * 2 / 3; i < N; i++) {
                for (int j = k + 1; j < N * 2 / 3; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
        if (myid == 7) {
            for (int i = N * 2 / 3; i < N; i++) {
                for (int j = N * 2 / 3; j < N; j++)
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }
    //将计算结果汇总到0号进程
    if (myid != 0) {
        //各进程发送自己负责计算的那些行
        //for (int i = myid; i < N; i += n_threads) {
         //   MPI_Send(A[i], N, MPI_FLOAT, 0, i, MPI_COMM_WORLD);
        //}
    }
    else {
        // 0号进程依次接收
        //for (int i = 0; i < N; i++) {
         //   if (i % n_threads != 0) {
          //      MPI_Recv(A[i], N, MPI_FLOAT, i % n_threads, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         //   }
      //  }

        //计时结束
        double te = MPI_Wtime();
        cout << "MPI_DIV_2D"<< " Time:" << te - ts << "s";
        //Print();
    }
    //MPI_Finalize();
}
void MPI_DIV_Col()
{
    int myid = 0;
    //MPI_Init(&argc, &argv);//MPI进行必要的初始化工作
    MPI_Comm_size(MPI_COMM_WORLD, &NUMS_THREADS);//设置进程数为n_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//识别调用进程的rank，值从0到size-1

    //初始化矩阵A
    init();//question:是分别调用init吗？

    //计时开始
    double tm_MPI_DIV_Col;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    int tmp = (n - n % NUMS_THREADS) / NUMS_THREADS;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    //逐行作为主行元素，进行初等行变换
    for (int k = 0; k < n; k++) {
        //同步当前主行元素到所有进程
        MPI_Bcast(A[k], n, MPI_FLOAT, k % NUMS_THREADS, MPI_COMM_WORLD);

        //各进程将自己所负责的行完成行初等变换
        int j = k + 1;
        while ((j - NUMS_THREADS * (j / NUMS_THREADS)) != myid) j++;
        for (; j < n; j += NUMS_THREADS) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                for (int j = k + 1; j < n; j++) {
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < n; i++) {
                for (int j = k + 1; j < n; j++) {
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                }
                A[i][k] = 0;
            }
        }
    }
    //将计算结果汇总到0号进程
    if (myid != 0)
    {
    }
    else {
        //计时结束
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        tm_MPI_DIV_Col = double((tail - head) * 1000.000 / freq);
        cout << tm_MPI_DIV_Col << endl;
        std::cout << "MPI_DIV_Col_Gauss time: " << std::fixed << std::setprecision(2) << tm_MPI_DIV_Col << "ms\n" << endl;;
    }
}
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);//MPI进行必要的初始化工作
    for (int i = 0; i < 10; i++)
    {
        cin >> n;
        cout << "Scale= " << n << " " << endl;
        

        double tm_normal = 0;
        cout << endl;
        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        init();
        Normal_Gauss();
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        //cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
        tm_normal = double((tail - head) * 1000.000 / freq);
        cout << tm_normal << endl;
        std::cout << "Normal_Gauss time: " << std::fixed << std::setprecision(2) << tm_normal << "ms\n" << endl;;




        //尝试多次算数取平均值！！！

        
        
        MPI_Gauss();
        //MPI_DIV_Col();
        //MPI_DIV_2D();

        
    }
    MPI_Finalize();
    return 0;
}
