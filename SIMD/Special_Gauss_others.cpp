#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<math.h>
#include<Windows.h>

using namespace std;

//将字符串转换为01矩阵
void string_to_num(string str, int row, int l, int** arr) {
    string s;
    int a;
    stringstream ss(str);//将字符串str转换为stringstream类型的对象ss 并将字符串作为对象的初始化值
    //这种转换对字符串进行输入输出操作，ss>>来从字符串中读取值，ss<<来向字符串中写入值
    while (ss >> s) {
        stringstream ts;
        ts << s;
        ts >> a;
        arr[row][l - a - 1] = 1; //将01矩阵中的元素填入
    }
}

// 获取01矩阵中第一个为1的元素的下标，返回值为从右往左第一个1的下标
int get_first_1(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == 1)
            return size - 1 - i; // 返回从右往左第一个1的下标
        else
            continue;
    }
    return -1;
}

// 判断是否已经存在相同的行，如果是则返回行号，如果不是则返回-1
int _exist(int** E, int* Ed, int row, int line) {
    for (int i = 0; i < row; i++) {
        if (get_first_1(E[i], line) == get_first_1(Ed, line)) // 判断两行从右往左第一个1的下标是否相同
            return i; // 返回相同行的行号
    }
    return -1;
}

// 特殊高斯消元方法
void special_Gauss(int** E, int** Ed, int row, int rowd, int line) {
    int count = row - rowd; // 初始化count，表示存储消元结果的01矩阵E的有效行数
    long long head, tail, freq;
    double sum_time = 0.0;
    for (int i = 0; i < rowd; i++) { // 对Ed中每一行进行消元
        while (get_first_1(Ed[i], line) != -1) { // 当被消元行不全为0时
            int exist_or_not = _exist(E, Ed[i], row, line); // 判断消元结果中是否已经存在和该被消元行相同的行，若存在则返回该行的行号，否则返回-1
            if (exist_or_not != -1) { // 存在与当前被消元行相同的行
                QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
                QueryPerformanceCounter((LARGE_INTEGER*)&head);
                int k;
                for (k = 0; k < line; k++) {
                    Ed[i][k] = Ed[i][k] ^ E[exist_or_not][k]; // 使用异或运算来实现消元，将该行与消元结果中相同的行进行异或
                }
                QueryPerformanceCounter((LARGE_INTEGER*)&tail);
                sum_time += (tail - head) * 1000.0 / freq;
            }
            else { // 不存在与当前被消元行相同的行，将该行加入到消元结果中
                for (int k = 0; k < line; k++) {
                    E[count][k] = Ed[i][k];
                }
                count++;
                break; // 跳出while循环，对下一行进行消元
            }
        }
    }
    cout << "The alg takes:" << sum_time << "ms" << endl; // 输出消元时间
}

int main() {
    ifstream eliminate;//消元子
    ifstream eliminated;//被消元子
    ifstream data;//行列数据
    ofstream result;//结果
    int row, line;//消元子的行与列数
    int rowd, lined;//被消元子的行与列数
    data.open("C:\\Users\\MT.37\\Desktop\\Gause1\\readme.txt", ios::in);
    data >> line;
    data >> row;
    data >> rowd;
    lined = line;
    row += rowd;
    int** E = new int* [row];
    for (int i = 0; i < row; i++)
        E[i] = new int[line];
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < line; j++) {
            E[i][j] = 0;
        }
    }//消元矩阵初始化
    int** Ed = new int* [rowd];
    for (int i = 0; i < rowd; i++)
        Ed[i] = new int[lined];
    for (int i = 0; i < rowd; i++) {
        for (int j = 0; j < lined; j++) {
            Ed[i][j] = 0;//被消元矩阵初始化
        }
    }

    eliminate.open("C:\\Users\\MT.37\\Desktop\\Gause1\\消元子.txt", ios::in);
    if (!eliminate.is_open()) {
        cout << "消元子文件打开失败" << endl;
        return 1;
    }
    vector<string> elte;
    string temp1;
    while (getline(eliminate, temp1))
        elte.push_back(temp1);
    eliminate.close();
    for (int i = 0; i < elte.size(); i++)
        string_to_num(elte[i], i, line, E);
    eliminated.open("C:\\Users\\MT.37\\Desktop\\Gause1\\被消元行.txt", ios::in);
    if (!eliminated.is_open()) {
        cout << "被消元行文件打开失败" << endl;
        return 1;
    }
    vector<string> elted;
    string temp2;
    while (getline(eliminated, temp2))
        elted.push_back(temp2);
    eliminated.close();
    for (int i = 0; i < elted.size(); i++)
        string_to_num(elted[i], i, lined, Ed);

    special_Gauss(E, Ed, row, rowd, line); // 调用特殊高斯消元方法
    result.open("C:\\Users\\MT.37\\Desktop\\消元结果.txt", ios::out);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < line; j++)
        {
            result << E[i][j];
        }
        result << endl;
    }
    // 释放内存
    for (int i = 0; i < row; i++)
        delete[] E[i];
    delete[] E;
    for (int i = 0; i < rowd; i++)
        delete[] Ed[i];
    delete[]Ed;
    return 0;
}