/*
* @author R. Karimov, 
* @date 2025-06-18
* To compile your source code, please use the provided makefile 
* In this exercise we try to compete with the OpenCL implementation of matrix multiplication
*/
#define CL_TARGET_OPENCL_VERSION 120

#include <gegl-0.4/opencl/cl.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "fvec/P4_F32vec4.h"
#include <immintrin.h>
#include "tbb/tbb.h"
#include <omp.h>



using Clock_t = std::chrono::steady_clock;
using TimeUnit_t = std::chrono::milliseconds;


const std::string time_units = " ms";


bool CompareMatrices(const std::vector<float>& mat1, const std::vector<float>& mat2, float tolerance, int N) {
    for (int i = 0; i < N * N; ++i) {
        if (std::abs(mat1[i] - mat2[i]) > tolerance) {
            return false;
        }
    }
    return true;
}


void ScalarMatrixMultiplication(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}


void fastMatrixMultiplication(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    const int BLOCK_SIZE = 64;

    tbb::parallel_for(
        tbb::blocked_range2d<size_t>(0, N, BLOCK_SIZE, 0, N, BLOCK_SIZE),
        [&](const tbb::blocked_range2d<size_t>& r) {
            for (size_t i = r.rows().begin(); i < r.rows().end(); i++) {
                for (size_t j = r.cols().begin(); j < r.cols().end(); j += 8) {
                    // Process 8 columns at a time
                    __m256 sum = _mm256_setzero_ps();
                    
                    for (size_t k = 0; k < N; k++) {
                        // Load one element from A[i,k]
                        __m256 a = _mm256_broadcast_ss(&A[i * N + k]);
                        
                        // Load 8 elements from B[k,j:j+8]
                        __m256 b = _mm256_loadu_ps(&B[k * N + j]);
                        
                        // Multiply and accumulate
                        sum = _mm256_fmadd_ps(a, b, sum);
                    }
                    
                    // Store the result
                    _mm256_storeu_ps(&C[i * N + j], sum);
                }
            }
        }
    );
}

void fastMatrixMultiplicationV2(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    const int BLOCK_SIZE = 64;
    
 

    tbb::parallel_for(
        tbb::blocked_range3d<size_t>(0, N, BLOCK_SIZE, 0, N, BLOCK_SIZE, 0, N, BLOCK_SIZE),
        [&](const tbb::blocked_range3d<size_t>& r) {
            for (size_t ii = r.pages().begin(); ii < r.pages().end(); ii++) {
                for (size_t jj = r.cols().begin(); jj < r.cols().end(); jj++) {
                    for (size_t kk = r.rows().begin(); kk < r.rows().end(); kk++) {
                        
                        // Process the block
                        for (size_t i = ii; i < std::min(ii + BLOCK_SIZE, (size_t)N); i++) {
                            for (size_t k = kk; k < std::min(kk + BLOCK_SIZE, (size_t)N); k++) {
                                __m256 a = _mm256_broadcast_ss(&A[i * N + k]);
                                
                                size_t j = jj;
                                for (; j + 7 < std::min(jj + BLOCK_SIZE, (size_t)N); j += 8) {
                                    __m256 b = _mm256_loadu_ps(&B[k * N + j]);
                                    __m256 c = _mm256_loadu_ps(&C[i * N + j]);
                                    c = _mm256_fmadd_ps(a, b, c);
                                    _mm256_storeu_ps(&C[i * N + j], c);
                                }
                                
                                for (; j < std::min(jj + BLOCK_SIZE, (size_t)N); j++) {
                                    C[i * N + j] += A[i * N + k] * B[k * N + j];
                                }
                            }
                        }
                    }
                }
            }
        }
    );
}

std::string readKernelSource(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


void checkError(cl_int err, const char* operation) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error during operation '" << operation << "': " << err << std::endl;
        exit(1);
    }
}


int main() {
    cl_int err;

    const int N = 2048;
    float tolerance = 1e-6;

    std::vector<float> A(N * N, 1.0f), B(N * N, 2.0f), C(N * N, 0.0f), C_scalar(N * N, 0.0f);

    auto start = Clock_t::now();

    cl_uint platformCount;
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, &platformCount);
    checkError(err, "clGetPlatformIDs");

    cl_uint deviceCount;
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
    checkError(err, "clGetDeviceIDs");

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * A.size(), A.data(), &err);
    checkError(err, "clCreateBuffer (bufferA)");
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * B.size(), B.data(), &err);
    checkError(err, "clCreateBuffer (bufferB)");
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * C.size(), NULL, &err);
    checkError(err, "clCreateBuffer (bufferC)");

    std::string sourceStr = readKernelSource("matrix_multiplication.cl");
    const char* source = sourceStr.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
    checkError(err, "clCreateProgramWithSource");

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), NULL);
        std::cerr << "Error during operation 'clBuildProgram': " << err << std::endl;
        std::cerr << "Build log:" << std::endl << log.data() << std::endl;
        exit(1);
    }

    cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &err);
    checkError(err, "clCreateKernel");

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    checkError(err, "clSetKernelArg (bufferA)");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    checkError(err, "clSetKernelArg (bufferB)");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    checkError(err, "clSetKernelArg (bufferC)");
    err = clSetKernelArg(kernel, 3, sizeof(int), &N);
    checkError(err, "clSetKernelArg (N)");

    size_t global_work_size[2] = { (size_t)N, (size_t)N };
    size_t local_work_size[2] = { 16, 16 };

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data(), 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer");

    auto finish = Clock_t::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "OpenCL - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);


    //Hyper Optimized SIMD TBB Method 
    start = Clock_t::now(); 
    fastMatrixMultiplication(A, B, C_scalar, N); 
    finish = Clock_t::now(); 
    elapsed = finish-start; 
    std::cout << "Fast Mat Mul - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl; 
    
    bool isSameBase = CompareMatrices(C, C_scalar, tolerance, N); 

    if (isSameBase) {
       std::cout << "The matrices are the same within tolerance." << std::endl;
    } else {
        std::cout << "The matrices are different." << std::endl;
    }
    return 0;
}
