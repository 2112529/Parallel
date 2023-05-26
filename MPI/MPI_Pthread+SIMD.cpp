#include <malloc.h>
#include <mpi.h>
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <sys/time.h> // gettimeofday
#include <arm_neon.h>

using namespace std;

int n_threads = 8;
pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;
typedef struct {
    int t_id;
} threadParam_t;

const int MAX_N = 512;
float A[MAX_N][MAX_N];
int k = 0;
int i;
int n;

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

void neon_elimination(float* A_i, float* A_k, float factor, int n) {
    float32x4_t factor_v = vdupq_n_f32(factor);
    for (int j = 0; j < n; j += 4) {
        float32x4_t A_i_v = vld1q_f32(A_i + j);
        float32x4_t A_k_v = vld1q_f32(A_k + j);
        float32x4_t result = vmlaq_f32(A_i_v, factor_v, A_k_v);
        vst1q_f32(A_i + j, result);
    }
}

void* threadFunc(void* param) {
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;

    for (int k = 0; k < n; k++) {
        if (t_id == 0) {
            for (int j = k + 1; j < n; j++)
                A[k][j] /= A[k][k];
            A[k][k] = 1.0;
        }
        pthread_barrier_wait(&barrier_Division);
        for (int i = k + 1 + t_id; i < n; i += n_threads) {
            neon_elimination(A[i] + k + 1, A[k] + k + 1, A[i][k] / A[k][k], n - k - 1);
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
    int myid = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    // 获取矩阵规模大小N
    cout << "Enter the matrix size (N): ";
    cin >> n;
    if (n <= 0 || n > MAX_N) {
        cout << "Invalid matrix size. Please enter a value between 1 and " << MAX_N << "." << endl;
        MPI_Finalize();
        return 1;
    }

    // 初始化矩阵A
    init();

    int tmp = (n - n % n_threads) / n_threads;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;

    pthread_barrier_init(&barrier_Division, NULL, n_threads);
    pthread_barrier_init(&barrier_Elimination, NULL, n_threads);
    pthread_t handles[n_threads];
    threadParam_t param[n_threads];

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
                for (int t_id = 0; t_id < n_threads; ++t_id) {
                    param[t_id].t_id = t_id;
                    pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
                }
                for (int t_id = 0; t_id < n_threads; ++t_id) {
                    pthread_join(handles[t_id], NULL);
                }
                A[i][k] = 0;
            }
        }
        if (r1 >= k + 1) {
            for (i = r1; i <= r2 && i < n; i++) {
                for (int t_id = 0; t_id < n_threads; ++t_id) {
                    param[t_id].t_id = t_id;
                    pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
                }
                for (int t_id = 0; t_id < n_threads; ++t_id) {
                    pthread_join(handles[t_id], NULL);
                }
                A[i][k] = 0;
            }
        }
    }
    if (myid == 0) {
        double te = MPI_Wtime();
        cout << "N: " << n << ", Time: " << te - ts << "s" << endl;
        //Print();
    }
    pthread_barrier_destroy(&barrier_Division);
    pthread_barrier_destroy(&barrier_Elimination);
    MPI_Finalize();
    return 0;
}
