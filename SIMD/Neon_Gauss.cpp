// Neon_Gauss.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <bits/stdc++.h>
#include <arm_neon.h>
#include <time.h>
#include <fstream>

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
//void Neon_Gauss()
//{
//    float32x4_t vt, va,vaij,vaik,vakj,vx;
//    for (int k = 0; k < n; k++)
//    {
//        vt = vdupq_n_f32(A[k][k]);
//        int j = k + 1;
//        for(j=k+1; j + 4 <= n; j += 4)
//        {
//            va = vld1q_f32(&A[k][j]);
//            va=vdivq_f32(va, vt);
//            vst1q_f32(&A[k][j], va);
//        }
//        for (; j <= n; j++)
//        {
//            A[k][j] = A[k][j] / A[k][k];
//        }
//        A[k][k] = 1.0;
//        for (int i = k + 1; i < n; i++)
//        {
//            vaik = vdupq_n_f32(A[i][k]);
//            int j = k + 1;
//            for (j = k + 1; j + 4 <= n; j += 4)
//            {
//                vakj = vld1q_f32(&A[k][j]);
//                vaij = vld1q_f32(&A[i][j]);
//                vx = vmulq_f32(vakj, vaik);
//                vaij= vsubq_f32(vaij, vx);
//                vst1q_f32(&A[i][j], vaij);
//            }
//            for (; j <= n; j++)
//            {
//                A[i][j] =A[i][j]- A[k][j] * A[i][k];
//            }
//            A[i][k] = 0;
//
//        }
//    }
//}
void Neon_Gauss_part1()
{
    float32x4_t vt, va, vaij, vaik, vakj, vx;
    for (int k = 0; k < n; k++)
    {
        vt = vdupq_n_f32(A[k][k]);
        int j = k + 1;
        for (j = k + 1; j + 4 <= n; j += 4)
        {
            va = vld1q_f32(&A[k][j]);
            va = vdivq_f32(va, vt);
            vst1q_f32(&A[k][j], va);
        }
        for (; j <= n; j++)
        {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
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
}
void Neon_Gauss_part2() 
{
    float32x4_t vt, va, vaij, vaik, vakj, vx;
    for (int k = 0; k < n; k++)
    {
        float tmp = A[k][k];
        for (int j = k; j < n; j++)
        {
            A[k][j] = A[k][j] / tmp;
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
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&A[i][j], vaij);
            }
            for (; j <= n; j++)
            {
                A[i][j] = A[i][j] - A[k][j] * A[i][k];
            }
            A[i][k] = 0;

        }
    }
    return;
}
void Neon_Gauss()
{
    float32x4_t vt, va, vaij, vaik, vakj, vx;
    for (int k = 0; k < n; k++)
    {
        vt = vdupq_n_f32(A[k][k]);
        int j = k + 1;
        for (j = k + 1; j + 4 <= n; j += 4)
        {
            va = vld1q_f32(&A[k][j]);
            va = vdivq_f32(va, vt);
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
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&A[i][j], vaij);
            }
            for (; j <= n; j++)
            {
                A[i][j] = A[i][j] - A[k][j] * A[i][k];
            }
            A[i][k] = 0;

        }
    }
}
void Neon_Gauss_aligned()
{
    float32_t* ptrA = &A[0][0];
    for (int k = 0; k < n; k++)
    {
        float32x4_t vt = vdupq_n_f32(ptrA[k * n + k]);
        int j = k + 1;
        for (j = k + 1; j + 3 < n; j += 4)
        {
            float32x4_t va = vld1q_f32(&ptrA[k * n + j]);
            va = vdivq_f32(va, vt);
            vst1q_f32(&ptrA[k * n + j], va);
        }
        for (; j < n; j++)
        {
            ptrA[k * n + j] = ptrA[k * n + j] / ptrA[k * n + k];
        }
        ptrA[k * n + k] = 1.0;
        for (int i = k + 1; i < n; i++)
        {
            float32x4_t vaik = vdupq_n_f32(ptrA[i * n + k]);
            j = k + 1;
            for (j = k + 1; j + 3 < n; j += 4)
            {
                float32x4_t vakj = vld1q_f32(&ptrA[k * n + j]);
                float32x4_t vaij = vld1q_f32(&ptrA[i * n + j]);
                float32x4_t vx = vmulq_f32(vakj, vaik);
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&ptrA[i * n + j], vaij);
            }
            for (; j < n; j++)
            {
                ptrA[i * n + j] = ptrA[i * n + j] - ptrA[k * n + j] * ptrA[i * n + k];
            }
            ptrA[i * n + k] = 0;
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

    clock_t start1, end1;
    start1 = clock();

    normal_Gauss();

    end1 = clock();
    printf("totile time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);
   
    cout << endl << endl << endl << "使用Neon_Gauss_part1的高斯消去法" << endl;

    clock_t start2, end2;
    start2 = clock();

    Neon_Gauss_part1();

    end2 = clock();
    printf("totile time=%f\n", (float)(end2 - start2) * 1000 / CLOCKS_PER_SEC);

    cout << endl << endl << endl << "使用Neon_Gauss_part2的高斯消去法" << endl;

    clock_t start3, end3;
    start3 = clock();

    Neon_Gauss_part2();

    end3 = clock();
    printf("totile time=%f\n", (float)(end3 - start3) * 1000 / CLOCKS_PER_SEC);
    
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
