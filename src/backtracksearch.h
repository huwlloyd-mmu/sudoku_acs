#pragma once
#include "board.h"
#include "timer.h"
#include "sudokusolver.h"

class BacktrackSearch : public SudokuSolver
{
private:
	Timer solutionTimer;
	float solTime;
	Board solution;
	void StepSolution(const Board& board);
	bool solved;
	int stepCount;
	bool timedOut;
	float timeOut;
public:
BacktrackSearch() : solTime(0.0f), stepCount(0), timedOut(false) {}
	virtual bool Solve(const Board& puzzle, float maxTime);
	virtual float GetSolutionTime() { return solTime; }
	virtual const Board& GetSolution() { return solution; }
	int GetStepCount() { return stepCount; }
};
