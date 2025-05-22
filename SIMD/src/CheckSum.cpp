// -*- C++ -*-
/*
==================================================
Authors: A. Mithran; I. Kulakov; M. Zyzak
==================================================
*/
/// use "g++ -O3 -fno-tree-vectorize -msse CheckSum.cpp && ./a.out" to run

// make calculation parallel: a) using SIMD instructions, b) using usual int-instructions.

#include "fvec/P4_F32vec4.h"    // wrapper of the SSE instruction
#include "utils/TStopWatch.h"
#include <cstring>
#include <iostream>
using namespace std;

#include <stdlib.h> // rand

const int NIter = 100;

const int N = 4000000; // matrix size. Has to be dividable by 4.
unsigned char str[N];

template <typename T>
T Sum(const T* data, const int N)
{
    T sum = 0;

    for (int i = 0; i < N; ++i)
        sum = sum ^ data[i];
    return sum;
}

int main()
{
    // fill string by random values
    for (int i = 0; i < N; i++) {
        str[i] = 256 * (double(rand()) /
                        RAND_MAX); // put a random value, from 0 to 255
    }

    /// -- CALCULATE --

    /// SCALAR

    unsigned char sumS = 0;
    TStopwatch timerScalar;
    for (int ii = 0; ii < NIter; ii++)
        sumS = Sum<unsigned char>(str, N);
    timerScalar.Stop();

    /// SIMD

    /// SIMD
    unsigned char sumV = 0;
    TStopwatch timerSIMD;
    __m128 vsum = _mm_set_ps1(0); 
    for (int ii = 0; ii < NIter; ii++) {
        vsum = _mm_set_ps1(0);
        for (int i = 0; i < N; i += 16) { 
            __m128 v = _mm_loadu_ps(reinterpret_cast<const float*>(&str[i]));
            vsum = _mm_xor_ps(vsum, v);
        }
        unsigned char* bytes = reinterpret_cast<unsigned char*>(&vsum);
        unsigned char temp = bytes[0];
        for (int j = 1; j < 16; j++)
            temp ^= bytes[j];
        if (ii == 0) sumV = temp; 
    }
    timerSIMD.Stop();

    /// SCALAR INTEGER

    unsigned char sumI = 0;
    TStopwatch timerINT;
    for (int ii = 0; ii < NIter; ii++) {
        unsigned int isum = 0;
        for (int i = 0; i < N - 3; i += 4) { // Ensure N is large enough
            unsigned int v;
            memcpy(&v, &str[i], sizeof(unsigned int));
            isum ^= v;
        }
        // Handle remaining bytes (if N is not a multiple of 4)
        for (int i = (N - N % 4); i < N; i++) {
            isum ^= static_cast<unsigned int>(str[i]);
        }
        unsigned char* bytes = reinterpret_cast<unsigned char*>(&isum);
        unsigned char temp = bytes[0];
        for (int j = 1; j < 4; j++) {
            temp ^= bytes[j];
        }
        if (ii == 0) sumI = temp; // Store result from first iteration
    }
    timerINT.Stop();

    /// -- OUTPUT --

    double tScal = timerScalar.RealTime() * 1000;
    double tINT = timerINT.RealTime() * 1000;
    double tSIMD = timerSIMD.RealTime() * 1000;

    cout << "Time scalar: " << tScal << " ms " << endl;
    cout << "Value scalar: " << sumS << endl; 
    cout << "Time INT:   " << tINT << " ms, speed up " << tScal / tINT << endl;
    cout << "Value int: " << sumI << endl; 
    cout << "Time SIMD:   " << tSIMD << " ms, speed up " << tScal / tSIMD
         << endl;
    cout << "Value simd: " << sumV << endl; 

    cout << static_cast<int>(sumS) << " " << static_cast<int>(sumV) << endl;
    if (sumV == sumS && sumI == sumS)
        std::cout << "Results are the same." << std::endl;
    else
        std::cout << "ERROR! Results are not the same." << std::endl;

    return 0;
}
