// OpenMP_test.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <sys/time.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
const int maxN = 10000;
int n;
float A[maxN][maxN];
using namespace std;
int thread_count;
//���Խ���ƽ�����Ӻ�
void normal_Gauss() //û��SSE���еĸ�˹��ȥ��
{
    for (int k = 0; k < n; k++)
    {
        float tmp = A[k][k];
        for (int j = k; j < n; j++)
        {
            A[k][j] = A[k][j] / tmp;
        }

        for (int i = k + 1; i < n; i++)
        {
            float tmp2 = A[i][k];
            for (int j = k + 1; j < n; j++)
            {
                A[i][j] = A[i][j] - tmp2 * A[k][j];
            }
            A[i][k] = 0;
        }
    }
    return;
}
const int NUM_THREADS = 3;
int main()
{
    long long head, tail, freq;
    double time;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    int i, j, k, temp;
    // ����ѭ��֮�ⴴ���̣߳������̷߳����������٣�ע�⹲�������˽�б���������
    #pragma omp parallel if (parallel), num_threads(NUM_THREADS), private(i, j, k, temp)
    for (k = 1; k < n; k++)
    {
        #pragma omp single
        {
            temp = A[k][k];
            for (j = k + 1; j < n; j++)
            {
                A[k][j] /= temp;
            }
            A[k][k] = 1.0;
        }
        //���в��֣�ʹ���л���
        #pragma omp for
        {
            for (i = k + 1; i < n; i++)
            {
                temp = A[i][k];
                for (j = k + 1; j < n; j++)
                    A[i][j] -= temp * A[k][j];
                A[i][k] /= 0.0;
            }
            A[k][k] = 1.0;
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    time = double((tail - head) * 1000.000 / freq);
    std::cout << "OMP_Gauss time: " << std::fixed << std::setprecision(2) << time << "ms\n";

    long long head1, tail1, freq;
    double time1;
    //QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    normal_Gauss();
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    time1 = double((tail1 - head1) * 1000.000 / freq);
    std::cout << "normal_Gauss time: " << std::fixed << std::setprecision(2) << time1 << "ms\n";
    

    return 0;
}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
