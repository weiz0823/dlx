#ifndef SUDOKU_H
#define SUDOKU_H
#include "dlx.h"
const int N = 3;
const int N2 = N * N;
const int N4 = N2 * N2;
const int N6 = N4 * N2;
class SudokuGrid;
class SudokuRowCell : public dlx::DlxCell {
   public:
    int r_ = -1, c_ = -1, n_ = -1;  // row, column, number
    int __padding__ = 4;
    SudokuRowCell() {}
    virtual ~SudokuRowCell() {}

    // friends:
    friend class SudokuGrid;
};
class SudokuGrid : public dlx::DlxGrid {
   private:
    SudokuRowCell row_[N6];
    dlx::DlxCell cell_[4 * N6];
    dlx::DlxCell col_[4 * N4];
    void PrintSolution(int count);

   public:
    SudokuGrid();
    virtual ~SudokuGrid() {}
    SudokuRowCell* GetRow(int r, int c, int n) {
        return row_ + r * N4 + c * N2 + n;
    }
    void PrintGrid();
    char str_[N4 + 1];
    bool benchmark_ = 0;
    bool verbose_ = 0;
    int output_format_ = 0;
};
#endif /* ifndef SUDOKU_H */
