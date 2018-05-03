#include "board.h"
#include <iostream>
#include <iomanip>
#include <inttypes.h>
#include <string>
#include <sstream>
//
// description of a sudoku board, with functions for setting cells and propagating constraints
//
Board::Board(const string &puzzleString)
{
	// check this is an order 3, 4, or 5 puzzle
	switch (puzzleString.length())
	{
	case 81:
		order = 3;
		break;
	case 256:
		order = 4;
		break;
	case 625:
		order = 5;
		break;
	case 1296:
		order = 6;
		break;
	case 2401:
		order = 7;
		break;
	case 4096:
		order = 8;
		break;
	default:
		std::cerr << "wrong number of cells for a sudoku board!" << std::endl;
		order = 0;
		break;
	}

	numUnits = order * order;
	numCells = numUnits * numUnits;
	
	cells = new ValueSet[numCells];

	int maxVal = numUnits;

	for (int i = 0; i < numCells; i++)
		cells[i].Init(maxVal);

	// set all possibilities for all cells
	for ( int i = 0; i < numCells; i++ )
	{
		cells[i] = ~cells[i];
	}
	// set the known cells one by one
	numInfeasible = 0;
	numFixedCells = 0;
	for (int i = 0; i < numCells; i++)
	{
		if (puzzleString[i] != '.')
		{
			int value;
			switch (order)
			{
			case 3:
				value = (int)(puzzleString[i] - '0');
				break;
			case 4:
				if (puzzleString[i] >= '0' && puzzleString[i] <= '9')
					value = 1+(int)(puzzleString[i] - '0');
				else
					value = 11 + (int)(puzzleString[i] - 'a');
				break;
			case 5:
			default:
				value = 1+(int)(puzzleString[i] - 'a');
			}
			SetCell( i, ValueSet(maxVal, (int64_t)1 << (value-1) ));
		}
	}
}

Board::Board(const Board &other)
{
	Copy(other);
}

void Board::Copy(const Board& other)
{
	order = other.order;
	numUnits = order * order;
	numCells = numUnits * numUnits;

	if (cells == nullptr)
		cells = new ValueSet[numCells];

	int maxVal = numUnits;

	for (int i = 0; i < numCells; i++)
		cells[i] = other.GetCell(i);

	numFixedCells = other.FixedCellCount();
	numInfeasible = other.InfeasibleCellCount();
}

Board::~Board()
{
	if ( cells != nullptr ) delete [] cells;
}

int Board::RowCell(int iRow, int iCell) const
{
	// returns cell index of the iCell'th cell in the iRow'th row
	return iRow * numUnits + iCell;
}

int Board::ColCell(int iCol, int iCell) const
{
	// returns cell index of the iCell'th cell in the iCol'th column
	return iCell * numUnits + iCol;
}

int Board::BoxCell(int iBox, int iCell) const
{
	// returns cell index of the iCell'th cell in the iBox'th box
	int boxCol = iBox%order;
	int boxRow = iBox / order;
	int topCorner =  (boxCol*order) + boxRow*order*order*order;
	return topCorner + (iCell%order) + (iCell / order)*order*order;
}

int Board::RowForCell(int iCell) const
{
	// returns index of the row which contains cell iCell 
	return iCell / numUnits;
}

int Board::ColForCell(int iCell) const
{
	// returns index of the column which contains cell iCell 
	return iCell % numUnits;
}

int Board::BoxForCell(int iCell) const
{
	// returns index of the box which contains cell iCell 
	return order*(iCell / (order*order*order)) + ((iCell%(order*order))/order);
}

string Board::AsString(bool useNumbers, bool showUnfixed )
{
	/*
	  Form a human-readable string from the board, using either numbers (1..numUnits) or a single character
	  per cell (9x9 - 1-9, 16x16 0-f, 25x25 - a-y). If showUnfixed is true, show the possibilies in
	  an unfixed cell (otherwise represent it is as '.'). If order is 4 or 5 and showUnfixed is true, force useNumbers to 
	  false for readability.
	 */

	if ( showUnfixed )
		useNumbers = false;

	stringstream puzString;
	string alphabet;

	if ( !useNumbers )
	{
		if ( order == 3 )
			alphabet = string("123456789");
		else if (order == 4 )
			alphabet = string("0123456789abcdef");
		else
			alphabet = string("abcdefghijklmnopqrstuvwxy");
	}

	vector<string> cellStrings;
	unsigned int maxLen = 0;
	for (int i = 0; i < numCells; i++)
	{
		string cellContents;
		if ( !useNumbers )
		{
			if ( !showUnfixed && !cells[i].Fixed())
				cellContents = string(".");
			else
				cellContents = cells[i].toString(alphabet);
		}
		else 
			cellContents = to_string(cells[i].Index() + 1);
		if (cellContents.size() > maxLen )
			maxLen = cellContents.size();
		cellStrings.push_back(cellContents);
	}
	int pitch = maxLen+1;
	for ( int i = 0; i < numCells; i++ )
	{
		puzString << setw(pitch) << cellStrings[i] << " ";
		if ( i%numUnits == numUnits -1 )
		{
			if ( i != numCells-1 )
				puzString << endl;
		}
		else if (i%order == order-1 )
			puzString << string("|");
		if ( i%(numUnits*order) == numUnits*order-1 && i != numCells-1)
		{
			for ( int j = 0; j < order; j++ )
			{
				for ( int k = 0; k < order*(pitch+1); k++ )
					puzString << '-';
				if ( j != order-1 )
					puzString << '+';
			}
			puzString << endl;
		}
	}
	return puzString.str();
}

