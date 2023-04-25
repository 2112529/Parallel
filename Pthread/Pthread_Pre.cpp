// Pthread_Pre.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>

const int maxN = 10000;
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
typedef struct {
	int k; //消去的轮次
	int t_id; // 线程 id
}threadParam_t;

void* threadFunc(void *param) 
{
	threadParam_t* p = (threadParam_t*)param;
	int k = p->k;
	int t_id = p->t_id;
	int i = k + t_id + 1;
	for (int j = k + 1; j < n; ++j)
	{
		A[i][j] -= A[i][k] * A[k][j];
	}
	A[i][k] = 0;
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
/* Global variable: accessible to all threads */


void* Hello(void* rank); /* Thread function */

int main(int argc, char* argv[]) {

	cin >> n;

	A_reset();


	cout << endl << "规模n=" << n << endl;
	clock_t start, end;
	start = clock();
	normal_Gauss();
	end = clock();
	printf("Normal Gauss time=%f\n", (float)(end - start) * 1000 / CLOCKS_PER_SEC);



	
	A_reset();
	cout << endl << "规模n=" << n << endl;
	clock_t start1, end1;
	start1 = clock();
	for (int k = 0; k < n; ++k)
	{
		for (int j = k + 1; j < n; j++)
		{
			A[k][j] /= A[k][k];
		}
		A[k][k] = 1.0;
		//创建线程准备进行计算
		int worker_count = n - 1 - k;
		pthread_t* thread_handles = static_cast<pthread_t*>(malloc( worker_count * sizeof(pthread_t)));
		threadParam_t* param = static_cast<threadParam_t*>(malloc(worker_count * sizeof(threadParam_t)));
		//分配任务
		for (int t_id = 0; t_id < worker_count; t_id++)
		{
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		//创建线程
		for (int t_id = 0; t_id < worker_count; t_id++)
		{
			pthread_create(&thread_handles[t_id], NULL, threadFunc, (void*)param);
		}
		for (int t_id = 0; t_id < worker_count; t_id++)
		{
			pthread_join(thread_handles[t_id], NULL);
		}
	}
	end1 = clock();
	printf("OpenMP time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);

	
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
