#include "hanidoku.h"

#include <chrono>
HanidokuGrid::HanidokuGrid() {
    // init full grid
    int t = 0, m = 0, len;
    dlx::DlxCell *start, *end;
    start = col_;
    end = col_ + 304;
    for (dlx::DlxCell* i = start; i != end; ++i) Insert(i, &head_, 'R');
    int cstart, cend;
    for (int r = 0; r < 9; ++r) {
        cstart = (r < 5) ? 0 : r - 4;
        cend = (r < 5) ? r + 5 : 9;
        for (int c = cstart; c < cend; ++c) {
            for (int n = 0; n < 9; ++n) {
                row_[t].r_ = r;
                row_[t].c_ = c;
                row_[t].n_ = n;
                row_[t].pos_ = t / 9;
                Insert(row_ + t, &head_, 'C');
                Insert(cell_ + m, row_ + t, 'R');
                Insert(cell_ + m, col_ + t / 9, 'C');
                ++m;
                Insert(cell_ + m, row_ + t, 'R');
                Insert(cell_ + m, col_ + 61 + r * 9 + n, 'C');
                ++m;
                Insert(cell_ + m, row_ + t, 'R');
                Insert(cell_ + m, col_ + 142 + c * 9 + n, 'C');
                ++m;
                Insert(cell_ + m, row_ + t, 'R');
                Insert(cell_ + m, col_ + 223 + (4 + c - r) * 9 + n, 'C');
                ++m;
                // row len
                len = 9 - std::abs(4 - r);
                for (int i = 0; i <= n - len; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 61 + r * 9 + i, 'X');
                    ++m;
                    col_[61 + r * 9 + i].type_ = 0;
                }
                for (int i = n + len; i < 9; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 61 + r * 9 + i, 'X');
                    ++m;
                    col_[61 + r * 9 + i].type_ = 0;
                }
                // column len
                len = 9 - std::abs(4 - c);
                for (int i = 0; i <= n - len; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 142 + c * 9 + i, 'X');
                    ++m;
                    col_[142 + c * 9 + i].type_ = 0;
                }
                for (int i = n + len; i < 9; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 142 + c * 9 + i, 'X');
                    ++m;
                    col_[142 + c * 9 + i].type_ = 0;
                }
                // diag len
                len = 9 - std::abs(c - r);
                for (int i = 0; i <= n - len; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 223 + (4 + c - r) * 9 + i, 'X');
                    ++m;
                    col_[223 + (4 + c - r) * 9 + i].type_ = 0;
                }
                for (int i = n + len; i < 9; ++i) {
                    Insert(cell_ + m, row_ + t, 'R');
                    Insert(cell_ + m, col_ + 223 + (4 + c - r) * 9 + i, 'X');
                    ++m;
                    col_[223 + (4 + c - r) * 9 + i].type_ = 0;
                }
                t++;
            }
        }
    }
    InitStacks(549);
}
void HanidokuGrid::PrintSolution(int count) {
    if (benchmark_) return;
    if (verbose_) printf("Solution #%d\n", count);
    HanidokuRowCell* row;
    for (int i = 0; i < choose_stack_.size_; ++i) {
        row = (HanidokuRowCell*)choose_stack_.data_[i]->Row();
        str_[row->pos_] = '1' + (char)row->n_;
    }
    str_[61] = '\0';
    PrintGrid();
}
void HanidokuGrid::PrintGrid() {
    char line[62];
    int t = 0, cstart, cend;
    switch (output_format_) {
        case 0:
            std::memcpy(line, str_, 62);
            for (int i = 0; i < 61; ++i)
                if (!std::isalnum(line[i]) || line[i] == '0') line[i] = '.';
            printf("%s\n", line);
            break;
        case 1:
            std::memcpy(line, str_, 62);
            for (int i = 0; i < 61; ++i)
                if (!std::isalnum(line[i]) || line[i] == '0') line[i] = '0';
            printf("%s\n", line);
            break;
        case 2:
            line[21] = '\0';
            std::memset(line, ' ', 20);
            printf("     .---------.\n");
            for (int i = 0; i < 9; ++i) {
                // clear line
                std::memset(line, ' ', 20);
                cstart = (i < 5) ? 0 : i - 4;
                cend = (i < 5) ? i + 5 : 9;
                for (int j = cstart; j < cend; ++j) {
                    if (std::isalnum(str_[t]) && str_[t] != '0')
                        line[4 - i + 2 + j * 2] = str_[t];
                    else
                        line[4 - i + 2 + j * 2] = '.';
                    ++t;
                }
                if (i == 4) {
                    line[0] = '{';
                    line[20] = '}';
                } else if (i < 4) {
                    line[4 - i] = '/';
                    line[4 - i + cend * 2 + 2] = '\\';
                    line[5 - i + cend * 2 + 2] = '\0';
                } else {
                    line[4 - i + cstart * 2] = '\\';
                    line[4 - i + cend * 2 + 2] = '/';
                    line[5 - i + cend * 2 + 2] = '\0';
                }
                printf("%s\n", line);
            }
            printf("     '---------'\n");
            break;
        case 3:
            std::memcpy(line, str_, 62);
            for (int i = 0; i < 61; ++i)
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
    HanidokuGrid grid;
    // no significant improvement
    grid.sort_rows_ = 0;
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
        for (int i = 0; i < 61; ++i)
            if (grid.str_[i] >= '1' && grid.str_[i] <= '9')
                grid.Reserve(grid.row_ + i * 9 + grid.str_[i] - '1');
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
    else
        fprintf(stderr, "%d puzzles solved in %.3fms.\n", puzzle_count,
                duration.count() / 1e6);
    return 0;
}
// gen by randomly setting clues
int generator(int argc, char* argv[]) {
    std::srand((unsigned)std::time(nullptr));
    auto start_time = std::chrono::high_resolution_clock::now();
    HanidokuGrid grid;
    int puzzle_count = 1;
    int max_clue = 61;
    int count;
    HanidokuRowCell* tmp;
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
                "  -m / --max_clue N: maximum number of clues (default: 61)\n");
            fprintf(stderr, "  -v / --verbose: verbose output\n");
            return 0;
        }
    }
    int i;
    // one parameter here
    const int addon = 3;
    // try min 4 clues
    const int CLUE_MIN = 3;
    for (int tt = 0; tt < puzzle_count; ++tt) {
        std::memset(grid.str_, '.', 61);
        grid.str_[61] = '\0';
        for (i = 0; i < max_clue + addon; ++i) {
            tmp = (HanidokuRowCell*)grid.GetRandRow();
            if (grid.Reserve(tmp) == -1) {
                // should be no fail
                // fprintf(stderr, "fail\n");
                --i;
                continue;
            }
            grid.str_[tmp->pos_] = (char)tmp->n_ + '1';
            if (i < CLUE_MIN)
                count = grid.Search(1);
            else
                count = grid.Search(2);
            grid.UnchooseAll();
            if (i >= CLUE_MIN && count == 1) {
                // check for mininal puzzle
                HanidokuRowCell** presets;
                presets = new HanidokuRowCell*[i + 1];
                int k, l;
                k = 0;
                std::memcpy(presets, grid.reserve_stack_.data_,
                            (unsigned)(i + 1) * sizeof(void*));
                while (i > CLUE_MIN) {
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
                        grid.str_[presets[k]->pos_] = '.';
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
                grid.str_[tmp->pos_] = '.';
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
    HanidokuGrid grid;
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
        for (int i = 0; i < 61; ++i)
            if (std::isalnum(grid.str_[i]) && grid.str_[i] != '0') ++clues;
        if (level > 0) {
            for (int i = 0; i < 61; ++i)
                if (grid.str_[i] >= '1' && grid.str_[i] <= '9')
                    grid.Reserve(grid.row_ + i * 9 + grid.str_[i] - '1');
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
