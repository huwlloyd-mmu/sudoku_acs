#include "sudokuantsystem.h"
#include <iostream>

void SudokuAntSystem::InitPheromone(int nNumCells, int valuesPerCell )
{
	numCells = nNumCells;
	pher = new float*[numCells];
	for (int i = 0; i < numCells; i++)
	{
		pher[i] = new float[valuesPerCell];
		for (int j = 0; j < valuesPerCell; j++)
			pher[i][j] = pher0;
	}
}

void SudokuAntSystem::ClearPheromone()
{
	for (int i = 0; i < numCells; i++)
		delete[] pher[i];
	delete[] pher;
}

float SudokuAntSystem::PherAdd( int cellsFilled)
{
	return numCells / (float)(numCells - cellsFilled);
}

void SudokuAntSystem::UpdatePheromone()
{
	for (int i = 0; i < numCells; i++)
	{
		if (bestSol.GetCell(i).Fixed())
		{
			pher[i][bestSol.GetCell(i).Index()] = pher[i][bestSol.GetCell(i).Index()] * (1.0f - rho) + rho*bestPher;
		}
	}
}

void SudokuAntSystem::LocalPheromoneUpdate(int iCell, int iChoice)
{
	pher[iCell][iChoice] = pher[iCell][iChoice] * 0.9f + pher0*0.1f;
}

bool SudokuAntSystem::Solve(const Board& puzzle, float maxTime )
{
	solutionTimer.Reset();
	int iter = 0;
	bool solved = false;
	bestPher = 0.0f;
	int curBestAnt = 0;
	InitPheromone( puzzle.CellCount(), puzzle.GetNumUnits() );
	while (!solved)
	{
		// start each ant on a different square
		std::uniform_int_distribution<int> dist(0, puzzle.CellCount()-1);
		for (auto a : antList)
		{
			a->InitSolution(puzzle, dist(randGen));
		}
		// fill cells one at a time
		for (int i = 0; i < puzzle.CellCount(); i++)
		{
			// step each ant in turn
			for (auto a : antList)
			{
				a->StepSolution();
			}
		}
		// update pheromone
		int iBest = 0;
		int bestVal = 0;
		for (unsigned int i = 0; i < antList.size(); i++)
		{
			if (antList[i]->NumCellsFilled() > bestVal)
			{
				bestVal = antList[i]->NumCellsFilled();
				iBest = i;
			}
		}
		float pherToAdd = PherAdd(bestVal);

		if (pherToAdd > bestPher)
		{
			// new best
			bestSol.Copy(antList[iBest]->GetSolution());
			bestPher = pherToAdd;
			curBestAnt = bestVal;
			if (bestVal == numCells)
			{
				solved = true;
				solTime = solutionTimer.Elapsed();
			}
		}
		UpdatePheromone();
		bestPher *= (1.0f - bestEvap);
		++iter;
		// check timer every 100 iterations
		if ((iter % 100) == 0)
		{
			float elapsed = solutionTimer.Elapsed();
			if ( elapsed > maxTime)
			{
				break;
			}
		}
	}
	ClearPheromone();
	return solved;
	
}

