//
// backtracking search using the same comstraint propagation code as the ant colony system
// HL 18/9/2017
//
#include "backtracksearch.h"

void BacktrackSearch::StepSolution(const Board &puzzle)
{
	// deal with timeout
	if (timedOut)
		return;
	stepCount++;
	if ( stepCount%5000 == 0 )
	{
		if ( solutionTimer.Elapsed() > timeOut )
		{
			timedOut = true;
			return;
		}
	}
	// find the cell with the least number of possibilities (minimum remaining values heuristic)
	int nextCell = -1;
	int minCount = puzzle.GetNumUnits()+1;

	for (int iCell = 0; iCell < puzzle.CellCount(); iCell++)
	{
		if (!puzzle.GetCell(iCell).Fixed() && puzzle.GetCell(iCell).Count() < minCount)
		{
			nextCell = iCell;
			minCount = puzzle.GetCell(iCell).Count();
			if (minCount == 2)
				break;
		}
	}
	if (nextCell == -1) // should only be here if the puzzle is solved before we try anything
	{
		solved = true;
		solution.Copy(puzzle);
		return;
	}

	// try the possibilities in turn
	ValueSet choice = ValueSet(puzzle.GetNumUnits(), 1);
	for (int i = 0; i < puzzle.GetNumUnits(); i++)
	{
		if (solved)
			return;
		if ( puzzle.GetCell(nextCell).Contains(choice))
		{
			// copy the board
			Board newBoard;
			newBoard.Copy(puzzle);
			// set the cell
			newBoard.SetCell(nextCell, choice);
			// did we solve the puzzle?
			if (newBoard.FixedCellCount() == newBoard.CellCount())
			{
				// solved
				solved = true;
				solution.Copy(newBoard);
				return;
			}
			// check no conflicts
			if (newBoard.InfeasibleCellCount() == 0)
			{
				// carry on and set the next cell
				StepSolution(newBoard);
			}
		}
		choice <<= 1;
	}
}

bool BacktrackSearch::Solve(const Board& puzzle, float maxTime)
{
	solved = false;
	timedOut = false;
	timeOut = maxTime;
	solutionTimer.Reset();
	StepSolution(puzzle);
	solTime = solutionTimer.Elapsed();
	return solved;
}
