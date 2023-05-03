// Pthread_Pre.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//三重循环全部纳入循环之中
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>
#include <semaphore.h>
#include <sys/time.h> //gettimeofday
#include<pmmintrin.h>

#include<time.h>

#include<xmmintrin.h>
#include<immintrin.h>
#include<iostream>
#include <bits/stdc++.h>

const int maxN = 1000;
int n;
float A[maxN][maxN];
using namespace std;
int thread_count;

void A_reset()
{
	for (int i = 0; i < n; i++)
	{
		A[i][i] = 1.0;
		for (int j = i + 1; j < n; j++)
			A[i][j] = rand();

	}
	for (int k = 0; k < n; k++)
		for (int i = k + 1; i < n; i++)
			for (int j = 0; j < n; j++)
				A[i][j] += A[k][j];
}

const int NUM_THREADS = 3;
typedef struct {
	//int k; //消去的轮次
	int t_id; // 线程 id
}threadParam_t;
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS - 1];
sem_t sem_Elimination[NUM_THREADS - 1];
void* threadFunc(void* param)
{
	threadParam_t* p = (threadParam_t*)param;
	//int k = p->k;
	int t_id = p->t_id;
	//int i = k + t_id + 1;
	for (int k = 0; k < n; k++)
	{
		if (t_id == 0)
		{
			for (int j = k + 1; j < n; j++)
				A[k][j] /= A[k][k];
			A[k][k] = 1.0;
		}
		else
		{
			sem_wait(&sem_Division[t_id - 1]); // 阻塞，等待完成除法操作（操作自己专属的信号量）
		}
		//t_id为0的线程唤醒其他线程
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; i++)
				sem_post(&sem_Division[i]);
		}
		//循环划分任务(尝试多种任务划分方式)
		for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
		{
			for (int j = k + 1; j < n; ++j)
			{
				A[i][j] -= A[i][k] * A[k][j];
			}
			A[i][k] = 0.0;
		}
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_wait(&sem_leader);
			}
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_post(&sem_Elimination[i]);
			}
		}
		else
		{
			sem_post(&sem_leader);
			sem_wait(&sem_Elimination[t_id - 1]);
		}

	}
	pthread_exit(NULL);
	return NULL;
}
void* threadFunc_SSE(void* param)
{
	threadParam_t* p = (threadParam_t*)param;
	//int k = p->k;
	int t_id = p->t_id;
	//int i = k + t_id + 1;
	__m128 t1, t2, t3, t4;
	for (int k = 0; k < n; k++)
	{
		if (t_id == 0)
		{
			float tmp[4] = { A[k][k], A[k][k], A[k][k], A[k][k] };
			t1 = _mm_load_ps(tmp);
			for (int j = n - 4; j >= k; j -= 4) //从后向前每次取四个
			{
				t2 = _mm_load_ps(A[k] + j);
				t3 = _mm_div_ps(t2, t1);//除法
				_mm_store_ps(A[k] + j, t3);
			}
			if (k % 4 != (n % 4)) //处理不能被4整除的元素
			{
				for (int j = k; j % 4 != (n % 4); j++)
				{
					A[k][j] = A[k][j] / tmp[0];
				}
			}
			for (int j = (n % 4) - 1; j >= 0; j--)
			{
				A[k][j] = A[k][j] / tmp[0];
			}
		}
		else
		{
			sem_wait(&sem_Division[t_id - 1]); // 阻塞，等待完成除法操作（操作自己专属的信号量）
		}
		//t_id为0的线程唤醒其他线程
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; i++)
				sem_post(&sem_Division[i]);
		}
		//循环划分任务(尝试多种任务划分方式)
		for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
		{
			float tmp[4] = { A[i][k], A[i][k], A[i][k], A[i][k] };
			t1 = _mm_load_ps(tmp);
			for (int j = n - 4; j > k; j -= 4)
			{
				t2 = _mm_load_ps(A[i] + j);
				t3 = _mm_load_ps(A[k] + j);
				t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //减法
				_mm_store_ps(A[i] + j, t4);
			}
			for (int j = k + 1; j % 4 != (n % 4); j++)
			{
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			}
			A[i][k] = 0;
		}
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_wait(&sem_leader);
			}
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_post(&sem_Elimination[i]);
			}
		}
		else
		{
			sem_post(&sem_leader);
			sem_wait(&sem_Elimination[t_id - 1]);
		}

	}
	pthread_exit(NULL);
	return NULL;
}
void* threadFunc_AVX(void* param)
{
	threadParam_t* p = (threadParam_t*)param;
	//int k = p->k;
	int t_id = p->t_id;
	//int i = k + t_id + 1;
	__m256 t1, t2, t3, t4;
	for (int k = 0; k < n; k++)
	{
		if (t_id == 0)
		{
			float tmp[8] = { A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k] };
			t1 = _mm256_load_ps(tmp);

			for (int j = n - 8; j >= k; j -= 8)
			{
				t2 = _mm256_load_ps(A[k] + j);
				t3 = _mm256_div_ps(t2, t1);
				_mm256_store_ps(A[k] + j, t3);
			}

			if (k % 8 != (n % 8))
			{
				for (int j = k; j % 8 != (n % 8); j++)
				{
					A[k][j] = A[k][j] / tmp[0];
				}
			}

			for (int j = (n % 8) - 1; j >= 0; j--)
			{
				A[k][j] = A[k][j] / tmp[0];
			}
		}
		else
		{
			sem_wait(&sem_Division[t_id - 1]); // 阻塞，等待完成除法操作（操作自己专属的信号量）
		}
		//t_id为0的线程唤醒其他线程
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; i++)
				sem_post(&sem_Division[i]);
		}
		//循环划分任务(尝试多种任务划分方式)
		for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
		{
			float tmp[8] = { A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k] };
			t1 = _mm256_load_ps(tmp);

			for (int j = n - 8; j > k; j -= 8)
			{
				t2 = _mm256_load_ps(A[i] + j);
				t3 = _mm256_load_ps(A[k] + j);
				t4 = _mm256_sub_ps(t2, _mm256_mul_ps(t1, t3));
				_mm256_store_ps(A[i] + j, t4);
			}

			for (int j = k + 1; j % 8 != (n % 8); j++)
			{
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			}

			A[i][k] = 0.0;
		}
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_wait(&sem_leader);
			}
			for (int i = 0; i < NUM_THREADS - 1; ++i)
			{
				sem_post(&sem_Elimination[i]);
			}
		}
		else
		{
			sem_post(&sem_leader);
			sem_wait(&sem_Elimination[t_id - 1]);
		}

	}
	pthread_exit(NULL);
	return NULL;
}

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
void SSE_Gauss() //加了SSE并行的高斯消去法
{
	__m128 t1, t2, t3, t4;
	for (int k = 0; k < n; k++)
	{
		float tmp[4] = { A[k][k], A[k][k], A[k][k], A[k][k] };
		t1 = _mm_load_ps(tmp);
		for (int j = n - 4; j >= k; j -= 4) //从后向前每次取四个
		{
			t2 = _mm_load_ps(A[k] + j);
			t3 = _mm_div_ps(t2, t1);//除法
			_mm_store_ps(A[k] + j, t3);
		}
		if (k % 4 != (n % 4)) //处理不能被4整除的元素
		{
			for (int j = k; j % 4 != (n % 4); j++)
			{
				A[k][j] = A[k][j] / tmp[0];
			}
		}
		for (int j = (n % 4) - 1; j >= 0; j--)
		{
			A[k][j] = A[k][j] / tmp[0];
		}
		for (int i = k + 1; i < n; i++)
		{
			float tmp[4] = { A[i][k], A[i][k], A[i][k], A[i][k] };
			t1 = _mm_load_ps(tmp);
			for (int j = n - 4; j > k; j -= 4)
			{
				t2 = _mm_load_ps(A[i] + j);
				t3 = _mm_load_ps(A[k] + j);
				t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //减法
				_mm_store_ps(A[i] + j, t4);
			}
			for (int j = k + 1; j % 4 != (n % 4); j++)
			{
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			}
			A[i][k] = 0;
		}
	}
}

