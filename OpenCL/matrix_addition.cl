__kernel void matrix_add(__global const float* A, __global const float* B, __global float* C, const int N) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    if (i < N && j < N) {
        C[i * N + j] = A[i * N + j] + B[i * N + j];
    }
}