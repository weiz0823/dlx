#ifndef DLX_H
#define DLX_H
#include <algorithm>
#include <iostream>
namespace dlx {
class DlxGrid;
class DlxCell {
   private:
    // up, down, left, right
    DlxCell *u_ = this, *d_ = this, *l_ = this, *r_ = this;
    DlxCell *row_ = this, *col_ = this;
    int size_ = 0;
    bool covered_ = 0;

   protected:
    virtual bool ColComp(const DlxCell& rhs) const { return size_ < rhs.size_; }
    virtual bool StopCompare() { return size_ <= 1; }
    virtual bool RowComp(const DlxCell& rhs) const { return size_ > rhs.size_; }

   public:
    DlxCell() {}
    virtual ~DlxCell() {}
    DlxCell* Row() { return row_; }
    char __padding__[7] = "7";
    // column type: 1) compulsory, 0) optional
    int type_ = 1;

    // friends:
    friend class DlxGrid;
};

// a LIFO stack, but able to iterate through
// implemented by array, because we can predict the max size
class DlxStack {
   public:
    DlxCell** data_ = nullptr;
    int size_ = 0;
    void Construct(int capacity) { data_ = new DlxCell*[capacity]; }

    DlxStack(int capacity = 0) {
        if (capacity > 0) Construct(capacity);
    }
    virtual ~DlxStack() {
        if (data_ != nullptr) delete[] data_;
    }
    void Push(DlxCell* cell) { data_[size_++] = cell; }
    DlxCell* Pop() { return data_[--size_]; }
    DlxCell* Top() {
        if (size_)
            return data_[size_ - 1];
        else
            return nullptr;
    }
    bool bEmpty() { return size_ == 0; }
    void Clear() { size_ = 0; }
    char __padding__[4] = "4";
};

class DlxGrid {
   private:
    void Cover(DlxCell* col);
    void Uncover(DlxCell* col);
    void Choose(DlxCell* cell);
    DlxCell* Unchoose();

   protected:
    DlxCell head_;
    virtual void PrintSolution(int count) {
        printf("Found solution #%d\n", count);
    }

   public:
    DlxStack reserve_stack_;
    DlxStack choose_stack_;
    DlxStack cover_stack_;
    DlxGrid() {}
    virtual ~DlxGrid() {}
    void InitStacks(int row_capacity = -1, int col_capacity = -1);
    //@dir: u=up, r=down, l=left, r=right, R=row, C=column
    // insert cell to the direction of pos
    void Insert(DlxCell* cell, DlxCell* pos, char dir);
    int Reserve(DlxCell* row);
    DlxCell* Unreserve();
    void UnreserveAll() {
        while (!reserve_stack_.bEmpty()) Unreserve();
    }
    void UnchooseAll() {
        while (!choose_stack_.bEmpty()) Unchoose();
        choose_count_ = 0;
    }
    // return number of solutions found
    int Search(int max_count = -1);
    DlxCell* GetRandRow() {
        int row_size = 0;
        DlxCell* i;
        for (i = head_.d_; i != &head_; i = i->d_)
            if (!i->covered_) ++row_size;
        int count = std::rand() % row_size;
        i = head_.d_;
        while (i->covered_) i = i->d_;
        for (int t = 0; t < count; ++t) do {
                i = i->d_;
            } while (i->covered_);
        return i;
    }
    int choose_count_ = 0;
    bool sort_rows_ = 0;
    char __padding__[3] = "3";
};
}  // namespace dlx
#endif /* ifndef DLX_H */