void AVX_Gauss()
{
	__m256 t1, t2, t3, t4;

	for (int k = 0; k < n; k++)
	{
		float tmp[8] = { A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k], A[k][k] };
		t1 = _mm256_load_ps(tmp);

		for (int j = n - 8; j >= k; j -= 8)
		{
			t2 = _mm256_load_ps(A[k] + j);
			t3 = _mm256_div_ps(t2, t1);
			_mm256_store_ps(A[k] + j, t3);
		}

		if (k % 8 != (n % 8))
		{
			for (int j = k; j % 8 != (n % 8); j++)
			{
				A[k][j] = A[k][j] / tmp[0];
			}
		}

		for (int j = (n % 8) - 1; j >= 0; j--)
		{
			A[k][j] = A[k][j] / tmp[0];
		}

		for (int i = k + 1; i < n; i++)
		{
			float tmp[8] = { A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k], A[i][k] };
			t1 = _mm256_load_ps(tmp);

			for (int j = n - 8; j > k; j -= 8)
			{
				t2 = _mm256_load_ps(A[i] + j);
				t3 = _mm256_load_ps(A[k] + j);
				t4 = _mm256_sub_ps(t2, _mm256_mul_ps(t1, t3));
				_mm256_store_ps(A[i] + j, t4);
			}

			for (int j = k + 1; j % 8 != (n % 8); j++)
			{
				A[i][j] = A[i][j] - A[i][k] * A[k][j];
			}

			A[i][k] = 0;
		}
	}
}

