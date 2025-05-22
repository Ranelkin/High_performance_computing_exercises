// -*- C++ -*-
/*
==================================================
Authors: A. Mithran; I. Kulakov; M. Zyzak
==================================================
*/
/// use "g++ -O3 -fno-tree-vectorize -msse QuadraticEqn.cpp && ./a.out" to run
/// Note:
/// __m128 - SIMD vector
/// SIMD intrinsics:
/// _mm_set_ps(f3,f2,f1,f0) - write 4 floats into the SIMD vector
/// note, that the order of entries is inversed
/// _mm_set_ps1(a) - write float "a" into the SIMD vector
/// _mm_add_ps(a,b) - a+b
/// _mm_sub_ps(a,b) - a-b
/// _mm_mul_ps(a,b) - a*b
/// _mm_div_ps(a,b) - a/b
/// _mm_sqrt_ps(a) - sqrt(a)
/// type2* pointer2 = reinterpret_cast<type2*>( pointer1 ) - change pointer type

#include "fvec/P4_F32vec4.h"    // wrapper of the SSE instruction
#include "utils/TStopWatch.h"

#include <cmath>
#include <iostream>

#include <stdlib.h> // rand

static const int NVectors = 10000;
static const int N = NVectors * fvecLen;

static const int NIterOut = 1000;

void CheckResults(const float* yScalar, const float* ySIMD, const int NSIMD)
{
    bool ok = true;
    for (int i = 0; i < N; i++)
        if (fabs(yScalar[i] - ySIMD[i]) > yScalar[i] * 0.01) {
            ok = false;
            //      std::cout << i<<" " << yScalar[i] << " " << ySIMD[i] << " " << fabs(yScalar[i] - ySIMD[i])<<std::endl;
        }
    if (!ok)
        std::cout << "ERROR! SIMD" << NSIMD
                  << " and scalar results are not the same." << std::endl;
    else
      std::cout << "SIMD" << NSIMD << " and scalar results are the same." << std::endl;
}

