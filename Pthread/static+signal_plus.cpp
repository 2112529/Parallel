// Pthread_Pre.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//
//����ѭ��ȫ������ѭ��֮��
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>
#include <semaphore.h>

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
	//int k; //��ȥ���ִ�
	int t_id; // �߳� id
}threadParam_t;
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS-1];
sem_t sem_Elimination[NUM_THREADS-1];
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
			sem_wait(&sem_Division[t_id-1]); // �������ȴ���ɳ��������������Լ�ר�����ź�����
		}
		//t_idΪ0���̻߳��������߳�
		if (t_id == 0)
		{
			for (int i = 0; i < NUM_THREADS - 1; i++)
				sem_post(&sem_Division[i]);
		}
		//ѭ����������(���Զ������񻮷ַ�ʽ)
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

	cin >> n;




	A_reset();
	//���ܲ���
	cout << endl << "��ģn=" << n << endl;
	clock_t start, end;
	start = clock();
	normal_Gauss();
	end = clock();
	cout << "Normal_Gauss time:" << (float)(end - start) * 1000 / CLOCKS_PER_SEC;
	//printf("static+signal_Gauss time=%f\n", (float)(end - start) * 1000 / CLOCKS_PER_SEC);




	A_reset();

	cout << endl << "��ģn=" << n << endl;
	clock_t start1, end1;
	start1 = clock();

	//��ʼ���ź���
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREADS-1; ++i)
	{
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	pthread_t handles[NUM_THREADS];// ������Ӧ�� Handle
	threadParam_t param[NUM_THREADS];// ������Ӧ���߳����ݽṹ


	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id], NULL, threadFunc, (void*)(param+t_id));
	}

	for (int t_id = 0; t_id < NUM_THREADS; t_id++)
	{
		pthread_join(handles[t_id], NULL);
	}
	sem_destroy(sem_Elimination);
	sem_destroy(sem_Division);
	sem_destroy(&sem_leader);

	end1 = clock();
	cout << "static+signal_plus_Gauss time:" << (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC;
	//printf("Normal Gauss time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);
	//printf("Normal Gauss time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);










} /* main */





// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
