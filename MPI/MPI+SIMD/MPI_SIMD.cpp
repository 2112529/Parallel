#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<immintrin.h>
using namespace std;
//������
int n_threads = 8;

//����
const int N = 512;
float A[N][N];

//��ʼ��
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
void MPI_AVX()
{
    int tmp = (N - N % n_threads) / n_threads;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    __m256 t1, t2, t3;
    int j;
    float temp[8];
    // ������Ϊ����Ԫ�أ����г����б任
    for (int k = 0; k < N; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < N; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
            for (int j = 0; j < n_threads; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], N, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], N, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                temp[4] = temp[5] = temp[6] = temp[7] = A[i][k];
                t1 = _mm256_loadu_ps(temp);
                j = k + 1;
                for (; j + 8 <= N; j += 8) {
                    t2 = _mm256_loadu_ps(&A[k][j]);
                    t3 = _mm256_loadu_ps(&A[i][j]);
                    t2 = _mm256_mul_ps(t2, t1);
                    t3 = _mm256_sub_ps(t3, t2);
                    _mm256_storeu_ps(&A[i][j], t3);
                }
                for (; j < N; j++) {
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
                A[i][k] = 0.0;
            }
        }
        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < N; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                temp[4] = temp[5] = temp[6] = temp[7] = A[i][k];
                t1 = _mm256_loadu_ps(temp);
                j = k + 1;
                for (; j + 8 <= N; j += 8) {
                    t2 = _mm256_loadu_ps(&A[k][j]);
                    t3 = _mm256_loadu_ps(&A[i][j]);
                    t2 = _mm256_mul_ps(t2, t1);
                    t3 = _mm256_sub_ps(t3, t2);
                    _mm256_storeu_ps(&A[i][j], t3);
                }
                for (; j < N; j++) {
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
                A[i][k] = 0.0;
            }
        }
    }

    // �����������ܵ�0�Ž���
    if (myid != 0) {
        // �����̷����Լ�����������Щ��
        for (int i = myid; i < N; i += n_threads) {
            MPI_Send(A[i], N, MPI_FLOAT, 0, i, MPI_COMM_WORLD);
        }
    }

    if (myid == 0) {
        // 0�Ž������ν���
        for (int i = 0; i < N; i++) {
            if (i % n_threads != 0) {
                MPI_Recv(A[i], N, MPI_FLOAT, i % n_threads, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // ��ʱ����
        double te = MPI_Wtime();
        cout << "MPI_AVX" << " Time: " << te - ts << "s" << endl;
    }
}
void MPI_Neon()
{
    double ts = MPI_Wtime();
    int tmp = (N - N % n_threads) / n_threads;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;

    float32x4_t t1, t2, t3;
    int j;
    float temp[4];

    for (int k = 0; k < N; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < N; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;

            for (int j = 0; j < n_threads; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], N, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], N, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                t1 = vld1q_f32(temp);
                j = k + 1;

                for (; j + 4 <= N; j += 4) {
                    t2 = vld1q_f32(&A[k][j]);
                    t3 = vld1q_f32(&A[i][j]);
                    t2 = vmulq_f32(t2, t1);
                    t3 = vsubq_f32(t3, t2);
                    vst1q_f32(&A[i][j], t3);
                }

                for (; j < N; j++) {
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }

                A[i][k] = 0.0;
            }
        }

        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < N; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                t1 = vld1q_f32(temp);
                j = k + 1;

                for (; j + 4 <= N; j += 4) {
                    t2 = vld1q_f32(&A[k][j]);
                    t3 = vld1q_f32(&A[i][j]);
                    t2 = vmulq_f32(t2, t1);
                    t3 = vsubq_f32(t3, t2);
                    vst1q_f32(&A[i][j], t3);
                }
                    for (; j < N; j++) {
                        A[i][j] = A[i][j] - A[i][k] * A[k][j];
                    }

                A[i][k] = 0.0;
            }
        }
    }

    if (myid == 0) {
        double te = MPI_Wtime();
        cout << "MPI_NEON Time: " << te - ts << "s" << endl;
    }

}
int main(int argc, char* argv[]) {
    int myid = 0;
    MPI_Init(&argc, &argv);//MPI���б�Ҫ�ĳ�ʼ������
    MPI_Comm_size(MPI_COMM_WORLD, &n_threads);//���ý�����Ϊn_threads
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);//ʶ����ý��̵�rank��ֵ��0��size-1

    //��ʼ������A
    init();

    //��ʱ��ʼ
    double ts = MPI_Wtime();
    int tmp = (N - N % n_threads) / n_threads;
    int r1 = myid * tmp;
    int r2 = myid * tmp + tmp - 1;
    __m128 t1, t2, t3;
    int j;
    float temp[4];
    //������Ϊ����Ԫ�أ����г����б任
    for (int k = 0; k < N; k++) {
        if (k >= r1 && k <= r2) {
            for (int j = k + 1; j < N; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
            for (int j = 0; j < n_threads; j++) {
                if (j == myid) continue;
                MPI_Send(&A[k][0], N, MPI_FLOAT, j, 100 - myid, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&A[k][0], N, MPI_FLOAT, k / tmp, 100 - k / tmp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        /*for(int i = r1;i <= r2; i++){
            for(int j = k + 1;j < N; j++){
                A[i][j] = A[i][j] - A[k][j] * A[i][k];
            }
            A[i][k] = 0;
        }*/
        if (r2 >= k + 1 && r1 < k + 1) {
            for (int i = k + 1; i <= r2; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                t1 = _mm_loadu_ps(temp);
                j = k + 1;
                for (; j + 4 <= N; j += 4)
                {
                    t2 = _mm_loadu_ps(&A[k][j]);
                    t3 = _mm_loadu_ps(&A[i][j]);
                    t2 = _mm_mul_ps(t2, t1);
                    t3 = _mm_sub_ps(t3, t2);
                    _mm_storeu_ps(&A[i][j], t3);
                }
                for (; j < N; j++)
                {
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
                A[i][k] = 0.0;
            }
        }
        if (r1 >= k + 1) {
            for (int i = r1; i <= r2 && i < N; i++) {
                temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
                t1 = _mm_loadu_ps(temp);
                j = k + 1;
                for (; j + 4 <= N; j += 4)
                {
                    t2 = _mm_loadu_ps(&A[k][j]);
                    t3 = _mm_loadu_ps(&A[i][j]);
                    t2 = _mm_mul_ps(t2, t1);
                    t3 = _mm_sub_ps(t3, t2);
                    _mm_storeu_ps(&A[i][j], t3);
                }
                for (; j < N; j++)
                {
                    A[i][j] = A[i][j] - A[i][k] * A[k][j];
                }
                A[i][k] = 0.0;
            }
        }

    }
    //cout<<myid<<endl;
    //�����������ܵ�0�Ž���
    if (myid != 0) {
        //�����̷����Լ�����������Щ��
       // for (int i = myid; i < N; i += n_threads) {
         //   MPI_Send(A[i], N, MPI_INT, 0, i, MPI_COMM_WORLD);
        //}
        //Print();
    }
    if (myid == 0) {
        // 0�Ž������ν���
      //  for (int i = 0; i < N; i++) {
         //   if (i % n_threads != 0) {
          ////      MPI_Recv(A[i], N, MPI_INT, i % n_threads, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          //  }
       // }

        //��ʱ����
        double te = MPI_Wtime();
        cout << "MPI_SSE"  << " Time:" << te - ts << "s";
        //Print();
    }
    MPI_Finalize();
    return 0;
}