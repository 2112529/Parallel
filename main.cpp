
#include <iostream>
#include <bits/stdc++.h>
using namespace std;
const int N = 110;
int nums[N][N];
int n;
int s[N];
int sum[N];
void compute_1()
{
    for (int i = 1; i <= n; i++)
    {
        sum[i] = 0;
        for (int j = 1; j <= n; j++)
        {
            sum[i] += nums[j][i] * s[j];
        }
    }

    return;
}
void compute_2()
{
    for (int i = 1; i <= n; i++)
    {
        sum[i] = 0;

    }
    for (int j = 1; j <= n; j++)
    {
        for (int k = 1; k <= n; k++)
        {
            sum[k] += nums[j][k] * s[j];
        }

    }
    return;
}
int main()
{
    cin >> n;
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            cin >> nums[i][j];
        }
    }
    for (int i = 1; i <= n; i++)
    {
        cin >> s[i];

    }
    compute_1();
    //for (int i = 1; i <= n; i++)
    //{
        //cout << sum[i] << " ";
    //}
    //cout << endl;
    //compute_2();
    //for (int i = 1; i <= n; i++)
    //{
        //cout << sum[i] << " ";
    //}



    return 0;
}

