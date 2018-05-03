#pragma once
#include "board.h"

class SudokuAntSystem;

class SudokuAnt
{
	Board sol;	// current working solution
	int iCell;	// current cell
	SudokuAntSystem *parent;	// parent ant system
	int failCells;	// no of cells on this attempt which were unsettable
	float *roulette; // working array for the roulette wheel selection
	ValueSet *rouletteVals; // working array for the roulette wheel selection

public:	
	SudokuAnt(SudokuAntSystem *parent) : parent(parent), iCell(0), roulette(nullptr), rouletteVals(nullptr) {}
	void InitSolution(const Board &puzzle, int ic);
	void StepSolution();
	const Board& GetSolution() { return sol; }
	int NumCellsFilled() { return sol.CellCount() - failCells; }
};
