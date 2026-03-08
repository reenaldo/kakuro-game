/**
 * @file Solver.cpp
 * @brief Implémentation des méthodes de la classe Solver pour la résolution de Kakuro.
 *
 * Ce fichier contient les définitions des fonctions membres de la classe Solver,
 * notamment les méthodes de propagation de contraintes, de recherche récursive,
 * de gestion des caches de valeurs possibles, et de vérification de placement.
 */

#include "Solver.h"

// use Solver::MAX_CELL_VALUE instead of local MAX_VALUE

/**
 * @brief Vérifie si une valeur peut être placée dans une run (ligne ou colonne).
 *
 * @param clue Indice de somme de la run.
 * @param sum Somme courante dans la run.
 * @param empty Nombre de cases vides restantes dans la run.
 * @param mask Masque des chiffres déjà utilisés dans la run.
 * @param v Valeur à tester.
 * @return true si la valeur peut être placée, false sinon.
 */
bool Solver::canPlaceInRun(int clue, int sum, int empty, const std::bitset<10>& mask, int v) const {
    if (mask.test(static_cast<size_t>(v))) return false; // duplicate digit
    sum += v;
    empty -= 1;
    auto new_mask = mask;
    new_mask.set(static_cast<size_t>(v));
    if (empty == 0) {
        return sum == clue;
    } else {
        int minSum = sum, maxSum = sum, cnt = empty;
        for (int d = 1; d <= MAX_CELL_VALUE && cnt > 0; ++d) {
            if (!new_mask.test(static_cast<size_t>(d))) { minSum += d; cnt--; }
        }
        cnt = empty;
        for (int d = MAX_CELL_VALUE; d >= 1 && cnt > 0; --d) {
            if (!new_mask.test(static_cast<size_t>(d))) { maxSum += d; cnt--; }
        }
        return minSum <= clue && clue <= maxSum;
    }
}

/**
 * @brief Vérifie si une valeur peut être placée dans une cellule donnée.
 *
 * @param grid Grille courante.
 * @param x Indice de ligne.
 * @param y Indice de colonne.
 * @param v Valeur à tester.
 * @return true si la valeur peut être placée, false sinon.
 */
bool Solver::canPlace(const Grille& /*grid*/, int x, int y, int v) {
    int rIdx = rowRunIdx[static_cast<size_t>(x)][static_cast<size_t>(y)];
    if (rIdx >= 0) {
        const auto& run = rowRuns[static_cast<size_t>(rIdx)];
        if (!canPlaceInRun(run.clue, rowSum[static_cast<size_t>(rIdx)], rowEmpty[static_cast<size_t>(rIdx)], rowMask[static_cast<size_t>(rIdx)], v)) return false;
    }
    int cIdx = colRunIdx[static_cast<size_t>(x)][static_cast<size_t>(y)];
    if (cIdx >= 0) {
        const auto& run = colRuns[static_cast<size_t>(cIdx)];
        if (!canPlaceInRun(run.clue, colSum[static_cast<size_t>(cIdx)], colEmpty[static_cast<size_t>(cIdx)], colMask[static_cast<size_t>(cIdx)], v)) return false;
    }
    return true;
}

/**
 * @brief Calcule les valeurs possibles pour une cellule (avec cache).
 *
 * @param grid Grille courante.
 * @param x Indice de ligne.
 * @param y Indice de colonne.
 * @return Vecteur des valeurs possibles.
 */
