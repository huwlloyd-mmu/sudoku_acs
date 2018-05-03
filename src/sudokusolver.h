#pragma once
#include "board.h"

// pure virtual interface shared between backtrack search and sudoku ant system
class SudokuSolver
{
public:
	virtual bool Solve(const Board& puzzle, float maxTime) = 0;
	virtual float GetSolutionTime() = 0;
	virtual const Board& GetSolution() = 0;
};
