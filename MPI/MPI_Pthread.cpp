#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <semaphore.h>
#include <sys/time.h> //gettimeofday
#define NUM_THREADS 7
using namespace std;
//进程数
int n_threads = 8;
pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;
typedef struct {
    int t_id;//线程id
}threadParam_t;
//方阵
const int N = 1024;
float A[N][N];
int k = 0;
int i;
int n;
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
void* threadFunc(void* param) {
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
    for (int k = 0; k < n; k++)
    {
        if (t_id == 0)
        {
            for (int j = k + 1; j < n; j++)
                A[k][j] /= A[k][k];
            A[k][k] = 1.0;
        }
        //第一个同步点
        pthread_barrier_wait(&barrier_Division);
        //循环划分任务(尝试多种任务划分方式)
        for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
        {
            for (int j = k + 1; j < n; ++j)
            {
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0.0;
        }
        pthread_barrier_wait(&barrier_Elimination);


    }
    pthread_exit(NULL);
    return NULL;


}
void Print() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }

}
int main(int argc, char* argv[]) {
    cin >> n;
    int myid = 0;
    MPI_Init(&argc, &argv);//MPI进行必要的初始化工作
    MPI_Comm_size(MPI_COMM_WORLD, &n_threads);//设置进程数为n_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//识别调用进程的rank，值从0到size-1

    //初始化矩阵A
    init();//question:是分别调用init吗？


    int tmp = (n - n % n_threads) / n_threads;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    //创建线程
    pthread_barrier_init(&barrier_Division, NULL, NUM_THREADS);
    pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);
    pthread_t handles[NUM_THREADS];//创建对应的handle
    threadParam_t param[NUM_THREADS];//创建对应的线程数据结构


    //计时开始
    double ts = MPI_Wtime();
    for (k = 0; k < n; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < n; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
            for (int j = 0; j < n_threads; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], n, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], n, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (r2 >= k + 1 && r1 < k + 1) {
            for (i = k + 1; i <= r2; i++) {
                for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
                    param[t_id].t_id = t_id;
                    pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
                }
                for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
                    pthread_join(handles[t_id], NULL);
                }
                A[i][k] = 0;
            }
        }
        if (r1 >= k + 1) {
            for (i = r1; i <= r2 && i < n; i++) {
                for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
                    param[t_id].t_id = t_id;
                    pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
                }
                for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
                    pthread_join(handles[t_id], NULL);
                }
                A[i][k] = 0;
            }
        }
    }
    if (myid == 0) {
        double te = MPI_Wtime();
        cout << endl<< "scale:" << n << ",Time:" << te - ts << "s";
        //Print();
    }
    pthread_barrier_destroy(&barrier_Division);
    pthread_barrier_destroy(&barrier_Elimination);
    MPI_Finalize();
    return 0;
}