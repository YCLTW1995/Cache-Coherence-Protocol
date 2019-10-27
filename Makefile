ID := 103062333

C                       := gcc
CXX                     := g++

MPICC                   := mpicc
MPICXX                  := mpic++

CCFLAGS                 := -O3 -march=native -Wall -std=c99
CXXFLAGS                := -O3 -march=native -Wall -std=c++0x 
LDFLAGS                 := -lX11 -fopenmp -lm

all : MSI MESI

clean:
	rm -f MSI
	rm -f MESI
MSI: MSI.cpp
	$(CXX) -o $@ $? $(CXXFLAGS) $(CXXFLAGS) 
MESI: MESI.cpp
	$(CXX) -o $@ $? $(CXXFLAGS) $(CXXFLAGS)

