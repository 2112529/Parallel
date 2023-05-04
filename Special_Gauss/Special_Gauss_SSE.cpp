#define SSE_ALG
#ifdef SSE_ALG

#include <iostream>
#include <cstdlib> //rand
#include <time.h> //gettimeofday
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include<immintrin.h>
#include <sys/time.h> //gettimeofday
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
using namespace std;


typedef long long LARGER_INTERGER;
typedef long long MyInteger;
const MyInteger IntegerLen = 64;
const MyInteger VecLength = 2;
const MyInteger MaxRowCount = 24000;//不小于实际最大行数的最小的IntegerLen的倍数
bool finishReadFlag, occupyRow[MaxRowCount];
alignas(32) MyInteger mtrx[MaxRowCount][MaxRowCount / IntegerLen];
alignas(32) MyInteger ques[MaxRowCount / IntegerLen];//当前待消元行
ifstream fins;
ofstream fouts;
inline bool getColBit(MyInteger* binarr, MyInteger col) {
    if (col >= MaxRowCount)return false;
    return (binarr[col / IntegerLen] & (MyInteger(1) << (col % IntegerLen))) != 0;
}
inline bool getColBit(MyInteger* binarr, MyInteger segIndex, MyInteger bitIndex) {
    if (segIndex >= MaxRowCount / IntegerLen || bitIndex >= IntegerLen || bitIndex < 0)return false;
    return (binarr[segIndex] & (MyInteger(1) << bitIndex)) != 0;
}
MyInteger inputRow(MyInteger* binarr) {
    MyInteger colIndex, firstColIndex = -1;
    string colIndexes;
    getline(fins, colIndexes);
    stringstream colIndexesStream(colIndexes);
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)binarr[i] = 0;
    while (colIndexesStream >> colIndex) {
        binarr[colIndex / IntegerLen] |= MyInteger(1) << (colIndex % IntegerLen);
        firstColIndex = max(firstColIndex, colIndex);
    }
    if (fins.eof())finishReadFlag = true;
    return firstColIndex;
}
void outputRow(MyInteger* binarr) {
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i])
            for (MyInteger j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j))
                    fouts << i * IntegerLen + j << " ";
    fouts << "\n";
}
void LevelUpRow(MyInteger rowIndex) {
    if (rowIndex < 0 || rowIndex >= MaxRowCount)return;
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        mtrx[rowIndex][i] = ques[i];
    occupyRow[rowIndex] = true;
}
void inputPreMatrix() {
    finishReadFlag = false;
    MyInteger firstColIndex;
    while (!finishReadFlag) {
        firstColIndex = inputRow(ques);
        for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
            mtrx[firstColIndex][i] = ques[i];
        occupyRow[firstColIndex] = true;
    }
}
void clearData() {
    finishReadFlag = false;
    memset(occupyRow, 0, sizeof(occupyRow));
    memset(mtrx, 0, sizeof(mtrx));
    memset(ques, 0, sizeof(ques));
}


MyInteger SpecialSSEStandardForSingleRow() {//返回消元完毕后最左侧非零元所在列，若全为0则返回-1
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i]) {
            for (MyInteger j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j)) {
                    MyInteger colnow = i * IntegerLen + j;
                    if (occupyRow[colnow]) {
                        MyInteger colmax = (i + 1) / VecLength * VecLength;
                        for (MyInteger k = 0; k < colmax; k += VecLength) {
                            __m128 xor_vectmp = _mm_load_ps((float*)&mtrx[colnow][k]);
                            __m128 mtrx_vectmp = _mm_load_ps((float*)&ques[k]);
                            mtrx_vectmp = _mm_xor_ps(mtrx_vectmp, xor_vectmp);
                            _mm_store_ps((float*)&ques[k], mtrx_vectmp);
                        }
                        for (MyInteger k = colmax; k <= i; k++)
                            ques[k] ^= mtrx[colnow][k];
                    }
                    else return colnow;
                }
        }
    return -1;
}
double SpecialSSEStandard() {
    finishReadFlag = false;
    long long head, tail, freq;
    double elapsed_time = 0.0;
    QueryPerformanceCounter((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    //在外面进行测试设计函数的调用，可能误差比较大
    while (!finishReadFlag) {
        inputRow(ques);
        
        MyInteger rowIndex = SpecialSSEStandardForSingleRow();
          
        LevelUpRow(rowIndex);
        outputRow(ques);
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    elapsed_time = double((tail - head) * 1000.000 / freq);
    std::cout << "SG_SSE_Elapsed Time(Standard):" << std::fixed << std::setprecision(2) << elapsed_time << endl;
    return elapsed_time;
}


MyInteger SpecialSSEUnalignedForSingleRow() {//返回消元完毕后最左侧非零元所在列，若全为0则返回-1
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i]) {
            for (MyInteger j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j)) {
                    MyInteger colnow = i * IntegerLen + j;
                    if (occupyRow[colnow]) {
                        MyInteger segStart = i;
                        while (segStart >= VecLength) {
                            segStart -= VecLength;
                            __m128 xor_vectmp = _mm_loadu_ps((float*)&mtrx[colnow][segStart + 1]);
                            __m128 mtrx_vectmp = _mm_loadu_ps((float*)&ques[segStart + 1]);
                            mtrx_vectmp = _mm_xor_ps(mtrx_vectmp, xor_vectmp);
                            _mm_storeu_ps((float*)&ques[segStart + 1], mtrx_vectmp);
                        }
                        //cout<<endl;
                        for (MyInteger k = 0; k <= segStart; k++)
                            ques[k] ^= mtrx[colnow][k];
                    }
                    else return colnow;
                }
        }
    return -1;
}
double SpecialSSEUnaligned() {
    finishReadFlag = false;
    long long head1, tail1,freq;
    double elapsed_time = 0.0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceFrequency((LARGE_INTEGER*)&head1);
    while (!finishReadFlag) {
        inputRow(ques);

        
        MyInteger rowIndex = SpecialSSEUnalignedForSingleRow();
        
        
        
        LevelUpRow(rowIndex);
        outputRow(ques);
    }
    QueryPerformanceFrequency((LARGE_INTEGER*)&tail1);
    elapsed_time = double((tail1 - head1) * 1000.000 / freq);
    std::cout << "SG_SSE_Elapsed Time(Unaligned):" << std::fixed << std::setprecision(2)<< elapsed_time << endl;
    return elapsed_time;
}

signed main() {
    ios::sync_with_stdio(false);

    string prematrixFilePath = "D:/Data/6_3799_2759_1953/1.txt";
    string sufmatrixFilePath = "D:/Data/6_3799_2759_1953/2.txt";
    string resultFilePath = "D:/Data/6_3799_2759_1953/myresult.txt";

    //Standard
    fins.open(prematrixFilePath);
    inputPreMatrix();
    fins.close();

    fins.open(sufmatrixFilePath);
    fouts.open(resultFilePath);
    SpecialSSEStandard();
    fouts.close();
    fins.close();

    clearData();

    //Unaligned
    fins.open(prematrixFilePath);
    inputPreMatrix();
    fins.close();

    fins.open(sufmatrixFilePath);
    fouts.open(resultFilePath);
    SpecialSSEUnaligned();
    fouts.close();
    fins.close();

    return 0;
}
#endif