std::vector<int> Solver::getPossibleValues(const Grille& grid, int x, int y) {
    if (cacheComputed[static_cast<size_t>(x)][static_cast<size_t>(y)]) return possibleValuesCache[static_cast<size_t>(x)][static_cast<size_t>(y)];
    cacheComputed[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
    auto& vals = possibleValuesCache[static_cast<size_t>(x)][static_cast<size_t>(y)];
    vals.clear();
    vals.reserve(MAX_CELL_VALUE);
    for (int v = 1; v <= MAX_CELL_VALUE; ++v) {
        if (canPlace(grid, x, y, v)) vals.push_back(v);
    }
    return vals;
}

/**
 * @brief Invalide le cache des valeurs possibles pour une cellule.
 *
 * @param x Indice de ligne.
 * @param y Indice de colonne.
 */
void Solver::invalidatePossibleValues(int x, int y) {
    cacheComputed[static_cast<size_t>(x)][static_cast<size_t>(y)] = false;
    possibleValuesCache[static_cast<size_t>(x)][static_cast<size_t>(y)].clear();
}

/**
 * @brief Invalide le cache des valeurs possibles pour toute la grille.
 */
void Solver::invalidateAllPossibleValues() {
    for (size_t i = 0; i < possibleValuesCache.size(); ++i) {
        for (size_t j = 0; j < possibleValuesCache[i].size(); ++j) {
            invalidatePossibleValues(static_cast<int>(i), static_cast<int>(j));
        }
    }
}

/**
 * @brief Effectue un forward checking sur les runs d'une cellule.
 *
 * @param grid Grille courante.
 * @param x Indice de ligne.
 * @param y Indice de colonne.
 * @return true si le forward check passe, false sinon.
 */
bool Solver::forwardCheck(const Grille& grid, int x, int y) {
    int rIdx = rowRunIdx[static_cast<size_t>(x)][static_cast<size_t>(y)];
    if (rIdx >= 0) {
        auto& run = rowRuns[static_cast<size_t>(rIdx)];
        for (auto [i,j] : run.cells) {
            const Cell* c = grid.getCell(i, j);
            int val = (c && c->isWhite()) ? static_cast<const Cell_white*>(c)->getValue() : 0;
            if (val==0) {
                bool ok=false;
                for (int v=1;v<=MAX_CELL_VALUE;++v) if (canPlace(grid,i,j,v)) { ok=true; break; }
                if (!ok) return false;
            }
        }
    }
    int cIdx = colRunIdx[static_cast<size_t>(x)][static_cast<size_t>(y)];
    if (cIdx >= 0) {
        auto& run = colRuns[static_cast<size_t>(cIdx)];
        for (auto [i,j] : run.cells) {
            const Cell* c = grid.getCell(i, j);
            int val = (c && c->isWhite()) ? static_cast<const Cell_white*>(c)->getValue() : 0;
            if (val==0) {
                bool ok=false;
                for (int v=1;v<=MAX_CELL_VALUE;++v) if (canPlace(grid,i,j,v)) { ok=true; break; }
                if (!ok) return false;
            }
        }
    }
    return true;
}

/**
 * @brief Fonction récursive principale de résolution (backtracking + MRV + forward checking).
 *
 * @param grid Grille courante à résoudre.
 * @return true si une solution est trouvée, false sinon.
 */
bool Solver::solveRecursive(Grille& grid) {
    size_t stateHash = computeStateHash(grid);
    auto it = memoCache.find(stateHash);
    if (it != memoCache.end()) return it->second;

    int rows = grid.getRows(), cols = grid.getCols();
    int bestX = -1, bestY = -1;
    int minOptions = MAX_CELL_VALUE + 1;
    // MRV: pick the empty white cell with fewest possibilities
    for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
            Cell* c = grid.getCell(static_cast<int>(i), static_cast<int>(j));
            if (c && c->isWhite()) {
                Cell_white* w = static_cast<Cell_white*>(c);
                if (w->getValue() == 0) {
                    auto vals = getPossibleValues(grid, static_cast<int>(i), static_cast<int>(j));
                    int options = static_cast<int>(vals.size());
                    if (options == 0) return memoCache[stateHash] = false;
                    if (options < minOptions) {
                        minOptions = options;
                        bestX = static_cast<int>(i); bestY = static_cast<int>(j);
                    }
                }
            }
        }
    }
    // if no empty cell, verify solution
    if (bestX == -1) {
        bool res = verify(grid);
        memoCache[stateHash] = res;
        return res;
    }
    // get possibilities for chosen cell
    auto vals = getPossibleValues(grid, bestX, bestY);
    // heuristic sort: minimize impact on peers
    std::sort(vals.begin(), vals.end(), [&](int a, int b) {
        Cell* cc = grid.getCell(bestX, bestY);
        Cell_white* cw = (cc && cc->isWhite()) ? static_cast<Cell_white*>(cc) : nullptr;
        if (cw) cw->setValue(a);
        int countA = 0;
        for (auto [xx,yy] : rowRuns[static_cast<size_t>(rowRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)])].cells) {
            if (xx==bestX && yy==bestY) continue;
            Cell* ci = grid.getCell(xx, yy);
            Cell_white* wi = (ci && ci->isWhite()) ? static_cast<Cell_white*>(ci) : nullptr;
            if (wi && wi->getValue()==0) countA += static_cast<int>(getPossibleValues(grid, xx, yy).size());
        }
        for (auto [xx,yy] : colRuns[static_cast<size_t>(colRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)])].cells) {
            if (xx==bestX && yy==bestY) continue;
            Cell* ci = grid.getCell(xx, yy);
            Cell_white* wi = (ci && ci->isWhite()) ? static_cast<Cell_white*>(ci) : nullptr;
            if (wi && wi->getValue()==0) countA += static_cast<int>(getPossibleValues(grid, xx, yy).size());
        }
        if (cw) cw->setValue(b);
        int countB = 0;
        for (auto [xx,yy] : rowRuns[static_cast<size_t>(rowRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)])].cells) {
            if (xx==bestX && yy==bestY) continue;
            Cell* ci = grid.getCell(xx, yy);
            Cell_white* wi = (ci && ci->isWhite()) ? static_cast<Cell_white*>(ci) : nullptr;
            if (wi && wi->getValue()==0) countB += static_cast<int>(getPossibleValues(grid, xx, yy).size());
        }
        for (auto [xx,yy] : colRuns[static_cast<size_t>(colRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)])].cells) {
            if (xx==bestX && yy==bestY) continue;
            Cell* ci = grid.getCell(xx, yy);
            Cell_white* wi = (ci && ci->isWhite()) ? static_cast<Cell_white*>(ci) : nullptr;
            if (wi && wi->getValue()==0) countB += static_cast<int>(getPossibleValues(grid, xx, yy).size());
        }
        if (cw) cw->setValue(0);
        return countA > countB;
    });
    // randomized tie-breaker if non-deterministic
    if (!deterministic_mode) {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(vals.begin(), vals.end(), g);
    }
    // try each candidate
    bool result = false;
    int rIdx = rowRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)];
    int cIdx = colRunIdx[static_cast<size_t>(bestX)][static_cast<size_t>(bestY)];
    for (int v : vals) {
        Cell* cc = grid.getCell(bestX, bestY);
        Cell_white* cw = (cc && cc->isWhite()) ? static_cast<Cell_white*>(cc) : nullptr;
        if (cw) cw->setValue(v);
        rowSum[static_cast<size_t>(rIdx)] += v; rowMask[static_cast<size_t>(rIdx)].set(static_cast<size_t>(v)); rowEmpty[static_cast<size_t>(rIdx)]--;
        colSum[static_cast<size_t>(cIdx)] += v; colMask[static_cast<size_t>(cIdx)].set(static_cast<size_t>(v)); colEmpty[static_cast<size_t>(cIdx)]--;
        invalidatePossibleValues(bestX, bestY);
        for (auto [xx,yy] : rowRuns[static_cast<size_t>(rIdx)].cells) invalidatePossibleValues(xx, yy);
        for (auto [xx,yy] : colRuns[static_cast<size_t>(cIdx)].cells) invalidatePossibleValues(xx, yy);
        if (forwardCheck(grid, bestX, bestY) && solveRecursive(grid)) { result = true; }
        if (result) break;
        if (cw) cw->setValue(0);
        rowSum[static_cast<size_t>(rIdx)] -= v; rowMask[static_cast<size_t>(rIdx)].reset(static_cast<size_t>(v)); rowEmpty[static_cast<size_t>(rIdx)]++;
        colSum[static_cast<size_t>(cIdx)] -= v; colMask[static_cast<size_t>(cIdx)].reset(static_cast<size_t>(v)); colEmpty[static_cast<size_t>(cIdx)]++;
        invalidatePossibleValues(bestX, bestY);
        for (auto [xx,yy] : rowRuns[static_cast<size_t>(rIdx)].cells) invalidatePossibleValues(xx, yy);
        for (auto [xx,yy] : colRuns[static_cast<size_t>(cIdx)].cells) invalidatePossibleValues(xx, yy);
    }
    memoCache[stateHash] = result;
    return result;
}