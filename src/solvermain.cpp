#include "sudokuantsystem.h"
#include "sudokusolver.h"
#include "backtracksearch.h"
#include "board.h"
#include "arguments.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string ReadFile( string fileName )
{
	char *puzString;
	ifstream inFile;
	inFile.open(fileName);
	if ( inFile.is_open() )
	{
		int order, idum;
		inFile >> order;
		int numCells = order*order*order*order;
		inFile >> idum;
		puzString = new char[numCells+1];
		for (int i = 0; i < numCells; i++)
		{
			int val;
			inFile >> val;
			if (val == -1)
				puzString[i] = '.';
			else if (order == 3)
				puzString[i] = '1' + (val - 1);
			else if (order == 4)
				if (val < 11)
					puzString[i] = '0' + val - 1;
				else
					puzString[i] = 'a' + val - 11;
			else
				puzString[i] = 'a' + val - 1;
		}
		puzString[numCells] = 0;
		inFile.close();
		string retVal = string(puzString);
		delete [] puzString;
		return retVal;
	}
	else
	{
		cerr << "could not open file: " << fileName << endl;
		return string();
	}
}

int main( int argc, char *argv[] )
{
	// solve, then spit out 0 for success, 1 for fail, followed by time in seconds
	Arguments a( argc, argv );
	string puzzleString;
	if ( a.GetArg("blank", 0 ) && a.GetArg("order", 0 ))
	{
		int order = a.GetArg("order", 0 );
		if ( order != 0 )
			puzzleString = string(order*order*order*order,'.');
	}
	else 
	{
		// read in the puzzle from a one-line string
		puzzleString = a.GetArg(string("puzzle"),string());
		if ( puzzleString.length() == 0 )
		{
			// try from a file
			string fileName = a.GetArg(string("file"),string());
			puzzleString = ReadFile(fileName);
		}
		if ( puzzleString.length() == 0 )
		{
			cerr << "no puzzle specified" << endl;
			exit(0);
		}
	}
	Board board(puzzleString);

	int algorithm = a.GetArg("alg", 0);
	int timeOutSecs = a.GetArg("timeout", 10);
	int nAnts = a.GetArg("ants", 10);
	float q0 = a.GetArg("q0", 0.9f);
	float rho = a.GetArg("rho", 0.9f);
	float evap = a.GetArg("evap", 0.005f );
	bool blank = a.GetArg("blank", false );
	bool verbose = a.GetArg("verbose", 0);
	bool showInitial = a.GetArg("showinitial", 0);
	bool success;

	float solTime;
	Board solution;
	SudokuSolver *solver;
	
	if ( algorithm == 0 )
		solver = new SudokuAntSystem( nAnts, q0, rho, 1.0f/board.CellCount(), evap);
	else
		solver = new BacktrackSearch();

	
	if ( showInitial )
	{
		// print inital grid
		cout << "Initial constrained grid" << endl;
		cout << board.AsString(false,true) << endl;
	}
	
	success = solver->Solve(board, (float)timeOutSecs );
	solution = solver->GetSolution();
	solTime = solver->GetSolutionTime();

	// sanity chack the solution:
	if ( success && !board.CheckSolution(solution) )
	{
		cout << "solution not valid" << a.GetArg("file",string()) << " " << algorithm << endl;
		cout << "numfixedCells " << solution.FixedCellCount() << endl;

		string outString = solution.AsString(true );
		cout << outString << endl;

		success = false;
	}
	if ( !verbose )
		cout << !success << endl << solTime << endl;
	else
	{
		if ( !success )
		{
			cout << "failed in time " << solTime << endl;
		}
		else
		{
			cout << "Solution:" << endl;
			string outString = solution.AsString( true );
			cout << outString << endl;
			cout << "solved in " << solTime << endl;
		}
	}
}
