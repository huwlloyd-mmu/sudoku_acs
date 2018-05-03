#include "sudokuant.h"
#include "sudokuantsystem.h"

void SudokuAnt::InitSolution(const Board &puzzle, int startCell )
{
	sol.Copy(puzzle);
	iCell = startCell;
	failCells = 0;
	if (roulette != nullptr)
	{
		delete[] roulette;
		delete[] rouletteVals;
	}
	roulette = new float[puzzle.GetNumUnits()];
	rouletteVals = new ValueSet[puzzle.GetNumUnits()];
}

void SudokuAnt::StepSolution()
{
	if (sol.GetCell(iCell).Empty())
	{
		failCells++;
	}
	else if ( !sol.GetCell(iCell).Fixed() )
	{
		// make a choice from the options
		ValueSet choice = ValueSet(sol.GetNumUnits(), 1);
		if (parent->random() > parent->Getq0())
		{
			// greedy selection
			ValueSet best;
			float maxPher = -1.0f;

			for (int i = 0; i < sol.GetNumUnits(); i++)
			{
				if (sol.GetCell(iCell).Contains(choice))
				{
					if (parent->Pher(iCell, i) > maxPher)
					{
						maxPher = parent->Pher(iCell, i);
						best = choice;
					}
				}
				choice <<= 1;
			}
			sol.SetCell(iCell, best);
			// do local pheromone update here
			parent->LocalPheromoneUpdate(iCell, best.Index());
		}
		else
		{
			// weighted selection
			float totPher = 0.0f;
			int numChoices = 0;
			for (int i = 0; i < sol.GetNumUnits(); i++)
			{
				if (sol.GetCell(iCell).Contains(choice))
				{
					roulette[numChoices] = totPher + parent->Pher(iCell, i);
					totPher = roulette[numChoices];
					rouletteVals[numChoices] = choice;
					++numChoices;
				}
				choice <<= 1;
			}
			float rouletteVal = totPher * parent->random();

			for (int i = 0; i < numChoices; i++)
			{
				if (roulette[i] > rouletteVal)
				{
					sol.SetCell(iCell, rouletteVals[i]);
					// do local pheromone update here
					parent->LocalPheromoneUpdate(iCell, rouletteVals[i].Index());
					break;
				}
			}
		}
	}
	++iCell;
	if (iCell == sol.CellCount()) // wrap around
		iCell = 0;
}

