CC=g++
CFLAGS=-c -O3 -std=c++0x

sudokusolver : board.o sudokuant.o sudokuantsystem.o backtracksearch.o solvermain.o 	
	$(CC) -o sudokusolver obj/board.o obj/sudokuant.o obj/sudokuantsystem.o obj/backtracksearch.o obj/solvermain.o
board.o: src/board.cpp
	$(CC) $(CFLAGS) src/board.cpp -o obj/board.o
sudokuant.o: src/sudokuant.cpp
	$(CC) $(CFLAGS) src/sudokuant.cpp -o obj/sudokuant.o
sudokuantsystem.o: src/sudokuantsystem.cpp
	$(CC) $(CFLAGS) src/sudokuantsystem.cpp -o obj/sudokuantsystem.o
backtracksearch.o: src/backtracksearch.cpp
	$(CC) $(CFLAGS) src/backtracksearch.cpp -o obj/backtracksearch.o
solvermain.o: src/solvermain.cpp
	$(CC) $(CFLAGS) src/solvermain.cpp -o obj/solvermain.o
clean :
	rm sudokusolver obj/*.o
