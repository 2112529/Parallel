// OpenMP_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <bits/stdc++.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
//尝试进行平方数加和


int main()
{
    int sum=0;
    int n;
    cin >> n;
    clock_t start, end;
    start = clock();
    #pragma omp parallel for num_threads(4)\
        reduction(+:sum)
    for (int i = 1; i < n; i++)
    {
        sum += i * i;
    }
    cout << sum << endl;
    end = clock();
    printf("OMP time=%f\n", (float)(end - start) * 1000 / CLOCKS_PER_SEC);

    clock_t start1, end1;
    sum = 0;
    start1 = clock();
    for (int i = 1; i < n; i++)
    {
        sum += i * i;
    }
    cout << sum << endl;
    end1 = clock();
    printf("Normal time=%f\n", (float)(end1 - start1) * 1000 / CLOCKS_PER_SEC);
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
