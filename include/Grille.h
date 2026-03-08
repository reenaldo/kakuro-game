#ifndef GRILLE_H
#define GRILLE_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include "Cell.h"
#include "Cell_black.h"
#include "Cell_white.h"
#include "Cell_triangulaire.h"

class Grille {
private:
    int rows, cols;
    std::vector<std::vector<std::unique_ptr<Cell>>> grid;
    static constexpr int MIN_CELL_WIDTH = 5;

    std::string getCellString(int i, int j) const {
        if (grid[static_cast<size_t>(i)][static_cast<size_t>(j)]) {
            return grid[static_cast<size_t>(i)][static_cast<size_t>(j)]->toString();
        }
        return "";
    }

public:
    Grille(int r, int c) : rows(r), cols(c), grid(static_cast<size_t>(r)) {
        for (size_t i = 0; i < static_cast<size_t>(r); ++i) {
            grid[i] = std::vector<std::unique_ptr<Cell>>(static_cast<size_t>(c));
        }
    }

    void setCell(int x, int y, std::unique_ptr<Cell> cell) {
        grid[static_cast<size_t>(x)][static_cast<size_t>(y)] = std::move(cell);
    }

    Cell* getCell(int x, int y) { return grid[static_cast<size_t>(x)][static_cast<size_t>(y)].get(); }
    const Cell* getCell(int x, int y) const { return grid[static_cast<size_t>(x)][static_cast<size_t>(y)].get(); }

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    // Helper to parse a cell string and return a unique_ptr<Cell>
    static std::unique_ptr<Cell> parseCellString(const std::string& cellData, int i, int j) {
        if (cellData == "#") {
            return std::make_unique<Cell_black>(i, j);
        } else if (cellData == "_") {
            return std::make_unique<Cell_white>(i, j);
        } else if (cellData.find('/') != std::string::npos) {
            int val_col = 0, val_row = 0;
            size_t slash = cellData.find('/');
            std::string left = cellData.substr(0, slash);
            std::string right = cellData.substr(slash + 1);
            if (!left.empty()) val_col = std::stoi(left);
            if (!right.empty()) val_row = std::stoi(right);
            // Explicitly reject negative clues
            if (val_col < 0 || val_row < 0) {
                throw std::invalid_argument("Indice invalide à la case [" + std::to_string(i) + "," + std::to_string(j) + "] : les valeurs négatives ne sont pas autorisées pour les indices (col=" + std::to_string(val_col) + ", ligne=" + std::to_string(val_row) + ").");
            }
            if (val_col == 0 && val_row == 0) {
                throw std::invalid_argument("Indice invalide à la case [" + std::to_string(i) + "," + std::to_string(j) + "] : au moins une somme doit être positive (col=" + std::to_string(val_col) + ", ligne=" + std::to_string(val_row) + ").");
            }
            return std::make_unique<Cell_triangulaire>(i, j, val_col, val_row);
        } else if (std::all_of(cellData.begin(), cellData.end(), [](unsigned char c) { return std::isdigit(c); })) {
            long tmp = std::stol(cellData);
            if (tmp < 1 || tmp > 9)
                throw std::out_of_range("Valeur invalide à la case [" + std::to_string(i) + "," + std::to_string(j) + "] : la valeur doit être comprise entre 1 et 9 (" + std::to_string(tmp) + ").");
            int fixedValue = static_cast<int>(tmp);
            auto cell = std::make_unique<Cell_white>(i, j);
            cell->setValue(fixedValue);
            return cell;
        } else {
            throw std::invalid_argument("Jeton de case non reconnu à [" + std::to_string(i) + "," + std::to_string(j) + "] : '" + cellData + "'.");
        }
    }