int main()
{
    //input data
    float* a = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* b = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* c = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    //output data
    float* x = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd1 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd2 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd3 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd4 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));

    // fill parameters by random numbers
    for (int i = 0; i < N; i++) {
        a[i] =
            0.01 + float(rand()) / float(RAND_MAX); // put a random value, from 0.01 to 1.01 (a has not to be equal 0)
        b[i] = float(rand()) / float(RAND_MAX);
        c[i] = -float(rand()) / float(RAND_MAX);
    }

    /// -- CALCULATE --

    // scalar calculations
    TStopwatch timerScalar;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < N; i++) {
            float det = b[i] * b[i] - 4 * a[i] * c[i];
            x[i] = (-b[i] + sqrt(det)) / (2 * a[i]);
        }
    timerScalar.Stop();

    // SIMD clculations with SIMD intrinsics and data copy
    TStopwatch timerSIMD;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < NVectors; i++) {
            __m128 aV = _mm_set_ps(a[i*4+3], a[i*4+2], a[i*4+1], a[i*4]);
            __m128 bV = _mm_set_ps(b[i*4+3], b[i*4+2], b[i*4+1], b[i*4]);
            __m128 cV = _mm_set_ps(c[i*4+3], c[i*4+2], c[i*4+1], c[i*4]);

            __m128 four = _mm_set_ps1(4.0f);
            __m128 two = _mm_set_ps1(2.0f);
            __m128 det = _mm_sub_ps(_mm_mul_ps(bV, bV), _mm_mul_ps(four, _mm_mul_ps(aV, cV)));
            __m128 xV = _mm_div_ps(_mm_add_ps(_mm_sub_ps(_mm_set_ps1(0.0f), bV), _mm_sqrt_ps(det)), _mm_mul_ps(two, aV));

            float* x_ptr = reinterpret_cast<float*>(&xV);
            for (int iE = 0; iE < fvecLen; iE++)
                x_simd1[i*fvecLen+iE] = x_ptr[iE];
        }
    timerSIMD.Stop();

    // SIMD clculations with SIMD intrinsics and reinterpret_cast
    TStopwatch timerSIMD2;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < N; i += fvecLen) {
            __m128 aV = _mm_load_ps(reinterpret_cast<float*>(&a[i]));
            __m128 bV = _mm_load_ps(reinterpret_cast<float*>(&b[i]));
            __m128 cV = _mm_load_ps(reinterpret_cast<float*>(&c[i]));

            __m128 four = _mm_set_ps1(4.0f);
            __m128 two = _mm_set_ps1(2.0f);
            __m128 det = _mm_sub_ps(_mm_mul_ps(bV, bV), _mm_mul_ps(four, _mm_mul_ps(aV, cV)));
            __m128 xV = _mm_div_ps(_mm_add_ps(_mm_sub_ps(_mm_set_ps1(0.0f), bV), _mm_sqrt_ps(det)), _mm_mul_ps(two, aV));

            // store output
            _mm_store_ps(&x_simd2[i], xV);
        }
    timerSIMD2.Stop();

    // SIMD clculations with headers and data copy
    TStopwatch timerSIMD3;
    for (int io = 0; io < NIterOut; io++) {
        for (int i = 0; i < NVectors; i++) {
            F32vec4 aV(a[i*4], a[i*4+1], a[i*4+2], a[i*4+3]);
            F32vec4 bV(b[i*4], b[i*4+1], b[i*4+2], b[i*4+3]);
            F32vec4 cV(c[i*4], c[i*4+1], c[i*4+2], c[i*4+3]);

            F32vec4 four(4.0f);
            F32vec4 two(2.0f);
            F32vec4 det = bV * bV - four * aV * cV;
            F32vec4 xV = (-bV + sqrt(det)) / (two * aV);

            for (int iE = 0; iE < fvecLen; iE++)
                x_simd3[i*fvecLen+iE] = xV[iE];
        }
    }
    timerSIMD3.Stop();

    // SIMD clculations with headers and reinterpret_cast
    TStopwatch timerSIMD4;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < N; i += fvecLen) {
            F32vec4 aV = *reinterpret_cast<F32vec4*>(&a[i]);
            F32vec4 bV = *reinterpret_cast<F32vec4*>(&b[i]);
            F32vec4 cV = *reinterpret_cast<F32vec4*>(&c[i]);

            F32vec4 four(4.0f);
            F32vec4 two(2.0f);
            F32vec4 det = bV * bV - four * aV * cV;
            F32vec4 xV = (-bV + sqrt(det)) / (two * aV);

            for (int iE = 0; iE < fvecLen; iE++)
                x_simd4[i+iE] = xV[iE];
        }
    timerSIMD4.Stop();

    double tScal = timerScalar.RealTime() * 1000;
    double tSIMD1 = timerSIMD.RealTime() * 1000;
    double tSIMD2 = timerSIMD2.RealTime() * 1000;
    double tSIMD3 = timerSIMD3.RealTime() * 1000;
    double tSIMD4 = timerSIMD4.RealTime() * 1000;
    std::cout << "Time scalar: " << tScal << " ms " << std::endl;
    std::cout << "Time SIMD1:   " << tSIMD1 << " ms, speed up "
              << tScal / tSIMD1 << std::endl;
    std::cout << "Time SIMD2:   " << tSIMD2 << " ms, speed up "
              << tScal / tSIMD2 << std::endl;
    std::cout << "Time SIMD3:   " << tSIMD3 << " ms, speed up "
              << tScal / tSIMD3 << std::endl;
    std::cout << "Time SIMD4:   " << tSIMD4 << " ms, speed up "
              << tScal / tSIMD4 << std::endl;

    //compare SIMD and scalar results
    CheckResults(x, x_simd1, 1);
    CheckResults(x, x_simd2, 2);
    CheckResults(x, x_simd3, 3);
    CheckResults(x, x_simd4, 4);

    return 1;
}
