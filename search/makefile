G=-std=c++11 -lboost_system -lboost_filesystem -I /usr/include -ljsoncpp

.PHONY:all 
all:main libsearch.so

main:main.cc search.h search.cc 
	g++ $^ -o $@ ${FLAG}

libsearch.so:search.h search.cc 
	g++ search.cc -shared -fPIC -o libsearch.so ${FLAG}
