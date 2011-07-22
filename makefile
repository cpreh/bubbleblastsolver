main : main.o
	g++ -lfcppt -o main main.o

main.o : main.cpp
	g++ -O3 -march=corei7-avx -c main.cpp
