/**
  Hello, world!.
 */


#include <iostream>
#include <omp.h>

int main() 
{
  
  #pragma omp parallel num_threads(10)
  #pragma omp critical 
  {int id = omp_get_thread_num();
    
  std::cout << " Hello, world! " << id << std::endl;
  }
  return 0;
}
