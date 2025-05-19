// -*- C++ -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#include "Utils.h"
#include <algorithm>
#include <random>
#include <numeric>

namespace Utils
{
  void MatVecMul(const std::vector<std::vector<float>>& matrix,
                 const std::vector<float>& vector,
                 std::vector<float>& result){
    for(int i = 0; i<result.size(); i++){
      float res=0; 
      for (int j = 0; j < matrix[0].size(); j++){
        res += matrix[i][j] * vector[j]; 
      }
      result[i] = res; 
    }
  }

  void MatTransposeVecMul(const std::vector<std::vector<float>>& matrix, const std::vector<float>& vector,
                          std::vector<float>& result)
  {
    
    for (int i = 0; i < matrix[0].size(); i++) {
      float sum = 0;
      for (int j = 0; j < matrix.size(); j++) {
        sum += matrix[j][i] * vector[j];
      }
      result[i] = sum;
    }
  }

  void Transpose(const std::vector<std::vector<float>>& matrix, std::vector<std::vector<float>>& result)
  {
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    
    // Resize result to have cols rows and rows cols
    result.resize(cols);
    for(size_t i = 0; i < cols; i++) {
        result[i].resize(rows);
    }

    for(size_t i = 0; i < rows; i++){
      for (size_t j = 0; j < cols; j++){
        result[j][i] = matrix[i][j]; 
      }
    }
  }

  void VecAdd(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result)
  {
    for (int i = 0; i< vectorA.size(); i++){
      result[i] = vectorA[i] + vectorB[i]; 
    }
  }

  void VecSub(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result)
  {
    size_t size = vectorA.size();

    for (size_t idx = 0; idx < size; idx++) {
      result[idx] = vectorA[idx] - vectorB[idx];
    }
  }

  void VecSca(std::vector<float>& vector, float scalar, std::vector<float>& result)
  {
    size_t size = vector.size();

    for (size_t idx = 0; idx < size; idx++) {
      result[idx] = vector[idx] * scalar;
    }
  }


  void AffineTransform(const std::vector<std::vector<float>>& matrixA,
                       std::vector<float>& vectorX,
                       std::vector<float>& vectorB,
                       std::vector<float>& result)
  {
    MatVecMul(matrixA, vectorX, result);
    VecAdd(result, vectorB, result);
  }

  void OuterProduct(const std::vector<float>& a, const std::vector<float>& b, std::vector<std::vector<float>>& result)
  {
    for (size_t i = 0; i < a.size(); ++i) {
      for (size_t j = 0; j < b.size(); ++j) {
        result[i][j] = a[i] * b[j];
      }
    }
  }

  void OuterProductAdd(const std::vector<float>& a,
                       const std::vector<float>& b,
                       std::vector<std::vector<float>>& result)
  {
    for (size_t i = 0; i < a.size(); ++i) {
      for (size_t j = 0; j < b.size(); ++j) {
        result[i][j] += a[i] * b[j];
      }
    }
  }

  void HadamardProduct(const std::vector<float>& vectorA, const std::vector<float>& vectorB, std::vector<float>& result)
  {
     for(int i=0; i<result.size(); i++){
      result[i] = vectorA[i]*vectorB[i]; 
     }
  }

  void FillRandomly(std::vector<float>& vector, float lowerBound, float upperBound)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(lowerBound, upperBound);
    for (float& element: vector) {
      element = dis(gen);
    }
  }


  void FillRandomly(std::vector<std::vector<float>>& matrix, float lowerBound, float upperBound)
  {
    for (std::vector<float>& row: matrix) {
      FillRandomly(row, lowerBound, upperBound);
    }
  }


  void FillRandomlyPyTorch(std::vector<float>& vector, size_t nInputFeatures)
  {
    float k = sqrtf(1.f / float(nInputFeatures));
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<float> dist(-k, k);

    for (float& element: vector) {
      element = dist(generator);
    }
  }


  void FillRandomlyPyTorch(std::vector<std::vector<float>>& matrix, size_t nInputFeatures)
  {
    for (std::vector<float>& row: matrix) {
      FillRandomlyPyTorch(row, nInputFeatures);
    }
  }


  void Shuffle(std::vector<std::vector<float>>& inputFeatures, std::vector<size_t>& labels)
  {
    const size_t n = inputFeatures.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);

    auto rng = std::default_random_engine{};
    std::shuffle(indices.begin(), indices.end(), rng);

    std::vector<std::vector<float>> tempFeatures = inputFeatures;
    std::vector<size_t> tempLabels = labels;

    for(size_t i = 0; i < n; i++) {
      inputFeatures[i] = tempFeatures[indices[i]];
      labels[i] = tempLabels[indices[i]];
    }
  }

  void Zeros(std::vector<float>& vector)
  {
    std::fill(vector.begin(), vector.end(), 0.f);
  }

  void Zeros(std::vector<std::vector<float>>& matrix)
  {
    for (std::vector<float>& row: matrix) {
      Zeros(row);
    }
  }

  void Print(std::vector<std::vector<float>>& matrix)
  {
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();

    std::cout << "Matrix: " << rows << " x " << cols << std::endl;

    std::cout << "{ ";
    for (size_t row = 0; row < rows; row++) {
      std::cout << "[" << row << "]\t" << "{ ";

      for (size_t col = 0; col < cols; col++) {
        std::cout << matrix[row][col];

        if (col != cols - 1) std::cout << ", ";
      }

      std::cout << " }" << std::endl;
    }

    std::cout << " }" << std::endl;
  }

  void Print(std::vector<float>& vector)
  {
    size_t cols = vector.size();

    std::cout << "Vector: " << cols << " x 1" << std::endl;

    std::cout << "{ ";
    for (size_t col = 0; col < cols; col++) {
      std::cout << std::fixed << std::setprecision(4) << vector[col];

      if (col != cols - 1) std::cout << ", ";
    }

    std::cout << " }" << std::endl;
  }

}
