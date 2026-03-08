#ifndef CELL_TRIANGULAIRE_H
#define CELL_TRIANGULAIRE_H

#include "Cell.h"

/**
 * @class Cell_triangulaire
 * @brief Représente une cellule triangulaire (indice) dans la grille de Kakuro.
 * Hérite de la classe Cell.
 */
class Cell_triangulaire : public Cell {
public:
    /**
     * @brief Construit une cellule triangulaire à la position (x, y) avec des indices de colonne et de ligne.
     * @param x Coordonnée x de la cellule.
     * @param y Coordonnée y de la cellule.
     * @param val_col Indice de la colonne (somme à réaliser dans la colonne).
     * @param val_row Indice de la ligne (somme à réaliser dans la ligne).
     * @throw std::invalid_argument si val_col ou val_row est négatif, ou si les deux sont nuls.
     */
    Cell_triangulaire(int x, int y, int val_col, int val_row)
        : Cell(x, y) {
        if (val_col < 0 || val_row < 0) {
            throw std::invalid_argument("val_col et val_row doivent être positifs ou nuls");
        }
        if (val_col == 0 && val_row == 0) {
            throw std::invalid_argument("Au moins l'un des indices val_col ou val_row doit être non nul");
        }
        this->val_col = val_col;
        this->val_row = val_row;
        is_master = true;
    }

    /**
     * @brief Affiche la cellule triangulaire sur la sortie standard.
     */
    void display() const noexcept override {
        if (val_col > 0 && val_row > 0)
            std::cout << val_col << "/" << val_row;
        else if (val_col > 0)
            std::cout << val_col << "/";
        else if (val_row > 0)
            std::cout << "/" << val_row;
        else
            std::cout << "/";
    }

    /**
     * @brief Clone la cellule triangulaire.
     * @return Un pointeur unique vers la nouvelle cellule triangulaire clonée.
     */
    std::unique_ptr<Cell> clone() const override {
        return std::make_unique<Cell_triangulaire>(posx, posy, val_col, val_row);
    }
    /**
     * @brief Retourne une représentation textuelle de la cellule triangulaire.
     * @return Une chaîne de caractères représentant les indices colonne/ligne.
     */
    std::string toString() const noexcept override {
        if (val_col > 0 && val_row > 0) return std::to_string(val_col) + "/" + std::to_string(val_row);
        if (val_col > 0) return std::to_string(val_col) + "/";
        if (val_row > 0) return "/" + std::to_string(val_row);
        return "/";
    }
};

#endif