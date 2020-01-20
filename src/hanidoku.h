#ifndef HANIDOKU_H
#define HANIDOKU_H
#include "dlx.h"
// 9 rows, 9 up diagonals, 9 down diagonals
// 61 cells, 61*9=549 options
// constraints: each cell: 61, each line: 3*9*9 (some optional)
// 0: 01234----
// 1: 012345---
// 2: 0123456--
// 3: 01234567-
// 4: 012345678
// 5: -12345678
// 6: --2345678
// 7: ---345678
// 8: ----45678
// int rpos[9] = {0, 5, 11, 18, 26, 34, 41, 47, 52};
// pos = rpos[r] + c
// print:
//   .---------.
//  / 0 0 0 0 0 \
// { 0 0 0 0 0 0 }
//  \ 0 0 0 0 0 /
//   '---------'
class HanidokuGrid;
class HanidokuRowCell : public dlx::DlxCell {
   public:
    // int r_ = -1, c_ = -1, n_ = -1;  // row, column, number
    int n_ = -1;
    int pos_ = -1;
    HanidokuRowCell() {}
    virtual ~HanidokuRowCell() {}

    // friends:
    friend class HanidokuGrid;
};
class HanidokuGrid : public dlx::DlxGrid {
   private:
    dlx::DlxCell cell_[4000];
    dlx::DlxCell col_[304];
    void PrintSolution(int count);

   public:
    HanidokuRowCell row_[549];
    HanidokuGrid();
    virtual ~HanidokuGrid() {}
    void PrintGrid();
    char str_[62];
    bool benchmark_ = 0;
    bool verbose_ = 0;
    int output_format_ = 0;
    char __padding__[4] = "4";
};
#endif /* ifndef HANIDOKU_H */