void Board::ConstrainCell(int i )
{
	if ( cells[i].Empty() || cells[i].Fixed()  )
		return; // already set or empty
	
	int iBox, iCol, iRow;
	iBox = BoxForCell(i);
	iCol = ColForCell(i);
	iRow = RowForCell(i);

	// set of all fixed cells in row, column, box
	ValueSet colFixed(numUnits), rowFixed(numUnits), boxFixed(numUnits);
    // set of all open values in row, column, box, not including this cell
	ValueSet colAll(numUnits), rowAll(numUnits), boxAll(numUnits);

	for (int j = 0; j < numUnits; j++)
	{
		int k;
		k = BoxCell(iBox, j);
		if (k != i)
		{
			if ( cells[k].Fixed() )
				boxFixed += cells[k];
			boxAll += cells[k];
		}
		k = ColCell(iCol, j);
		if (k != i)
		{
			if ( cells[k].Fixed() )
				colFixed += cells[k];
			colAll += cells[k];
		}
		k = RowCell(iRow, j);
		if (k != i)
		{
			if ( cells[k].Fixed() )
				rowFixed += cells[k];
			rowAll += cells[k];
		}
	}
	ValueSet fixedCellsConstraint = ~(rowFixed + colFixed + boxFixed);

	if ( fixedCellsConstraint.Fixed() ) 
		SetCell( i, fixedCellsConstraint ); // only one possibility left
	else
	{
		// eliminate all the values already taken in this cell's units
		cells[i] ^= fixedCellsConstraint;
		// are any of the remaining values for this cell in the only possible
		// place in a unit? If so, set the cell to that value
		if ((cells[i]-rowAll).Fixed())
			SetCell(i, cells[i]-rowAll);
		else if ((cells[i]-colAll).Fixed())
			SetCell(i, cells[i]-colAll);
		else if ((cells[i]-boxAll).Fixed())
			SetCell(i, cells[i]-boxAll);
	}
	if ( cells[i].Empty())
		numInfeasible++;
}

void Board::SetCell(int i, const ValueSet &c )
{
	if ( cells[i].Fixed())
		return; // already set
	// set the cell
	cells[i] = c;
	++numFixedCells;
	// propagate the constraints
  	int iBox, iCol, iRow;
	iBox = BoxForCell(i);
	iCol = ColForCell(i);
	iRow = RowForCell(i);

	for (int j = 0; j < numUnits; j++)
	{
		int k;
		k = BoxCell(iBox, j);
		if (k != i)
			ConstrainCell(k);
		k = ColCell(iCol, j);
		if (k != i)
		    ConstrainCell(k);
		k = RowCell(iRow, j);
		if (k != i)
			ConstrainCell(k);
	}
}

const ValueSet& Board::GetCell(int i) const
{
	return cells[i];
}

int Board::FixedCellCount(void) const
{
	return numFixedCells;
}

int Board::InfeasibleCellCount(void) const
{
	return numInfeasible;
}

int Board::CellCount(void) const
{
	return numCells;
}

int Board::GetNumUnits(void) const
{
	return numUnits;
}

bool Board::CheckSolution(const Board& other) const
{
	// check that other is 1/ a solution, 2/ consistent with this board
	if (other.CellCount() != CellCount())
		return false;
	bool isSolution = true;
	bool isConsistent = true;

	// check its a solution
	// first - all cells must be filled
	for (int i = 0; i < other.CellCount(); i++)
	{
		if (!other.GetCell(i).Fixed())
			isSolution = false;
	}
	// second - all rows, columns, boxes must have one of each number
	for (int i = 0; i < numUnits; i++)
	{
		ValueSet row, col, box;
		row.Init(numUnits);
		col.Init(numUnits);
		box.Init(numUnits);
		for (int j = 0; j < numUnits; j++)
		{
			row += other.GetCell(RowCell(i, j));
			col += other.GetCell(ColCell(i, j));
			box += other.GetCell(BoxCell(i, j));
		}
		if (row.Count() != numUnits || col.Count() != numUnits || box.Count() != numUnits )
			isSolution = false;
	}

	// check consistency with this board
	for (int i = 0; i < CellCount(); i++)
	{
		if (GetCell(i).Fixed())
		{
			if (GetCell(i).Index() != other.GetCell(i).Index() )
				isConsistent = false;
		}
	}

	return isSolution && isConsistent;
}

