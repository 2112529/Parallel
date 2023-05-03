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
	//int k; //��ȥ���ִ�
	int t_id; // �߳� id
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
			sem_wait(&sem_Division[t_id - 1]); // �������ȴ���ɳ��������������Լ�ר�����ź�����
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
			for (int j = n - 4; j >= k; j -= 4) //�Ӻ���ǰÿ��ȡ�ĸ�
			{
				t2 = _mm_load_ps(A[k] + j);
				t3 = _mm_div_ps(t2, t1);//����
				_mm_store_ps(A[k] + j, t3);
			}
			if (k % 4 != (n % 4)) //�����ܱ�4������Ԫ��
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
			sem_wait(&sem_Division[t_id - 1]); // �������ȴ���ɳ��������������Լ�ר�����ź�����
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
			float tmp[4] = { A[i][k], A[i][k], A[i][k], A[i][k] };
			t1 = _mm_load_ps(tmp);
			for (int j = n - 4; j > k; j -= 4)
			{
				t2 = _mm_load_ps(A[i] + j);
				t3 = _mm_load_ps(A[k] + j);
				t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //����
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
			sem_wait(&sem_Division[t_id - 1]); // �������ȴ���ɳ��������������Լ�ר�����ź�����
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
void SSE_Gauss() //����SSE���еĸ�˹��ȥ��
{
	__m128 t1, t2, t3, t4;
	for (int k = 0; k < n; k++)
	{
		float tmp[4] = { A[k][k], A[k][k], A[k][k], A[k][k] };
		t1 = _mm_load_ps(tmp);
		for (int j = n - 4; j >= k; j -= 4) //�Ӻ���ǰÿ��ȡ�ĸ�
		{
			t2 = _mm_load_ps(A[k] + j);
			t3 = _mm_div_ps(t2, t1);//����
			_mm_store_ps(A[k] + j, t3);
		}
		if (k % 4 != (n % 4)) //�����ܱ�4������Ԫ��
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
				t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //����
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
	
	//���ܲ���
	cout << endl << "��ģn=" << n << endl;
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
	cout << endl << "��ģn=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//��ʼ���ź���
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < NUM_THREADS - 1; ++i)
	{
		sem_init(&sem_Division[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
	pthread_t handles[NUM_THREADS];// ������Ӧ�� Handle
	threadParam_t param[NUM_THREADS];// ������Ӧ���߳����ݽṹ


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
	cout << endl << "��ģn=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//��ʼ���ź���
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
	cout << endl << "��ģn=" << n << endl;
	QueryPerformanceCounter((LARGE_INTEGER*)&head2);

	//��ʼ���ź���
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





// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
