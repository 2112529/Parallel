#include <iostream>

using namespace std;
const int N=33;
int a[N];
int n;
int sum;
void compute_1()
{

    sum=0;
    for(int i=1;i<=n;i++)
    {
        sum+=a[i];

    }
    return ;
}
void compute_2()
{

    sum=0;
    int sum1=0;
    int sum2=0;
    for(int i=1;i<=n;i+=2)
    {
        sum1+=a[i];
        sum2+=a[i+1];

    }
    sum=sum1+sum2;
    return ;
}
int main()
{
    cin>>n;
    for(int i=1;i<=n;i++)
    {
        cin>>a[i];

    }
    compute_1();
    //cout<<sum;
    //cout<<endl;
    //compute_2();
    //cout<<sum;
    return 0;
}
