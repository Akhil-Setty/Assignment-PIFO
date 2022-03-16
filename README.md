# Assignment-PIFO
Assignment 2 - PIFO implementation

Requirements: g++/gcc compiler that supports C++ 11 

How to compile?
g++ -g main.cpp lstf.hpp pifo.hpp pifo.cpp (-g is for debugging and can be skipped)

What does each file mean?
main.cpp contains the scheduling algorithm implementation i.e. the LSTF implementation, packet generators (individual hosts) and the PIFO pipeline of enqueu/dequeue operations
pifo.cpp/pifo.hpp contain the implementation of the linked-list based PFIO datastructure
lstf.hpp contains the configurable variables (slack time variables defined in the report: BASE_SLACK_TIME, SLACK_TIME_RANGE & the number of packets and flows)

