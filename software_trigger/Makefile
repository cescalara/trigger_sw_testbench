CC=gcc
CXX=g++
OPTIMIZE_FLAGS=-O3
DEBUG_FLAGS=-g
WARNING_FLAGS=-Wall
CFLAGS:=-std=c++11 $(OPTIMIZE_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS)

TESTS = \
	l1_trigger_software \
	l2_trigger_software \

all:    $(TESTS)

l1_trigger_software: l1_trigger_software.cpp
	$(CXX) $(CFLAGS) l1_trigger_software.cpp -o l1_trigger 

l2_trigger_software: l2_trigger_software.cpp
	$(CXX) $(CFLAGS) l2_trigger_software.cpp -o l2_trigger 

clean:
	rm -f *.o *.dat *.dSYM *~ $(TESTS)
