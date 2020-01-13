#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
struct Grid {
    int grid[9][9];
    int cand[3][9];  // row, column, block
    int pos;         // pos that is filling
};
Grid grids[85];
int counter = 0;
bool check() {
    // check grid at current counter, 0 for ok
    int r, c, b;
    for (r = 0; r < 9; ++r)
        for (c = 0; c < 9; ++c) {
            if (grids[counter].grid[r][c] > 0) continue;
            b = r / 3 * 3 + c / 3;
            if ((grids[counter].cand[0][r] & grids[counter].cand[1][c] &
                 grids[counter].cand[2][b]) == 0)
                return 1;
        }
    return 0;
}
int solution_counter = 0;
Grid solutions[2];  // 2 solutions at most
int getsize(int num) {
    int size = 0;
    while (num > 0) {
        while ((num & 1) == 0) num >>= 1;
        ++size;
        num >>= 1;
    }
    return size;
}
int findpos() {
    int best_pos = 0;
    while (best_pos < 81 && grids[counter].grid[best_pos / 9][best_pos % 9] > 0)
        ++best_pos;
    int r = best_pos / 9, c = best_pos % 9;
    int b = r / 3 * 3 + c / 3;
    int min_size =
        getsize(grids[counter].cand[0][r] & grids[counter].cand[1][c] &
                grids[counter].cand[2][b]);
    int tmp_size;
    for (; r < 9; ++r)
        for (c = 0; c < 9; ++c) {
            if (grids[counter].grid[r][c] > 0) continue;
            b = r / 3 * 3 + c / 3;
            tmp_size =
                getsize(grids[counter].cand[0][r] & grids[counter].cand[1][c] &
                        grids[counter].cand[2][b]);
            if (min_size > tmp_size) {
                min_size = tmp_size;
                best_pos = r * 9 + c;
            }
        }
    grids[counter].pos = best_pos;
    return best_pos;
}
void dfs() {
    if (counter < 0) return;
    // choose pos for the next layer, but don't choose number
    int pos = grids[counter].pos;
    int r = pos / 9, c = pos % 9;
    int b = r / 3 * 3 + c / 3;
    int num = grids[counter].grid[r][c];
    int cand = grids[counter].cand[0][r] & grids[counter].cand[1][c] &
               grids[counter].cand[2][b];
    int off = num + 1;
    cand >>= off;
    ++counter;
    while (cand > 0) {
        memcpy(&grids[counter], &grids[counter - 1], sizeof(Grid));
        while (!(cand & 1)) {
            ++off;
            cand >>= 1;
        }
        grids[counter].grid[r][c] = off;
        grids[counter].cand[0][r] ^= 1 << off;
        grids[counter].cand[1][c] ^= 1 << off;
        grids[counter].cand[2][b] ^= 1 << off;
        // while (pos < 81 && grids[counter].grid[pos / 9][pos % 9] > 0) ++pos;
        if (findpos() == 81) {
            memcpy(solutions + solution_counter, grids + counter, sizeof(Grid));
            ++solution_counter;
            if (solution_counter == 2) return;
        }
        // grids[counter].pos = pos;
        if (!check()) dfs();
        if (solution_counter == 2) return;
        ++off;
        cand >>= 1;
    }
    --counter;
}
int main() {
    string str;
    printf("input grid: ");
    cin >> str;
    auto start_time = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 81; ++i)
        if (str[i] >= '1' && str[i] <= '9') {
            grids[0].grid[i / 9][i % 9] = str[i] - '0';
        }
    int r, c, b;
    for (int i = 0; i < 9; ++i) {
        grids[0].cand[0][i] = ((1 << 9) - 1) << 1;
        grids[0].cand[1][i] = ((1 << 9) - 1) << 1;
        grids[0].cand[2][i] = ((1 << 9) - 1) << 1;
    }
    for (r = 0; r < 9; ++r)
        for (c = 0; c < 9; ++c) {
            b = r / 3 * 3 + c / 3;
            grids[0].cand[0][r] ^= 1 << grids[0].grid[r][c];
            grids[0].cand[1][c] ^= 1 << grids[0].grid[r][c];
            grids[0].cand[2][b] ^= 1 << grids[0].grid[r][c];
        }
    int pos = 0;
    while (pos < 81 && grids[0].grid[pos / 9][pos % 9] > 0) ++pos;
    counter = 0;
    grids[0].pos = pos;
    dfs();
    for (int i = 0; i < solution_counter; ++i) {
        for (r = 0; r < 9; ++r) {
            for (c = 0; c < 9; ++c) {
                printf("%d", solutions[i].grid[r][c]);
            }
        }
        printf("\n");
    }
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    printf("%d solutions found in %.3fms.\n", solution_counter,
           duration.count() / 1e6);
    return 0;
}
