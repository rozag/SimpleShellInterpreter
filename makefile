Shell.o: Constants.h Shell.h Shell.cpp
	g++ -c Shell.cpp -o Shell.o -std=c++11 -Wfatal-errors
shell: main.cpp Shell.o
	g++ main.cpp Shell.o -o shell -std=c++11 -Wfatal-errors