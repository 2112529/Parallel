#include <bits/stdc++.h>
using namespace std;

//位图的实现
class BitMap
{
public:
    //位图的内存大小和数据范围有关
    BitMap(size_t range)
        :_bit(range / 32 + 1)
    {}
    void set(const size_t num)
    {
        //计算数组中的下标
        int idx = num / 32;
        //计算num在对应下标整数中的下标位置
        int bitIdx = num % 32;
        //将对应的比特位置1
        _bit[idx] |= 1 << bitIdx;
    }
    bool find(const size_t num)
    {
        int idx = num / 32;
        int bitIdx = num % 32;
        return (_bit[idx] >> bitIdx) & 1;
    }
    void reset(const size_t num)
    {
        int idx = num / 32;
        int bitIdx = num % 32;
        _bit[idx] &= ~(1 << bitIdx);
    }
private:
    vector<int> _bit;
};

//首先对拿到的数据集进行预处理，读取文件，存储在位图中
const int N = 5000;
BitMap* bitmap[12];
//核心处理函数
void set()//不断更新处理中的bitmap
{
    
}
void SIMD_unique_Gauss()
{
    for (int k = 0; k < n; k++)//表示一共n行，处理第k行
    {
        while (bitmap[k] != 0)
        {
            //进行按位异或操作
            for (int i = 0; i < e_num; i++)
            {
                bitmap[k] ^= e[i]
            }

        }
        if (bitmap[k] != 0)
        {
            e[firstn0] = bitmap[k];
        }

    }
    return;
}
void print()
{
    for (int k = 0; k < n; k++)
    {
        while (bitmap[k] != 0)
        {
            cout << bitmap[k] << endl;
        }
    }
    return;

}
int main()
{
    for (int i = 0; i < 11; i++)
    {
        int num;
        cin >> num;
        bitmap[i] = new BitMap(num);
    }
    return 0;
}