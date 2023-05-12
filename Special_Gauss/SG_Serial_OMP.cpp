#define SERIAL_ALG
#ifdef SERIAL_ALG

#include <iostream>
#include <cstdlib> //rand
#include <sys/time.h> //gettimeofday
#include <fstream>
#include <sstream>
#include <string>
#include<omp.h>
using namespace std;

const int IntegerLen = 32;
const int MaxRowCount = 3900;//不小于实际最大行数的最小的IntegerLen的倍数
bool finishReadFlag, occupyRow[MaxRowCount];
alignas(32) int mtrx[MaxRowCount][MaxRowCount / IntegerLen];
alignas(32) int ques[MaxRowCount / IntegerLen];//当前待消元行
ifstream fins;
ofstream fouts;

const int NUMS_THREADS = 4;
inline bool getColBit(int* binarr, int col) {
    if (col >= MaxRowCount)return false;
    return (binarr[col / IntegerLen] & (1 << (col % IntegerLen))) != 0;
}
inline bool getColBit(int* binarr, int segIndex, int bitIndex) {
    if (segIndex >= MaxRowCount / IntegerLen || bitIndex >= IntegerLen || bitIndex < 0)return false;
    return (binarr[segIndex] & (1 << bitIndex)) != 0;
}
int inputRow(int* binarr) {
    char chtmp; int colIndex, firstColIndex = -1;
    string colIndexes;
    getline(fins, colIndexes);
    stringstream colIndexesStream(colIndexes);
    for (int i = MaxRowCount / IntegerLen - 1; i >= 0; i--)binarr[i] = 0;
    while (colIndexesStream >> colIndex) {
        binarr[colIndex / IntegerLen] |= 1 << (colIndex % IntegerLen);
        firstColIndex = max(firstColIndex, colIndex);
    }
    if (fins.eof())finishReadFlag = true;
    return firstColIndex;
}
void outputRow(int* binarr) {
    for (int i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i])
            for (int j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j))
                    fouts << i * IntegerLen + j << " ";
    fouts << "\n";
}

int SpecialSerialForSingleRow() {//返回消元完毕后最左侧非零元所在列，若全为0则返回-1
    for (int i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i]) {
            for (int j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j)) {
                    int colnow = i * IntegerLen + j;
                    if (occupyRow[colnow]) {
                        bool parallel = true;
#pragma omp parallel if(parallel), num_threads(NUMS_THREADS)
                        for (int k = i; k >= 0; k--)
                            ques[k] ^= mtrx[colnow][k];
                    }
                    else return colnow;
                }
        }
    return -1;
}

void LevelUpRow(int rowIndex) {
    if (rowIndex < 0 || rowIndex >= MaxRowCount)return;
    for (int i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        mtrx[rowIndex][i] = ques[i];
    occupyRow[rowIndex] = true;
}
double SpecialSerial() {
    finishReadFlag = false;
    double elapsed_time = 0;
    timeval start_time, end_time;
    while (!finishReadFlag) {
        inputRow(ques);

        gettimeofday(&start_time, NULL);
        int rowIndex = SpecialSerialForSingleRow();
        gettimeofday(&end_time, NULL);
        elapsed_time += (end_time.tv_sec - start_time.tv_sec) * 1000.0
            + (end_time.tv_usec - start_time.tv_usec) / 1000.0;

        LevelUpRow(rowIndex);
        outputRow(ques);
    }
    cout << "Elapsed Time:" << elapsed_time << endl;
    return elapsed_time;
}
void inputPreMatrix() {
    finishReadFlag = false;
    int firstColIndex;
    while (!finishReadFlag) {
        firstColIndex = inputRow(ques);
        for (int i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
            mtrx[firstColIndex][i] = ques[i];
        occupyRow[firstColIndex] = true;
    }
}

int main() {
    ios::sync_with_stdio(false);
    string s1[5];
    s1[0] = "/home/data/Groebner/5_2362_1226_453/1.txt";
    s1[1] = "/home/data/Groebner/6_3799_2759_1953/1.txt";
    s1[2] = "/home/data/Groebner/7_8399_6375_4535/1.txt";
    s1[3] = "/home/data/Groebner/8_23045_18748_14325/1.txt";
    s1[4] = "/home/data/Groebner/9_37960_29304_14921/1.txt";
    string s2[5];
    s2[0] = "/home/data/Groebner/5_2362_1226_453/2.txt";
    s2[1] = "/home/data/Groebner/6_3799_2759_1953/2.txt";
    s2[2] = "/home/data/Groebner/7_8399_6375_4535/2.txt";
    s2[3] = "/home/data/Groebner/8_23045_18748_14325/2.txt";
    s2[4] = "/home/data/Groebner/9_37960_29304_14921/2.txt";

    for (int i = 0; i < 5; i++)
    {
        string prematrixFilePath = s1[i];
        string sufmatrixFilePath = s2[i];
        string resultFilePath = "/home/ss2112529/Special_Gauss/myresult.txt";

        fins.open(prematrixFilePath);
        inputPreMatrix();
        fins.close();

        fins.open(sufmatrixFilePath);
        fouts.open(resultFilePath);
        SpecialSerial();
        fouts.close();
        fins.close();
    }

    return 0;
}

#endif