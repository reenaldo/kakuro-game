#ifndef SOLVER_H
#define SOLVER_H

#include "Grille.h"
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <bitset>

class Solver {
public:
    static constexpr int MAX_CELL_VALUE = 9; 
    Solver(bool deterministic = false) : lastRows(-1), lastCols(-1), deterministic_mode(deterministic) {}

    // Initialize run state then solve
    bool solve(Grille& grid) {
        if (!grid.validateStructure()) return false;
        if (!preprocessRuns(grid)) return false;
        int rows = grid.getRows(), cols = grid.getCols();
        possibleValuesCache.assign(static_cast<size_t>(rows), std::vector<std::vector<int>>(static_cast<size_t>(cols)));
        cacheComputed.assign(static_cast<size_t>(rows), std::vector<bool>(static_cast<size_t>(cols), false));
        invalidateAllPossibleValues(); // Invalidate cache on new solve
        int nr = static_cast<int>(rowRuns.size()), nc = static_cast<int>(colRuns.size());
        rowSum.assign(static_cast<size_t>(nr), 0);
        rowMask.assign(static_cast<size_t>(nr), 0);
        rowEmpty.resize(static_cast<size_t>(nr));
        for (size_t i = 0; i < static_cast<size_t>(nr); ++i) rowEmpty[i] = static_cast<int>(rowRuns[i].cells.size());
        colSum.assign(static_cast<size_t>(nc), 0);
        colMask.assign(static_cast<size_t>(nc), 0);
        colEmpty.resize(static_cast<size_t>(nc));
        for (size_t i = 0; i < static_cast<size_t>(nc); ++i) colEmpty[i] = static_cast<int>(colRuns[i].cells.size());
        return solveRecursive(grid);
    }

    bool verify(const Grille& grid) {
        int rows = grid.getRows();
        int cols = grid.getCols();
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                const Cell* cell = grid.getCell(static_cast<int>(i), static_cast<int>(j));
                if (cell && cell->isMaster()) {
                    int clueRow = cell->getValRow();
                    int clueCol = cell->getValCol();
                    if (clueRow > 0) {
                        int sum = 0;
                        int seen = 0;
                        for (int y = static_cast<int>(j)+1; y < cols; ++y) {
                            const Cell* c2 = grid.getCell(static_cast<int>(i), y);
                            if (!c2 || !c2->isWhite()) break;
                            auto w = static_cast<const Cell_white*>(c2);
                            int v = w->getValue();
                            if (v <= 0 || v > MAX_CELL_VALUE || (seen & (1 << v))) return false;
                            seen |= (1 << v);
                            sum += v;
                        }
                        if (sum != clueRow) return false;
                    }
                    if (clueCol > 0) {
                        int sum = 0;
                        int seen = 0;
                        for (int x = static_cast<int>(i)+1; x < rows; ++x) {
                            const Cell* c2 = grid.getCell(x, static_cast<int>(j));
                            if (!c2 || !c2->isWhite()) break;
                            auto w = static_cast<const Cell_white*>(c2);
                            int v = w->getValue();
                            if (v <= 0 || v > MAX_CELL_VALUE || (seen & (1 << v))) return false;
                            seen |= (1 << v);
                            sum += v;
                        }
                        if (sum != clueCol) return false;
                    }
                }
            }
        }
        return true;
    }

