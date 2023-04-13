// Neon_Gauss.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <bits/stdc++.h>
#include <arm_neon.h>
#include <time.h>

const int maxN = 10000;
int n;
float A[maxN][maxN];
using namespace std;
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
void A_reset1()
{
    for (int i = 0; i < n; i++)

    {

        for (int j = 0; j < n; j++)

        {

            A[i][j] = rand() % 100;
            
        }
        

    }
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
void Neon_Gauss()
{
    float32x4_t vt, va,vaij,vaik,vakj,vx;
    for (int k = 0; k < n; k++)
    {
        vt = vdupq_n_f32(A[k][k]);
        int j = k + 1;
        for(j=k+1; j + 4 <= n; j += 4)
        {
            va = vld1q_f32(&A[k][j]);
            va=vdivq_f32(va, vt);
            vst1q_f32(&A[k][j], va);
        }
        for (; j <= n; j++)
        {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i = k + 1; i < n; i++)
        {
            vaik = vdupq_n_f32(A[i][k]);
            int j = k + 1;
            for (j = k + 1; j + 4 <= n; j += 4)
            {
                vakj = vld1q_f32(&A[k][j]);
                vaij = vld1q_f32(&A[i][j]);
                vx = vmulq_f32(vakj, vaik);
                vaij= vsubq_f32(vaij, vx);
                vst1q_f32(&A[i][j], vaij);
            }
            for (; j <= n; j++)
            {
                A[i][j] =A[i][j]- A[k][j] * A[i][k];
            }
            A[i][k] = 0;

        }
    }
    

    
}
int main()
{
    cin >> n;

    A_reset1();
    //for (int i = 0; i < n; i++)
    //{
    //    for (int j = 0; j < n; j++)
    //        cin>>A[i][j];
    //    
    //}


    cout << n << endl;

    cout << endl << endl << endl << "使用Normal的高斯消去法" << endl;

    clock_t start, end;
    start = clock();

    normal_Gauss();

    end = clock();
    printf("totile time=%f\n", (float)(end - start) * 1000 / CLOCKS_PER_SEC);
   
    //for (int i = 0; i < n; i++)
    //{
    //    for (int j = 0; j < n; j++)
    //        cout << A[i][j] << " ";
    //    cout << endl;
    //}
    
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