    // Deep copy (clone) method for Grille
    Grille clone() const {
        Grille copie(rows, cols);
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                if (grid[i][j]) {
                    copie.grid[i][j] = grid[i][j]->clone();
                } else {
                    throw std::runtime_error("clone() : cellule nulle à (" + std::to_string(i) + "," + std::to_string(j) + ")");
                }
            }
        }
        return copie;
    }

    // Returns a formatted string representation of the grid
    std::string format() const {
        int width = MIN_CELL_WIDTH;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::string s = getCellString(i, j);
                width = std::max(width, static_cast<int>(s.size()) + 2);
            }
        }
        std::ostringstream out;
        auto printSep = [&]() {
            out << "+";
            for (int j = 0; j < cols; ++j) out << std::string(static_cast<size_t>(width), '-') << "+";
            out << "\n";
        };
        printSep();
        for (int i = 0; i < rows; ++i) {
            out << "|";
            for (int j = 0; j < cols; ++j) {
                std::string s = getCellString(i, j);
                int pad = width - static_cast<int>(s.size());
                int padL = pad / 2;
                int padR = pad - padL;
                out << std::string(static_cast<size_t>(padL), ' ') << s << std::string(static_cast<size_t>(padR), ' ') << "|";
            }
            out << "\n";
            printSep();
        }
        return out.str();
    }

    void display() const {
        std::cout << format();
    }

    void initializeFromInput() {
        std::string line;
        bool valid = false;
        while (!valid) {
            std::cout << "Entrez les dimensions de la grille (lignes et colonnes) : ";
            std::getline(std::cin >> std::ws, line);
            std::istringstream iss(line);
            int r = 0, c = 0;
            if (iss >> r >> c && iss.eof()) {
                if (r < 3 || c < 3) {
                    std::cout << "Erreur : la taille minimale de la grille est 3x3. Veuillez entrer des dimensions plus grandes." << std::endl;
                    continue;
                }
                rows = r;
                cols = c;
                valid = true;
            } else {
                std::cout << "Format invalide. Veuillez entrer deux entiers positifs séparés par un espace (ex : '5 4')." << std::endl;
            }
        }

        grid.clear();
        grid.resize(static_cast<size_t>(rows));
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            grid[i] = std::vector<std::unique_ptr<Cell>>(static_cast<size_t>(cols));
        }

        std::cout << "Saisissez les données de la grille ligne par ligne (utilisez # pour les cases noires, _ pour les cases vides, et B/D pour les cases de sommes) :\n";
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                std::string cellData;
                bool ok = false;
                while (!ok) {
                    std::cout << "Case["<<i<<","<<j<<"] : ";
                    std::cin >> cellData;
                    try {
                        grid[i][j] = parseCellString(cellData, static_cast<int>(i), static_cast<int>(j));
                        ok = true;
                    } catch (const std::exception& e) {
                        std::cerr << "Entrée invalide pour la case ["<<i<<","<<j<<"] ('"<< cellData << "') : "
                                  << e.what() << ". Veuillez réessayer." << std::endl;
                    }
                }
            }
        }
    }

    // Validates the structure of the grid to ensure clues are followed by valid cells
    bool validateStructure() const {
        // First, check that all clues are followed by at least one white cell (existing logic)
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                const Cell* cell = grid[static_cast<size_t>(i)][static_cast<size_t>(j)].get();
                if (cell && cell->isMaster()) {
                    if (cell->getValRow() > 0) {
                        bool found = false;
                        for (int y = j + 1; y < cols; ++y) {
                            if (!grid[static_cast<size_t>(i)][static_cast<size_t>(y)]) break;
                            if (grid[static_cast<size_t>(i)][static_cast<size_t>(y)]->isWhite()) { found = true; break; }
                            if (grid[static_cast<size_t>(i)][static_cast<size_t>(y)]->isMaster() || !grid[static_cast<size_t>(i)][static_cast<size_t>(y)]->isWhite()) break;
                        }
                        if (!found) {
                            std::cerr << "Indice de ligne orphelin ou invalide à la case (" << i << "," << j << ") avec la valeur : row=" << cell->getValRow() << ", au moins une case blanche attendue à droite." << std::endl;
                            return false;
                        }
                    }
                    if (cell->getValCol() > 0) {
                        bool found = false;
                        for (int x = i + 1; x < rows; ++x) {
                            if (!grid[static_cast<size_t>(x)][static_cast<size_t>(j)]) break;
                            if (grid[static_cast<size_t>(x)][static_cast<size_t>(j)]->isWhite()) { found = true; break; }
                            if (grid[static_cast<size_t>(x)][static_cast<size_t>(j)]->isMaster() || !grid[static_cast<size_t>(x)][static_cast<size_t>(j)]->isWhite()) break;
                        }
                        if (!found) {
                            std::cerr << "Indice de colonne orphelin ou invalide à la case (" << i << "," << j << ") avec la valeur : col=" << cell->getValCol() << ", au moins une case blanche attendue en dessous." << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        // Precompute run validity for each white cell
        std::vector<std::vector<bool>> rowRunValid(static_cast<size_t>(rows), std::vector<bool>(static_cast<size_t>(cols), false));
        std::vector<std::vector<bool>> colRunValid(static_cast<size_t>(rows), std::vector<bool>(static_cast<size_t>(cols), false));
        // Row runs
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            bool inRun = false;
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                const Cell* cell = grid[i][j].get();
                if (cell && cell->isMaster() && cell->getValRow() > 0) {
                    inRun = true;
                } else if (cell && cell->isWhite() && inRun) {
                    rowRunValid[i][j] = true;
                } else if (!cell || (!cell->isWhite() && !cell->isMaster())) {
                    inRun = false;
                }
            }
        }
        // Column runs
        for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
            bool inRun = false;
            for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
                const Cell* cell = grid[i][j].get();
                if (cell && cell->isMaster() && cell->getValCol() > 0) {
                    inRun = true;
                } else if (cell && cell->isWhite() && inRun) {
                    colRunValid[i][j] = true;
                } else if (!cell || (!cell->isWhite() && !cell->isMaster())) {
                    inRun = false;
                }
            }
        }
        // Now, ensure every white cell is part of both a valid row and column run
        for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
            for (size_t j = 0; j < static_cast<size_t>(cols); ++j) {
                const Cell* cell = grid[i][j].get();
                if (cell && cell->isWhite()) {
                    if (!rowRunValid[i][j] || !colRunValid[i][j]) {
                        std::cerr << "Case blanche orpheline à (" << i << "," << j << ") - n'appartient pas à une ligne et une colonne valides." << std::endl;
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

#endif