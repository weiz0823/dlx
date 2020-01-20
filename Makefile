CXX=g++
CXXFLAGS=-std=c++11 -O2

all install: sudoku hanidoku

sudoku: src/dlx.cc src/sudoku.cc
	$(CXX) $(CXXFLAGS) src/dlx.cc src/sudoku.cc \
		-o exe/sudoku

hanidoku: src/dlx.cc src/hanidoku.cc
	$(CXX) $(CXXFLAGS) src/dlx.cc src/hanidoku.cc \
		-o exe/hanidoku

hanidoku_old: src/dlx.cc src/hanidoku_old.cc
	$(CXX) $(CXXFLAGS) src/dlx.cc src/hanidoku_old.cc \
		-o exe/hanidoku_old