private:
    // Precomputed runs for faster lookup
    struct Run { int clue; std::vector<std::pair<int,int>> cells; };
    std::vector<Run> rowRuns, colRuns;
    // For each cell, indices of its row and column run in rowRuns/colRuns
    std::vector<std::vector<int>> rowRunIdx, colRunIdx;
    // Runtime state for runs: sum, empty count, and used-digit mask
    std::vector<int> rowSum, colSum, rowEmpty, colEmpty;
    std::vector<std::bitset<10>> rowMask, colMask;
    int lastRows = -1, lastCols = -1;
    bool deterministic_mode = false; // If true, disables shuffling for deterministic solving
    // Cache for possible values per cell (x, y) using fixed-size 2D structure
    std::vector<std::vector<std::vector<int>>> possibleValuesCache;
    std::vector<std::vector<bool>> cacheComputed;

    // Memoization cache for partial solutions
    std::unordered_map<size_t, bool> memoCache;

    // Hash combine utility (boost-like)
    static void hash_combine(std::size_t& seed, std::size_t value) noexcept {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // Hash the current partial solution (white cell values + run states)
    size_t computeStateHash(const Grille& grid) {
        size_t h = 0;
        int rows = grid.getRows(), cols = grid.getCols();
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                const Cell* c = grid.getCell(static_cast<int>(i), static_cast<int>(j));
                if (c && c->isWhite()) {
                    auto w = static_cast<const Cell_white*>(c);
                    hash_combine(h, std::hash<int>()(w->getValue() + 31 * (static_cast<int>(i) * cols + static_cast<int>(j))));
                }
            }
        }
        // Add run state
        for (size_t i = 0; i < rowSum.size(); ++i) {
            hash_combine(h, std::hash<int>()(rowSum[i] + 17 * static_cast<int>(rowMask[i].to_ulong()) + 23 * rowEmpty[i]));
        }
        for (size_t i = 0; i < colSum.size(); ++i) {
            hash_combine(h, std::hash<int>()(colSum[i] + 19 * static_cast<int>(colMask[i].to_ulong()) + 29 * colEmpty[i]));
        }
        return h;
    }

    // Helper for checking if a value can be placed in a run (row or column)
    bool canPlaceInRun(int clue, int sum, int empty, const std::bitset<10>& mask, int v) const;

    // Build runs and index mapping once per grid size
    bool preprocessRuns(const Grille& grid) {
        int rows = grid.getRows(), cols = grid.getCols();
        if (lastRows == rows && lastCols == cols && !rowRuns.empty()) return true;
        lastRows = rows; lastCols = cols;
        rowRuns.clear(); colRuns.clear();
        rowRunIdx.assign(static_cast<size_t>(rows), std::vector<int>(static_cast<size_t>(cols), -1));
        colRunIdx.assign(static_cast<size_t>(rows), std::vector<int>(static_cast<size_t>(cols), -1));
        // Row runs
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                auto master = grid.getCell(static_cast<int>(i), static_cast<int>(j));
                if (!(master && master->isMaster())) continue;
                if (master->getValRow() <= 0) continue; // Skip invalid clues
                Run run; run.clue = master->getValRow();
                int idx = static_cast<int>(rowRuns.size());
                // Reserve maximum possible size for row run
                run.cells.reserve(static_cast<size_t>(cols - (static_cast<int>(j)+1)));
                for (int jj = static_cast<int>(j)+1; jj < cols; ++jj) {
                    auto c = grid.getCell(static_cast<int>(i), jj);
                    if (!c || !c->isWhite()) break;
                    run.cells.emplace_back(static_cast<int>(i), jj);
                    rowRunIdx[i][static_cast<size_t>(jj)] = idx;
                }
                rowRuns.push_back(std::move(run));
            }
        }
        // Column runs
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                auto master = grid.getCell(static_cast<int>(i), static_cast<int>(j));
                if (!(master && master->isMaster())) continue;
                if (master->getValCol() <= 0) continue; // Skip invalid clues
                Run run; run.clue = master->getValCol();
                int idx = static_cast<int>(colRuns.size());
                run.cells.reserve(static_cast<size_t>(rows - (static_cast<int>(i)+1)));
                for (int ii = static_cast<int>(i)+1; ii < rows; ++ii) {
                    auto c = grid.getCell(ii, static_cast<int>(j));
                    if (!c || !c->isWhite()) break;
                    run.cells.emplace_back(ii, static_cast<int>(j));
                    colRunIdx[static_cast<size_t>(ii)][j] = idx;
                }
                colRuns.push_back(std::move(run));
            }
        }
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                auto c = grid.getCell(static_cast<int>(i), static_cast<int>(j));
                if (c && c->isWhite()) {
                    if (rowRunIdx[i][j] == -1 || colRunIdx[i][j] == -1) {
                        std::cerr << "Erreur : La case blanche en (" << i << ", " << j << ") n'appartient pas à une ligne et une colonne valides. Les deux indices doivent être >= 0 mais ont rowRunIdx=" << rowRunIdx[i][j] << ", colRunIdx=" << colRunIdx[i][j] << "." << std::endl;
                        return false; // Orphan white cell, abort
                    }
                }
            }
        }
        return true;
    }

    // Helper to get possible values for a cell (with caching)
    std::vector<int> getPossibleValues(const Grille& grid, int x, int y);
    // Invalidate possible values cache for a cell
    void invalidatePossibleValues(int x, int y);
    // Invalidate all possible values cache
    void invalidateAllPossibleValues();
    // Forward checking: only check the runs (row and col) that (x,y) belongs to
    bool forwardCheck(const Grille& grid, int x, int y);
    // Recursive solver
    bool solveRecursive(Grille& grid);
    // Can place value in cell
    bool canPlace(const Grille& grid, int x, int y, int v);
};

#endif