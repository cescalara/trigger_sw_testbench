CC=gcc
CXX=g++
OPTIMIZE_FLAGS=-O3
WARNING_FLAGS=-Wall
CFLAGS:=-std=c++11 $(OPTIMIZE_FLAGS) $(WARNING_FLAGS) 

TESTS = \
	l1_trigger_software \
	l2_trigger_software \

all:    $(TESTS)

l1_trigger_software: l1_trigger_software.cpp
	$(CXX) $(CFLAGS) l1_trigger_software.cpp -o l1_trigger 
	mkdir -p ../bin
	mv l1_trigger ../bin

l2_trigger_software: l2_trigger_software.cpp
	$(CXX) $(CFLAGS) l2_trigger_software.cpp -o l2_trigger 
	mv l2_trigger ../bin

clean:
	rm -r -f *.o *.dat *.dSYM *~ $(TESTS)
