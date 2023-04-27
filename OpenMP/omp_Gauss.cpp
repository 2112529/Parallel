// OpenMP_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
//尝试进行平方数加和
void normal_Gauss() //没加SSE串行的高斯消去法
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
    // 在外循环之外创建线程，避免线程反复创建销毁，注意共享变量和私有变量的设置
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
        //并行部分，使用行划分
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

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
