#include<pmmintrin.h>

#include<time.h>

#include<xmmintrin.h>
#include<immintrin.h>
#include<iostream>
#include <bits/stdc++.h>
//#define N 5000
int n;


using namespace std;



float** normal_gaosi(float** matrix) //没加SSE串行的高斯消去法

{

    for (int k = 0; k < n; k++)

    {

        float tmp = matrix[k][k];

        for (int j = k; j < n; j++)

        {

            matrix[k][j] = matrix[k][j] / tmp;

        }

        for (int i = k + 1; i < n; i++)

        {

            float tmp2 = matrix[i][k];

            for (int j = k + 1; j < n; j++)

            {

                matrix[i][j] = matrix[i][j] - tmp2 * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }

    return matrix;

}



void SSE_gaosi(float** matrix) //加了SSE并行的高斯消去法
{
    __m128 t1, t2, t3, t4;
    for (int k = 0; k < n; k++)
    {
        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm_load_ps(tmp);
        for (int j = n - 4; j >= k; j -= 4) //从后向前每次取四个
        {
            t2 = _mm_load_ps(matrix[k] + j);
            t3 = _mm_div_ps(t2, t1);//除法
            _mm_store_ps(matrix[k] + j, t3);
        }
        if (k % 4 != (n % 4)) //处理不能被4整除的元素
        {
            for (int j = k; j % 4 != (n % 4); j++)
            {
                matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }
        for (int j = (n % 4) - 1; j >= 0; j--)
        {
            matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (int i = k + 1; i < n; i++)
        {
            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm_load_ps(tmp);
            for (int j = n - 4; j > k; j -= 4)
            {
                t2 = _mm_load_ps(matrix[i] + j);
                t3 = _mm_load_ps(matrix[k] + j);
                t4 = _mm_sub_ps(t2, _mm_mul_ps(t1, t3)); //减法
                _mm_store_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 4 != (n % 4); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}

void AVX_gaosi(float** matrix)
{
    __m256 t1, t2, t3, t4;

    for (int k = 0; k < n; k++)
    {
        float tmp[8] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm256_load_ps(tmp);

        for (int j = n - 8; j >= k; j -= 8)
        {
            t2 = _mm256_load_ps(matrix[k] + j);
            t3 = _mm256_div_ps(t2, t1);
            _mm256_store_ps(matrix[k] + j, t3);
        }

        if (k % 8 != (n % 8))
        {
            for (int j = k; j % 8 != (n % 8); j++)
            {
                matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }

        for (int j = (n % 8) - 1; j >= 0; j--)
        {
            matrix[k][j] = matrix[k][j] / tmp[0];
        }

        for (int i = k + 1; i < n; i++)
        {
            float tmp[8] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm256_load_ps(tmp);

            for (int j = n - 8; j > k; j -= 8)
            {
                t2 = _mm256_load_ps(matrix[i] + j);
                t3 = _mm256_load_ps(matrix[k] + j);
                t4 = _mm256_sub_ps(t2, _mm256_mul_ps(t1, t3));
                _mm256_store_ps(matrix[i] + j, t4);
            }

            for (int j = k + 1; j % 8 != (n % 8); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }

            matrix[i][k] = 0;
        }
    }
}




void print(float** matrix) //输出

{

    for (int i = 0; i < n; i++)

    {

        for (int j = 0; j < n; j++)

        {

            cout << matrix[i][j] << " ";

        }

        cout << endl;

    }

}



int main()

{
    for (int i = 1; i <= 9; i++)
    {
        cin >> n;



        srand((unsigned)time(NULL));

        float** matrix = new float* [n];

        float** matrix2 = new float* [n];

        float** matrix3 = new float* [n];

        for (int i = 0; i < n; i++)

        {

            matrix[i] = new float[n];

            matrix3[i] = matrix2[i] = matrix[i];

        }



        //cout << "我们生成了初始随机矩阵" << endl;

        for (int i = 0; i < n; i++)

        {

            for (int j = 0; j < n; j++)

            {

                matrix3[i][j] = rand() % 100;
                //cout << matrix[i][j] << " ";
            }
            //cout << endl;

        }

        //print(matrix);



        //cout << endl << endl << endl << "不使用SSE串行的高斯消去法" << endl;

        clock_t  clockBegin, clockEnd;

        //clockBegin = clock(); //开始计时

        //float** B = normal_gaosi(matrix);

        //clockEnd = clock();

        ////print(matrix);

        //cout << "总共耗时： " << clockEnd - clockBegin << "ms" << endl;

        //print(B);



        cout << endl << endl << endl << "使用AVX并行的高斯消去法" << endl;

        clockBegin = clock(); //开始计时

        SSE_gaosi(matrix3);

        clockEnd = clock();

        //print(matrix2);

        cout << "总共耗时： " << clockEnd - clockBegin << "ms" << endl;
    }
    

    system("pause");

    return 0;

}
