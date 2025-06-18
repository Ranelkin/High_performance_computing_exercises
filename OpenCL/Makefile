# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -march=native -mtune=native -mavx -mfma -fopenmp -funroll-loops -ffast-math -flto -fstrict-aliasing -fno-tree-vectorize
LDFLAGS = -L/usr/lib64 -lOpenCL -ltbb -fopenmp

# TBB include 
TBB_HOME = $(HOME)/TBB_Lib/tbb
TBB_INCLUDE = -I$(TBB_HOME)/include
TBB_LIB = -L$(TBB_HOME)/lib64

# GEGL include path
GEGL_INCLUDE = -I/usr/include/gegl-0.4

# Source and output
TARGET = fastmatmul
SRC = FastMatrixMultiplication.cpp

# Build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(TBB_INCLUDE) $(GEGL_INCLUDE) $(SRC) -o $(TARGET) $(LDFLAGS) $(TBB_LIB)

# Clean rule
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: clean