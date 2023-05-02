// Pthread_Pre.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>
#include <semaphore.h>
#include <sys/time.h> //gettimeofday



const int maxN = 10000;
int n;
float A[maxN][maxN];
using namespace std;
int thread_count;
typedef long long LARGER_INTERGER;
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

const int NUM_THREADS = 9;
typedef struct {
	//int k; //��ȥ���ִ�
	int t_id; // �߳� id
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
		sem_wait(&sem_workerstart[t_id]); // �������ȴ�������ɳ��������������Լ�ר�����ź�����
		//ѭ����������
		for (int i = k + 1 + t_id; i < n; i += NUM_THREADS)
		{
			for (int j = k + 1; j < n; ++j)
			{
				A[i][j] -= A[i][k] * A[k][j];
			}
			A[i][k] = 0.0;
		}
		sem_post(&sem_main);
		//cout << t_id << endl;
		sem_wait(&sem_workerend[t_id]);
	}

	//cout << t_id << "is Over��" << endl;
	pthread_exit(NULL);
	return NULL;
}

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
/* Global variable: accessible to all threads */


//void* Hello(void* rank); /* Thread function */

int main(int argc, char* argv[]) {
	for (int i = 0; i < 10; i++)
	{
		cin >> n;
		A_reset();


		cout << endl << "Scale n=" << n << endl;


		timeval head1, tail1, freq, head2, tail2;
		double tm1 = 0, tm2 = 0;
		cout << setprecision(4);
		//QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		gettimeofday(&head1, NULL);

		//��ʼ���ź���
		sem_init(&sem_main, 0, 0);
		for (int i = 0; i < NUM_THREADS; ++i)
		{
			sem_init(&sem_workerstart[i], 0, 0);
			sem_init(&sem_workerend[i], 0, 0);
		}
		pthread_t handles[NUM_THREADS];// ������Ӧ�� Handle
		threadParam_t param[NUM_THREADS];// ������Ӧ���߳����ݽṹ
		for (int t_id = 0; t_id < NUM_THREADS; t_id++)
		{
			param[t_id].t_id = t_id;
			pthread_create(&handles[t_id], NULL, threadFunc, (void*)(param + t_id));
		}

		for (int k = 0; k < n; k++)
		{
			//���̳߳���
			for (int j = k + 1; j < n; j++)
				A[k][j] /= A[k][k];
			A[k][k] = 1.0;
			//��ʼ���ѹ����߳�
			for (int t_id = 0; t_id < NUM_THREADS; t_id++)
			{
				sem_post(&sem_workerstart[t_id]);
			}
			for (int t_id = 0; t_id < NUM_THREADS; t_id++)
			{
				sem_wait(&sem_main);
				//cout << t_id << endl;
			}
			for (int t_id = 0; t_id < NUM_THREADS; t_id++)
			{
				sem_post(&sem_workerend[t_id]);
			}
		}
		for (int t_id = 0; t_id < NUM_THREADS; t_id++)
		{
			pthread_join(handles[t_id], NULL);
		}
		sem_destroy(sem_workerstart);
		sem_destroy(sem_workerend);
		sem_destroy(&sem_main);

		//QueryPerformanceCounter((LARGE_INTEGER*)&tail1);
		//cout << "static+signal_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
		gettimeofday(&tail1, NULL);
		tm1 += (tail1.tv_sec - head1.tv_sec) * 1000.0
			+ (tail1.tv_usec - head1.tv_usec) / 1000.0;
		cout << tm1 << endl;
		std::cout << "static+siganl_Gauss time: " << std::fixed << std::setprecision(2) << tm1 << "ms\n";



		A_reset();


		cout << setprecision(4);

		gettimeofday(&head2, NULL);
		normal_Gauss();
		gettimeofday(&tail2, NULL);
		tm2 += (tail2.tv_sec - head2.tv_sec) * 1000.0
			+ (tail2.tv_usec - head2.tv_usec) / 1000.0;
		cout << tm2 << endl;
		std::cout << "Normal_Gauss time: " << std::fixed << std::setprecision(2) << tm2 << "ms\n";

	}


	

	










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
