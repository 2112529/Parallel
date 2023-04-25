// Pthread_Pre.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>
#include <semaphore.h>

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

const int NUM_THREADS = 10;
typedef struct {
	//int k; //消去的轮次
	int t_id; // 线程 id
}threadParam_t;
sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];
sem_t sem_workerend[NUM_THREADS];
void* threadFunc(void* param)
{
	threadParam_t* p = (threadParam_t*)param;
	//int k = p->k;
	int t_id = p->t_id;
	//int i = k + t_id + 1;
	for (int k = 0; k < n; k++)
	{
		sem_wait(&sem_workerstart[t_id]); // 阻塞，等待主线完成除法操作（操作自己专属的信号量）
		//循环划分任务
		for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
		{
			for (int j = k + 1; j < n; ++j)
			{
				A[i][j] -= A[i][k] * A[k][j];
			}
			A[i][k] = 0;
		}
		sem_post(&sem_main);
		sem_wait(&sem_workerend[t_id]);
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
/* Global variable: accessible to all threads */


//void* Hello(void* rank); /* Thread function */

int main(int argc, char* argv[]) {

	cin >> n;




	A_reset();
	//性能测试
	cout << endl << "规模n=" << n << endl;
	clock_t start, end;
	start = clock();
	normal_Gauss();
	end = clock();
	cout << "Normal_Gauss time:" << (float)(end - start) * 1000 / CLOCKS_PER_SEC;
	//printf("static+signal_Gauss time=%f\n", (float)(end - start) * 1000 / CLOCKS_PER_SEC);




	A_reset();

	cout << endl << "规模n=" << n << endl;
	clock_t start1, end1;
	start1 = clock();

	//初始化信号量
	sem_init(&sem_main, 0, 0);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		sem_init(&sem_workerstart[i], 0, 0);
		sem_init(&sem_workerend[i], 0, 0);
	}
	pthread_t handles[NUM_THREADS];// 创建对应的 Handle
	threadParam_t param[NUM_THREADS];// 创建对应的线程数据结构
	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id], NULL, threadFunc, (void*)param);
	}
	for (int k = 0; k < n; k++)
	{
		//主线程除法
		for (int j = k + 1; j < n; j++)
			A[k][j] /= A[k][k];
		A[k][k] = 1.0;
		//开始唤醒工作线程
		for (int t_id = 0; t_id < NUM_THREADS; t_id++)
		{
			sem_post(&sem_workerstart[t_id]);
		}
		for (int t_id = 0; t_id < NUM_THREADS; t_id++)
		{
			sem_wait(&sem_main);

		}
		for (int t_id = 0; t_id < NUM_THREADS; t_id++)
		{
			sem_post(&sem_workerend[t_id]);
		}
	}
	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
	sem_destroy(sem_workerstart);
	sem_destroy(sem_workerend);
	sem_destroy(&sem_main);

	end1 = clock();
	cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
	//printf("Normal Gauss time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);
	//printf("Normal Gauss time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);

	




	
	


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
