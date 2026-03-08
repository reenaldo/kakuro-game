#ifndef CELL_BLACK_H
#define CELL_BLACK_H

#include "Cell.h"

/**
 * @class Cell_black
 * @brief Représente une cellule noire dans la grille de Kakuro.
 * Hérite de la classe Cell.
 */
class Cell_black : public Cell {
public:
    /**
     * @brief Construit une cellule noire à la position (x, y).
     * @param x Coordonnée x de la cellule.
     * @param y Coordonnée y de la cellule.
     * @throw std::invalid_argument si x ou y est négatif.
     */
    Cell_black(int x, int y) : Cell(x, y) {
        if (x < 0 || y < 0) throw std::invalid_argument("Cellule noire : x et y doivent être positifs ou nuls");
        is_master = false;
    }

    /**
     * @brief Affiche la cellule noire sur la sortie standard.
     */
    void display() const noexcept override {
        std::cout << "#";
    }

    /**
     * @brief Clone la cellule noire.
     * @return Un pointeur unique vers la nouvelle cellule noire clonée.
     */
    std::unique_ptr<Cell> clone() const override {
        return std::make_unique<Cell_black>(posx, posy);
    }
    /**
     * @brief Retourne une représentation textuelle de la cellule noire.
     * @return "#"
     */
    std::string toString() const noexcept override { return "#"; }
};

#endif