#ifndef CELL_WHITE_H
#define CELL_WHITE_H

#include "Cell.h"

/**
 * @class Cell_white
 * @brief Représente une cellule blanche (modifiable) dans la grille de Kakuro.
 * Hérite de la classe Cell.
 */
class Cell_white : public Cell {
private:
    int value = 0;

public:
    /**
     * @brief Construit une cellule blanche à la position (x, y).
     * @param x Coordonnée x de la cellule.
     * @param y Coordonnée y de la cellule.
     * @throw std::invalid_argument si x ou y est négatif.
     */
    Cell_white(int x, int y) : Cell(x, y) {
        if (x < 0 || y < 0) throw std::invalid_argument("Cell_white : x et y doivent être positifs ou nuls");
        is_white = true;
    }

    /**
     * @brief Définit la valeur de la cellule blanche.
     * @param val La valeur à affecter à la cellule.
     */
    void setValue(int val) noexcept { value = val; }
    /**
     * @brief Retourne la valeur de la cellule blanche.
     * @return La valeur courante de la cellule.
     */
    int getValue() const noexcept { return value; }

    /**
     * @brief Clone la cellule blanche.
     * @return Un pointeur unique vers la nouvelle cellule blanche clonée.
     */
    std::unique_ptr<Cell> clone() const override {
        auto c = std::make_unique<Cell_white>(posx, posy);
        c->setValue(value);
        return c;
    }

    /**
     * @brief Affiche la cellule blanche sur la sortie standard.
     */
    void display() const noexcept override {
        if (value == 0) {
            std::cout << "_";
        } else {
            std::cout << value;
        }
    }

    /**
     * @brief Retourne une représentation textuelle de la cellule blanche.
     * @return "_" si la cellule est vide, sinon la valeur sous forme de chaîne.
     */
    std::string toString() const noexcept override {
        if (value == 0) return "_";
        return std::to_string(value);
    }
};

#endif