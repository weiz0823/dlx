# Dancing Links (DLX)

An implementation of [dancing links algorithm](https://en.wikipedia.org/wiki/Dancing_Links)
(DLX) in C++, and its application on [Sudoku](https://en.wikipedia.org/wiki/Sudoku)
solving & generating and [Hanidoku](http://www.sudocue.net/hanidoku_home.php)
solving & generating.

## Features

- Dancing links with friendly API.
- Sudoku as a basic exact cover example.
- Hanidoku as an example to show more complicated cover problem that DLX can solve.
- Quick generation of Sudoku and Hanidoku puzzles that are fun to play with.

## Benchmark result

- Sudoku solving costs 0.28ms on average, testing on 1465 puzzles in puzzle set [data/sudoku/top1465.txt].
An average of 0.07ms is taken for solving each of the 49151 puzzles in [data/sudoku/sudoku17\_convert.txt].

- For Sudoku generating, no more than 1s will be take if the limit of clues is higher than 22.
Limit of 22 may take about 1s to 2s. Limit of 21 may take several seconds.
Limit of 20 may take much much longer. Strict limits are not tested.

- Hanidoku solving costs 16ms on average, testing on 172 puzzles in puzzle set [data/hanidoku/puzzles.txt].

- For Hanidoku generating, it may often several seconds, but will often give you 5--10 clues,
because the generating function requires that we can't erase any clue in the puzzle to get another
puzzle with exactly one solution. The strictest limit tested is 5, and it takes about 5 seconds to generate one puzzle.

## Usage of Sudoku & Hanidoku program

There are 3 commands for both program: solve, generate, convert.
Only the first letter is needed.
Type `./program [command] -h` to see options.
Input and output are standard I/O by default, but you can redirect.

Note that only one-line format is supported for input, and irrelevant lines may
cause error, so it's _highly recommended_ that you choose non-verbose one-line
format for output when doing batch generating, then using convert command to read.

## Compiling Sudoku & Hanidoku

`make`, `make all`, or `make install` for making both programs,
`make sudoku` or `make hanidoku` for making single one.
Binary executable stored in `exe/`.

## Usage of DLX API

### Class `DlxCell`

Class for basic cells in DLX. May be used for row, column, or in-grid cells.
_May be inherited_ to include specific information. It's advised to inherit row
cells, for they can represent candidates.

#### Virtual functions that can be redefined

- `bool ColComp(const DlxCell& rhs) const;`

  Function for column heuristic, to choose the best column.
  Default: choose the column with smallest size.

- `bool StopCompare();`

  Function that can stop searching for a better column under certain circumstances.
  Default: when the size of the best column is no more than 1.

- `bool RowComp(const DlxCell& rhs) const;`

  Only used when you choose to sort the rows. To sort the rows in the order defined by this function.
  Default: bigger row is the better one.

#### Public methods and properties

- `DlxCell(); virtual ~DlxCell(); char __padding__[7] = "7";`

  Trivial construction and destruction. Padding to silence the warning.

- `DlxCell* Row();`

  Return the pointer to row cell.

- `int type_ = 1;`

  Column type, 1 for compulsory (must be covered), 0 for optional (at most one cover).
  For dangling cells (will be introduced below), the value is automatically set to
  -1, don't modify.

### Class `DlxStack`

Class for storing stack when searching. Better NOT to be inherited.
Nothing needs to be handled except when getting the solution found.
`DlxCell** data_;` stores an array of pointers pointing to DLX cells, with length of
`size_`.

### Class `DlxGrid`

Methods and properties for the full grid.

#### Virtual functions that can be redefined

- `void PrintSolution(int count);`

  Function to print the solution of number `count` (starting from 0).
  As long as all compulsory columns have been covered, it's regarded a solution.
  Default: claim that the solution number `count` has been found.

#### Public methods and properties

- `DlxGrid(); virtual ~DlxGrid(); char __padding__[3] = "3";`

  Trivial construction and destruction. Padding to silence the warning.

- `DlxStack reserve_stack_, choose_stack_, cover_stack_;`

  `reserve_stack_` stores the preset, `choose_stack_` stores the _cells_ (not rows)
  that has been chosen, which may be used for printing solution. `cover_stack_`
  stores the column that has been covered.

- `void InitStacks(int row_capacity = -1, int col_capacity = -1);`

  Initialize the stacks, when the array will be constructed.
  MUST be called before doing the reservation.
  `reserve_stack_` and `choose_stack_` will be initialized as `row_capacity` if specified,
  otherwise will be the count of rows.
  `cover_stack_` will be initialized as `col_capacity` if specified, otherwise will be
  _four times_ the count of columns.

- `void Insert(DlxCell* cell, DlxCell* pos, char dir);`

`dir`: u=up, r=down, l=left, r=right, R=row, C=column, X=dangling

Mostly you will use R/C/X. Note that one cell should be handled twice to properly set its horizontal and vertical position.
X is for dangling cells in terms of vertical position, which means that choosing the row
in which the cell is will cover the column it points to, but covering the column will not cover this row.
You can see its usage in Hanidoku solving.

- `int Reserve(DlxCell* row);`

Reserve a row (set it as preset which can not be changed).

- `DlxCell* Unreserve();`

Pop the last preset and return it. Return `nullptr` if the reserve stack is empty.

- `void UnreserveAll();`

Pop everything from the reserve stack.
Used for setting next set of presets.

- `void UnchooseAll();`

Pop everything from the choose stack.
Used for clearing the stack for the next search.

- `int Search(int max_count = -1);`

Main searching function. You can set the maximum count of solutions to search for.
$-1$ means no limit.

- `DlxCell* GetRandRow();`

Randomly ge ta row that has not been covered. Can be used for generating random presets.

- `int choose_count_ = 0;`

  The counter of the choices made.

- `bool sort_rows_ = 0;`

  Sort the rows according to `bool RowComp(const DlxCell& rhs) const;` or don't sort.
  Note: sorting costs time, may not be really efficient.

- `DlxCell head_;`

Protected. Row cells should be added to the column of `head_`, column cells should be added to the row of `head_`.

### General procedure of solving the problem

1. Create a grid and enough cells.

1. To initialize the full grid, firstly add column cells and row cells to `head_`,
   then add grid cells to columns and rows.

1. Call `InitStack(row_capacity, col_capacity)` to manually or automatically set
   the size of stacks, to construct the array in stack.

1. Call `Reserve(row_cell)` to set the presets.

1. Call `Search(count)` to start solving the problem. The solution will be printed
   (or you can also store it) whenever a solution has been found.

1. To initialize everything for the next problem, _first_ call `UnchooseAll()`,
   _then_ call `UnreserveAll()`, and you can get the full grid back.

## How is Hanidoku different from Sudoku

I made the Sudoku grid with four kinds of constraints (columns): every blank should have a number,
every row / column / $3 \times 3$ block should have number 1 to 9. That is 324 dlx columns in total.
So each column is compulsory, and under that constriant we can get exactly the legal solutions of sudoku.

However, Hanidoku doesn't require every row / incline / decline has exactly 1 to 9.
It requires a set of _consecutive_ numbers in 1--9.
I designed the constraints as: every blank has a number, every row / incline / decline has 1 to 9.
That is 304 dlx columns in total. But firstly, not every column is compulsory.
For example, only 4, 5, 6 are compulsory in the second row, which has a length of 6.
But this is not enough to express the constraint that numbers in a line should be consecutive.
So we need many dangling cells. For example, if we fill 9 in the fist row, then 1--4 can't be chosen in the first row.
But we can't make 9 in any column, unless we make many columns, each for one pair.
That's because if we choose 4, it means not to choose 9, but doesn't mean not to choose 1, 2, 3.

## References

- [The original DLX paper on arxiv by D. Knuth](https://arxiv.org/abs/cs/0011047)