int main(int argc, char* argv[]) {

	cin >> n;
	
	//性能测试
	cout << endl << "规模n=" << n << endl;
	long long head1, tail1, freq, head2, tail2;
	double tm1 = 0, tm2 = 0;
	cout << setprecision(4);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	A_reset();
	QueryPerformanceCounter((LARGE_INTEGER*)&head1);
	normal_Gauss();
	QueryPerformanceCounter((LARGE_INTEGER*)&tail1);
	//cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
	tm1 = double((tail1 - head1) * 1000.000 / freq);
	cout << tm1 << endl;
	std::cout << "normal_Gauss time: " << std::fixed << std::setprecision(2) << tm1 << "ms\n";

	A_reset();
	QueryPerformanceCounter((LARGE_INTEGER*)&head1);
	SSE_Gauss();
	QueryPerformanceCounter((LARGE_INTEGER*)&tail1);
	//cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
	tm1 = double((tail1 - head1) * 1000.000 / freq);
	cout << tm1 << endl;
	std::cout << "SSE_Gauss time: " << std::fixed << std::setprecision(2) << tm1 << "ms\n";

	A_reset();
	QueryPerformanceCounter((LARGE_INTEGER*)&head1);
	AVX_Gauss();
	QueryPerformanceCounter((LARGE_INTEGER*)&tail1);
	//cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
	tm1 = double((tail1 - head1) * 1000.000 / freq);
	cout << tm1 << endl;
	std::cout << "AVX_Gauss time: " << std::fixed << std::setprecision(2) << tm1 << "ms\n";
	




	A_reset();
	cout << endl << "规模n=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//初始化信号量
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREADS - 1; ++i)
	{
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	pthread_t handles[NUM_THREADS];// 创建对应的 Handle
	threadParam_t param[NUM_THREADS];// 创建对应的线程数据结构


	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id], NULL, threadFunc, (void*)(param + t_id));
	}

	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		pthread_join(handles[t_id], NULL);
	}
	sem_destroy(sem_Elimination);
	sem_destroy(sem_Division);
	sem_destroy(&sem_leader);

	QueryPerformanceCounter((LARGE_INTEGER*)&tail2);
	tm2 = double((tail2 - head2) * 1000.000 / freq);
	cout << tm2 << endl;
	std::cout << "static+siganl_plus time: " << std::fixed << std::setprecision(2) << tm2 << "ms\n";

	A_reset();
	cout << endl << "规模n=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//初始化信号量
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREADS - 1; ++i)
	{
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id], NULL, threadFunc_SSE, (void*)(param + t_id));
	}

	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		pthread_join(handles[t_id], NULL);
	}
	sem_destroy(sem_Elimination);
	sem_destroy(sem_Division);
	sem_destroy(&sem_leader);

	QueryPerformanceCounter((LARGE_INTEGER*)&tail2);
	tm2 = double((tail2 - head2) * 1000.000 / freq);
	cout << tm2 << endl;
	std::cout << "static+siganl_plus_SSE time: " << std::fixed << std::setprecision(2) << tm2 << "ms\n";

	A_reset();
	cout << endl << "规模n=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//初始化信号量
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREADS - 1; ++i)
	{
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	


	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id], NULL, threadFunc_AVX, (void*)(param + t_id));
	}

	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		pthread_join(handles[t_id], NULL);
	}
	sem_destroy(sem_Elimination);
	sem_destroy(sem_Division);
	sem_destroy(&sem_leader);

	QueryPerformanceCounter((LARGE_INTEGER*)&tail2);
	tm2 = double((tail2 - head2) * 1000.000 / freq);
	cout << tm2 << endl;
	std::cout << "static+siganl_plus_AVX time: " << std::fixed << std::setprecision(2) << tm2 << "ms\n";









} /* main */





// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
