#include "sudoku.h"

#include <chrono>
SudokuGrid::SudokuGrid() {
    // init full grid
    int t = 0;
    int b;
    dlx::DlxCell *start, *end;
    start = col_;
    end = col_ + 4 * N4;
    for (dlx::DlxCell* i = start; i != end; ++i) Insert(i, &head_, 'R');
    for (int r = 0; r < N2; ++r)
        for (int c = 0; c < N2; ++c) {
            b = r / N * N + c / N;
            for (int n = 0; n < N2; ++n) {
                row_[t].r_ = r;
                row_[t].c_ = c;
                row_[t].n_ = n;
                Insert(row_ + t, &head_, 'C');
                Insert(cell_ + t, row_ + t, 'R');
                Insert(cell_ + t, col_ + r * N2 + c, 'C');
                Insert(cell_ + N6 + t, row_ + t, 'R');
                Insert(cell_ + N6 + t, col_ + N4 + r * N2 + n, 'C');
                Insert(cell_ + 2 * N6 + t, row_ + t, 'R');
                Insert(cell_ + 2 * N6 + t, col_ + 2 * N4 + c * N2 + n, 'C');
                Insert(cell_ + 3 * N6 + t, row_ + t, 'R');
                Insert(cell_ + 3 * N6 + t, col_ + 3 * N4 + b * N2 + n, 'C');
                t++;
            }
        }
    InitStacks();
}
void SudokuGrid::PrintSolution(int count) {
    if (benchmark_) return;
    if (verbose_) printf("Solution #%d\n", count);
    SudokuRowCell* row;
    for (int i = 0; i < choose_stack_.size_; ++i) {
        row = (SudokuRowCell*)choose_stack_.data_[i]->Row();
        if (row->n_ < 9)
            str_[row->r_ * N2 + row->c_] = '1' + (char)row->n_;
        else
            str_[row->r_ * N2 + row->c_] = 'A' + (char)row->n_ - 9;
    }
    str_[N4] = '\0';
    PrintGrid();
}
void SudokuGrid::PrintGrid() {
    const int block_len = 2 * N + 2;
    const size_t len = N * block_len + 2;
    char sep[len];
    char line[N4 + 1];
    switch (output_format_) {
        case 0:
            std::memcpy(line, str_, N4 + 1);
            for (int i = 0; i < N4; ++i)
                if (!std::isalnum(line[i]) || line[i] == '0') line[i] = '.';
            printf("%s\n", line);
            break;
        case 1:
            std::memcpy(line, str_, N4 + 1);
            for (int i = 0; i < N4; ++i)
                if (!std::isalnum(line[i]) || line[i] == '0') line[i] = '0';
            printf("%s\n", line);
            break;
        case 2:
            sep[len - 1] = '\0';
            line[len - 1] = '\0';
            std::memset(sep, '-', len - 1);
            std::memset(line, ' ', len - 1);
            for (int i = 0; i <= N; ++i) {
                sep[i * block_len] = '.';
                line[i * block_len] = '|';
            }
            printf("%s\n", sep);
            sep[0] = ':';
            sep[len - 2] = ':';
            for (int i = 1; i < N; ++i) sep[i * block_len] = '+';
            for (int j = 0; j < N2; ++j) {
                if (std::isalnum(str_[j]) && str_[j] != '0')
                    line[j / N * block_len + j % N * 2 + 2] = str_[j];
            }
            printf("%s\n", line);
            for (int i = 1; i < N2; ++i) {
                if (i % N == 0) printf("%s\n", sep);
                // clear line
                for (int j = 0; j < N; ++j)
                    std::memset(line + j * block_len + 1, ' ', block_len - 1);
                for (int j = 0; j < N2; ++j) {
                    if (std::isalnum(str_[i * N2 + j]) &&
                        str_[i * N2 + j] != '0')
                        line[j / N * block_len + j % N * 2 + 2] =
                            str_[i * N2 + j];
                }
                printf("%s\n", line);
            }
            for (int i = 0; i <= N; ++i) sep[i * block_len] = '\'';
            printf("%s\n", sep);
            break;
        case 3:
            std::memcpy(line, str_, N4 + 1);
            for (int i = 0; i < N4; ++i)
                if (!std::isalnum(line[i]) || line[i] == '0') line[i] = '-';
            printf("%s\n", line);
            break;
        default:
            printf("%s\n", str_);
            break;
    }
}
int strtoi(char* str) { return (int)strtol(str, nullptr, 10); }
int solver(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();
    SudokuGrid grid;
    int puzzle_count = 1;
    int max_solution = -1;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
            puzzle_count = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-b") == 0 ||
                 strcmp(argv[i], "--benchmark") == 0)
            grid.benchmark_ = 1;
        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0)
            grid.output_format_ = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-m") == 0 ||
                 strcmp(argv[i], "--max_solution") == 0)
            max_solution = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-v") == 0 ||
                 strcmp(argv[i], "--verbose") == 0)
            grid.verbose_ = 1;
        else {
            fprintf(stderr, "Usage: %s %s ...\n", argv[0], argv[1]);
            fprintf(stderr,
                    "  -b / --benchmark: don't print solution, for benchmark "
                    "purpose\n");
            fprintf(stderr,
                    "  -c / --count N: count of puzzle to solve (default: "
                    "1)\n");
            fprintf(stderr,
                    "  -f / --format [0/1/2]: print format (default: 0)\n");
            fprintf(stderr, "    0) one-line, '.' denotes blank\n");
            fprintf(stderr, "    1) one-line, '0' denotes blank\n");
            fprintf(stderr, "    2) reader-friendly grid, ' ' denotes blank\n");
            fprintf(stderr,
                    "  -m / --max_solution N: maximum number of solutions, "
                    "-1 means no limit (default: -1)\n");
            fprintf(stderr, "  -v / --verbose: verbose output\n");
            return 0;
        }
    }
    for (int tt = 0; tt < puzzle_count; ++tt) {
        scanf("%s", grid.str_);
        for (int i = 0; i < N4; ++i)
            if (grid.str_[i] >= '1' && grid.str_[i] <= '9')
                grid.Reserve(grid.GetRow(i / N2, i % N2, grid.str_[i] - '1'));
            else if (grid.str_[i] >= 'A' && grid.str_[i] <= 'Z')
                grid.Reserve(
                    grid.GetRow(i / N2, i % N2, grid.str_[i] - 'A' + 9));
        if (grid.verbose_) printf("Puzzle #%d\n", tt);
        if (grid.benchmark_)
            grid.Search(max_solution);
        else if (max_solution < 0)
            fprintf(stderr, "Puzzle #%d: %d solutions found.\n", tt,
                    grid.Search(max_solution));
        else
            fprintf(stderr, "Puzzle #%d: %d/%d solutions found.\n", tt,
                    grid.Search(max_solution), max_solution);
        grid.UnchooseAll();
        grid.UnreserveAll();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    if (duration.count() > 1e10)
        fprintf(stderr, "%d puzzles solved in %.3fs.\n", puzzle_count,
                duration.count() / 1e9);
    else if (duration.count() > 1e5)
        fprintf(stderr, "%d puzzles solved in %.3fms.\n", puzzle_count,
                duration.count() / 1e6);
    else
        fprintf(stderr, "%d puzzles solved in %.3fus.\n", puzzle_count,
                duration.count() / 1e3);
    return 0;
}
// gen by randomly setting clues
int generator(int argc, char* argv[]) {
    std::srand((unsigned)std::time(nullptr));
    auto start_time = std::chrono::high_resolution_clock::now();
    SudokuGrid grid;
    int puzzle_count = 1;
    int max_clue = N4;
    int count;
    SudokuRowCell* tmp;
    grid.benchmark_ = 1;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
            puzzle_count = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0)
            grid.output_format_ = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-m") == 0 ||
                 strcmp(argv[i], "--max_clue") == 0)
            max_clue = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-v") == 0 ||
                 strcmp(argv[i], "--verbose") == 0)
            grid.verbose_ = 1;
        else {
            fprintf(stderr, "Usage: %s %s ...\n", argv[0], argv[1]);
            fprintf(
                stderr,
                "  -c / --count N: count of puzzle to generate (default: 1)\n");
            fprintf(stderr,
                    "  -f / --format [0/1/2]: print format (default: 0)\n");
            fprintf(stderr, "    0) one-line, '.' denotes blank\n");
            fprintf(stderr, "    1) one-line, '0' denotes blank\n");
            fprintf(stderr, "    2) reader-friendly grid, ' ' denotes blank\n");
            fprintf(
                stderr,
                "  -m / --max_clue N: maximum number of clues (default: %d)\n",
                N4);
            fprintf(stderr, "  -v / --verbose: verbose output\n");
            return 0;
        }
    }
    int i;
    // one parameter here
    const int addon = 3;
    for (int tt = 0; tt < puzzle_count; ++tt) {
        std::memset(grid.str_, '.', N4);
        grid.str_[N4] = '\0';
        for (i = 0; i < max_clue + addon; ++i) {
            tmp = (SudokuRowCell*)grid.GetRandRow();
            if (grid.Reserve(tmp) == -1) {
                // should be no fail
                // fprintf(stderr, "fail\n");
                --i;
                continue;
            }
            if (tmp->n_ < 9)
                grid.str_[tmp->r_ * N2 + tmp->c_] = (char)tmp->n_ + '1';
            else
                grid.str_[tmp->r_ * N2 + tmp->c_] = (char)tmp->n_ + 'A' - 9;
            // min 17 clues
            if (i < 16)
                count = grid.Search(1);
            else
                count = grid.Search(2);
            grid.UnchooseAll();
            if (i >= 16 && count == 1) {
                // check for mininal puzzle
                SudokuRowCell** presets;
                presets = new SudokuRowCell*[i + 1];
                int k, l;
                k = 0;
                std::memcpy(presets, grid.reserve_stack_.data_,
                            (unsigned)(i + 1) * sizeof(void*));
                while (i > 16) {
                    grid.UnreserveAll();
                    for (; k <= i; ++k) {
                        for (l = k + 1; l <= i; ++l) grid.Reserve(presets[l]);
                        if (grid.Search(2) == 1)
                            break;
                        else {
                            grid.UnchooseAll();
                            for (l = k; l < i; ++l) grid.Unreserve();
                            grid.Reserve(presets[k]);
                        }
                    }
                    if (k > i)
                        break;
                    else {
                        --i;
                        grid.str_[presets[k]->r_ * N2 + presets[k]->c_] = '.';
                        std::memcpy(presets, grid.reserve_stack_.data_,
                                    (unsigned)(i + 1) * sizeof(void*));
                    }
                }
                delete[] presets;
                if (i < max_clue) {
                    if (grid.verbose_)
                        printf("Puzzle #%d, %d clues\n", tt, i + 1);
                    grid.PrintGrid();
                } else {
                    --tt;
                }
                break;
            } else if (count == 0) {
                grid.Unreserve();
                grid.str_[tmp->r_ * N2 + tmp->c_] = '.';
                --i;
            }
        }
        grid.UnchooseAll();
        grid.UnreserveAll();
        if (i == max_clue + addon) {
            // fprintf(stderr, "Failed, try once more.\n");
            --tt;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    if (duration.count() > 1e10)
        fprintf(stderr, "%d puzzles generated in %.3fs.\n", puzzle_count,
                duration.count() / 1e9);
    else
        fprintf(stderr, "%d puzzles generated in %.3fms.\n", puzzle_count,
                duration.count() / 1e6);
    return 0;
}
int convert(int argc, char* argv[]) {
    SudokuGrid grid;
    int puzzle_count = 1, input_format = 0;
    int level = -1;
    grid.output_format_ = 2;
    grid.benchmark_ = 1;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
            puzzle_count = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--in") == 0)
            input_format = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--level") == 0)
            level = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--out") == 0)
            grid.output_format_ = strtoi(argv[++i]);
        else if (strcmp(argv[i], "-v") == 0 ||
                 strcmp(argv[i], "--verbose") == 0)
            grid.verbose_ = 1;
        else {
            fprintf(stderr, "Usage: %s %s ...\n", argv[0], argv[1]);
            fprintf(
                stderr,
                "  -c / --count N: count of puzzle to convert (default: 1)\n");
            fprintf(stderr,
                    "  -i / --input [0/1/3]: input format (default: 0)\n");
            fprintf(stderr, "    0) one-line, '.' denotes blank\n");
            fprintf(stderr, "    1) one-line, '0' denotes blank\n");
            // fprintf(stderr, "    2) reader-friendly grid, ' ' denotes
            // blank\n");
            fprintf(stderr, "    3) one-line, '-' denotes blank\n");
            fprintf(stderr,
                    "  -l / --level N: only print puzzles with level>=N,\n"
                    "which reflects how hard it is to find a solution by dfs "
                    "(default: -1)\n");
            fprintf(stderr,
                    "  -o / --output [0/1/2/3]: output format (default: 2)\n");
            fprintf(stderr, "    0) one-line, '.' denotes blank\n");
            fprintf(stderr, "    1) one-line, '0' denotes blank\n");
            fprintf(stderr, "    2) reader-friendly grid, ' ' denotes blank\n");
            fprintf(stderr, "    3) one-line, '-' denotes blank\n");
            fprintf(stderr, "  -v / --verbose: verbose output\n");
            return 0;
        }
    }
    int clues;
    for (int tt = 0; tt < puzzle_count; ++tt) {
        if (input_format != 2) scanf("%s", grid.str_);
        clues = 0;
        for (int i = 0; i < N4; ++i)
            if (std::isalnum(grid.str_[i]) && grid.str_[i] != '0') ++clues;
        if (level > 0) {
            for (int i = 0; i < N4; ++i)
                if (grid.str_[i] >= '1' && grid.str_[i] <= '9')
                    grid.Reserve(
                        grid.GetRow(i / N2, i % N2, grid.str_[i] - '1'));
                else if (grid.str_[i] >= 'A' && grid.str_[i] <= 'Z')
                    grid.Reserve(
                        grid.GetRow(i / N2, i % N2, grid.str_[i] - 'A' + 9));
            grid.UnchooseAll();
            grid.choose_count_ = 0;
            grid.Search(1);
            if (grid.choose_count_ >= level) {
                if (grid.verbose_)
                    printf("Puzzle #%d, %d clues, level %d\n", tt, clues,
                           grid.choose_count_);
                grid.PrintGrid();
            }
            grid.UnchooseAll();
            grid.UnreserveAll();
        } else {
            if (grid.verbose_) printf("Puzzle #%d, %d clues\n", tt, clues);
            grid.PrintGrid();
        }
    }
    return 0;
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s solve/generate/convert ...\n", argv[0]);
        return 0;
    }
    if (argv[1][0] == 'g')
        return generator(argc, argv);
    else if (argv[1][0] == 's')
        return solver(argc, argv);
    else if (argv[1][0] == 'c')
        return convert(argc, argv);
    else
        fprintf(stderr, "Usage: %s solve/generate/convert ...\n", argv[0]);
    return 0;
}
