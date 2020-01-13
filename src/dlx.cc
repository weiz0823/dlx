#include "dlx.h"
namespace dlx {
void DlxGrid::InitStacks(int row_capacity, int col_capacity) {
    int row_size = 0;
    for (DlxCell* i = head_.d_; i != &head_; i = i->d_) ++row_size;
    if (row_capacity > 0) {
        reserve_stack_.Construct(row_capacity);
        choose_stack_.Construct(row_capacity);
    } else {
        reserve_stack_.Construct(row_size);
        choose_stack_.Construct(row_size);
    }
    int col_size = 0;
    for (DlxCell* i = head_.r_; i != &head_; i = i->r_) ++col_size;
    if (col_capacity > 0)
        cover_stack_.Construct(col_capacity);
    else
        cover_stack_.Construct(col_size << 2);
}
void DlxGrid::Insert(DlxCell* cell, DlxCell* pos, char dir) {
    switch (dir) {
        case 'C':
            pos = pos->col_;
            ++pos->size_;
        case 'u':
            cell->d_ = pos;
            cell->u_ = pos->u_;
            cell->d_->u_ = cell;
            cell->u_->d_ = cell;
            cell->col_ = pos->col_;
            break;
        case 'd':
            cell->u_ = pos;
            cell->d_ = pos->d_;
            cell->d_->u_ = cell;
            cell->u_->d_ = cell;
            cell->col_ = pos->col_;
            break;
        case 'R':
            pos = pos->row_;
            ++pos->size_;
        case 'l':
            cell->r_ = pos;
            cell->l_ = pos->l_;
            cell->r_->l_ = cell;
            cell->l_->r_ = cell;
            cell->row_ = pos->row_;
            break;
        case 'r':
            cell->l_ = pos;
            cell->r_ = pos->r_;
            cell->r_->l_ = cell;
            cell->l_->r_ = cell;
            cell->row_ = pos->row_;
            break;
        case 'X':
            cell->col_ = pos->col_;
            cell->type_ = -1;
            break;
    }
}
void DlxGrid::Cover(DlxCell* col) {
    if (col->covered_) return;
    cover_stack_.Push(col);
    col->covered_ = 1;
    col->l_->r_ = col->r_;
    col->r_->l_ = col->l_;
    for (DlxCell* i = col->d_; i != col; i = i->d_) {
        for (DlxCell* j = i->r_; j != i; j = j->r_) {
            if (j->d_ != j) {
                j->u_->d_ = j->d_;
                j->d_->u_ = j->u_;
                --j->col_->size_;
                j->covered_ = 1;
            }
        }
        // i->covered_ = 1;
    }
}
// must be exactly reverse order
void DlxGrid::Uncover(DlxCell* col) {
    if (cover_stack_.Top() != col) return;
    for (DlxCell* i = col->u_; i != col; i = i->u_) {
        // i->covered_ = 0;
        for (DlxCell* j = i->l_; j != i; j = j->l_) {
            if (j->d_ != j) {
                j->covered_ = 0;
                ++j->col_->size_;
                j->d_->u_ = j;
                j->u_->d_ = j;
            }
        }
    }
    col->r_->l_ = col;
    col->l_->r_ = col;
    col->covered_ = 0;
    cover_stack_.Pop();
}
void DlxGrid::Choose(DlxCell* cell) {
    DlxCell* row = cell->row_;
    row->u_->d_ = row->d_;
    row->d_->u_ = row->u_;
    for (DlxCell* i = row->r_; i != row; i = i->r_) Cover(i->col_);
    choose_stack_.Push(cell);
    ++choose_count_;
}
DlxCell* DlxGrid::Unchoose() {
    DlxCell* cell = choose_stack_.Pop();
    DlxCell* row = cell->row_;
    for (DlxCell* i = row->l_; i != row; i = i->l_) Uncover(i->col_);
    row->d_->u_ = row;
    row->u_->d_ = row;
    return cell;
}
// same as choose but in different stack
int DlxGrid::Reserve(DlxCell* cell) {
    DlxCell* row = cell->row_;
    if (row->covered_) return -1;
    row->u_->d_ = row->d_;
    row->d_->u_ = row->u_;
    for (DlxCell* i = row->r_; i != row; i = i->r_) Cover(i->col_);
    reserve_stack_.Push(row);
    row->covered_ = 1;
    return 0;
}
DlxCell* DlxGrid::Unreserve() {
    DlxCell* cell = reserve_stack_.Pop();
    DlxCell* row = cell->row_;
    for (DlxCell* i = row->l_; i != row; i = i->l_) Uncover(i->col_);
    row->d_->u_ = row;
    row->u_->d_ = row;
    row->covered_ = 0;
    return cell;
}
int DlxGrid::Search(int max_count) {
    int count = 0;
    DlxCell* best_col;
    DlxCell *i, *t, *tmp;
    DlxCell** row_array;
    int col_size = 0;
    bool back_flag = 0;  // backtrack flag
    int l = 0, k = 0;
    for (;;) {
        // check state, and choose column according to heuristic
        best_col = head_.r_;
        while (best_col != &head_ && best_col->type_ == 0)
            best_col = best_col->r_;
        back_flag = 0;
        if (best_col == &head_) {
            // solution found
            PrintSolution(count++);
            // enough solutions found
            if (max_count > 0 && count >= max_count) return count;
            // continue to find more solutions
            back_flag = 1;
        } else {
            if (best_col->size_ <= 0) back_flag = 1;
            if (!back_flag && !best_col->StopCompare())
                for (i = best_col->r_; i != &head_; i = i->r_) {
                    if (i->type_ == 0) continue;
                    if (i->size_ <= 0) {
                        back_flag = 1;
                        break;
                    }
                    if (i->ColComp(*best_col)) {
                        best_col = i;
                        if (best_col->StopCompare()) break;
                    }
                }
            // col_size = 0;
            // for (i = best_col->d_; i != best_col; i = i->d_) ++col_size;
            col_size = best_col->size_;
        }
        if (back_flag) {
            // backtrack
            if (choose_stack_.bEmpty()) return count;
            // backtrack recursively
            if (sort_rows_) {
                t = Unchoose();
                t = t->d_;
                while (t == t->col_) {
                    // unsort rows by u
                    tmp = t;
                    for (i = t->u_; i != t; i = i->u_) i->d_ = tmp, tmp = i;
                    t->d_ = tmp;
                    if (choose_stack_.bEmpty()) break;
                    t = Unchoose();
                    t = t->d_;
                }
                // search end
                if (t == t->col_ && choose_stack_.bEmpty()) return count;
            } else {
                do {
                    t = Unchoose();
                    t = t->d_;
                } while (t == t->col_ && !choose_stack_.bEmpty());
                // search end
                if (t == t->col_ && choose_stack_.bEmpty()) return count;
            }
            Choose(t);
        } else {
            // choose first one in new layer
            if (sort_rows_ && col_size > 1) {
                row_array = new DlxCell*[col_size];
                l = 0;
                k = 0;
                for (DlxCell* j = best_col->d_; j != best_col; j = j->d_, ++l) {
                    k = l - 1;
                    while (k >= 0 && j->RowComp(*row_array[k])) {
                        row_array[k + 1] = row_array[k];
                        --k;
                    }
                    row_array[k + 1] = j;
                }
                // construct new order by d
                for (int pp = 0; pp < col_size - 1; ++pp)
                    row_array[pp]->d_ = row_array[pp + 1];
                row_array[col_size - 1]->d_ = best_col;
                best_col->d_ = row_array[0];
                delete[] row_array;
            }
            Choose(best_col->d_);
        }
    }
}
}  // namespace dlx
