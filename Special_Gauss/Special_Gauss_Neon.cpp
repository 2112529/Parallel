#define NEON_ALG
#ifdef NEON_ALG

#include <iostream>
#include <cstdlib> //rand
#include <sys/time.h> //gettimeofday
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include<arm_neon.h>

using namespace std;

typedef long long MyInteger;
const MyInteger IntegerLen = 64;
const MyInteger VecLength = 2;
const MyInteger MaxRowCount = 38016;//不小于实际最大行数的最小的IntegerLen的倍数
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


MyInteger SpecialNeonStandardForSingleRow() {//返回消元完毕后最左侧非零元所在列，若全为0则返回-1
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i]) {
            for (MyInteger j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j)) {
                    MyInteger colnow = i * IntegerLen + j;
                    if (occupyRow[colnow]) {
                        MyInteger colmax = (i + 1) / VecLength * VecLength;
                        for (MyInteger k = 0; k < colmax; k += VecLength) {
                            int64x2_t xor_vectmp = vld1q_s64((int64_t*)&mtrx[colnow][k]);
                            int64x2_t mtrx_vectmp = vld1q_s64((int64_t*)&ques[k]);
                            mtrx_vectmp = veorq_s64(mtrx_vectmp, xor_vectmp);
                            vst1q_s64((int64_t*)&ques[k], mtrx_vectmp);
                        }
                        for (MyInteger k = colmax; k <= i; k++)
                            ques[k] ^= mtrx[colnow][k];
                    }
                    else return colnow;
                }
        }
    return -1;
}
double SpecialNeonStandard() {
    finishReadFlag = false;
    double elapsed_time = 0;
    timeval start_time, end_time;
    while (!finishReadFlag) {
        inputRow(ques);

        gettimeofday(&start_time, NULL);
        MyInteger rowIndex = SpecialNeonStandardForSingleRow();
        gettimeofday(&end_time, NULL);
        elapsed_time += (end_time.tv_sec - start_time.tv_sec) * 1000.0
            + (end_time.tv_usec - start_time.tv_usec) / 1000.0;

        LevelUpRow(rowIndex);
        outputRow(ques);
    }
    cout << "Elapsed Time(Standard):" << elapsed_time << endl;
    return elapsed_time;
}


MyInteger SpecialNeonUnalignedForSingleRow() {//返回消元完毕后最左侧非零元所在列，若全为0则返回-1
    for (MyInteger i = MaxRowCount / IntegerLen - 1; i >= 0; i--)
        if (ques[i]) {
            for (MyInteger j = IntegerLen - 1; j >= 0; j--)
                if (getColBit(ques, i, j)) {
                    MyInteger colnow = i * IntegerLen + j;
                    if (occupyRow[colnow]) {
                        MyInteger segStart = i;
                        while (segStart >= VecLength) {
                            segStart -= VecLength;
                            int64x2_t xor_vectmp = vld1q_s64((int64_t*)&mtrx[colnow][segStart + 1]);
                            int64x2_t mtrx_vectmp = vld1q_s64((int64_t*)&ques[segStart + 1]);
                            mtrx_vectmp = veorq_s64(mtrx_vectmp, xor_vectmp);
                            vst1q_s64((int64_t*)&ques[segStart + 1], mtrx_vectmp);
                        }
                        for (MyInteger k = 0; k <= segStart; k++)
                            ques[k] ^= mtrx[colnow][k];
                    }
                    else return colnow;
                }
        }
    return -1;
}
double SpecialNeonUnaligned() {
    finishReadFlag = false;
    double elapsed_time = 0;
    timeval start_time, end_time;
    while (!finishReadFlag) {
        inputRow(ques);

        gettimeofday(&start_time, NULL);
        MyInteger rowIndex = SpecialNeonUnalignedForSingleRow();
        gettimeofday(&end_time, NULL);
        elapsed_time += (end_time.tv_sec - start_time.tv_sec) * 1000.0
            + (end_time.tv_usec - start_time.tv_usec) / 1000.0;

        LevelUpRow(rowIndex);
        outputRow(ques);
    }
    cout << "Elapsed Time(Unaligned):" << elapsed_time << endl;
    return elapsed_time;
}

signed main() {
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
        //Standard
        fins.open(prematrixFilePath);
        inputPreMatrix();
        fins.close();

        fins.open(sufmatrixFilePath);
        fouts.open(resultFilePath);
        SpecialNeonStandard();
        fouts.close();
        fins.close();

        clearData();

        //Unaligned
        fins.open(prematrixFilePath);
        inputPreMatrix();
        fins.close();

        fins.open(sufmatrixFilePath);
        fouts.open(resultFilePath);
        SpecialNeonUnaligned();
        fouts.close();
        fins.close();
    }
    

    

    return 0;
}
#endif

