# sudoku_release
Release candidate for the sudoku ant system

## Build

make -f Makefile

The makefile uses g++ to compile. However, any C++ compiler should work, as long as it supports the C++11 standard.

Alternatively, for windows, there is a Visual Studio 2017 project file in the __vs2017__ folder.

## Command-line arguments

__--alg n__ n=0 (default) use Ant Colony System. n=1 use backtracking search

__--file filename__ open puzzle instance in filename

__--puzzle puzzle_string__ read puzzle in string format - use '.' for blank cells, 1-9 for 9x9, 0-f for 16x16, a-y for 25x25.


__--blank__ start with a blank grid, need to set order

__--order__ set the order for a blank grid (3, 4 or 5 for 9x9, 16x16, 25x25)

__--verbose__ print the solution after solving. If not set, the code outputs 0 (success) or 1 (fail) followed by the elapsed time

__--showinitial__ print the initial (constrained) grid. The grid is constructed by setting each given cell in turn, and propagating the constraints. In some cases this is sufficient to solve the puzzle, so the initial constrained grid will be the solution.

__--timeout secs__ set the timeout in seconds (default 10 seconds)

__--nAnts n__ set number of ants, default 10

__--q0 value__ set ACS q0 parameter (which controls greediness) - default 0.9

__--rho value__ set ACS rho parameter (which controls pheromone evaporation) - default 0.9

__--evap f__ use value f for the best-value evaporation parameter. Default is 0.005

