#ifndef CELL_H
#define CELL_H

#include <memory>

/**
 * @class Cell
 * @brief Classe de base abstraite représentant une cellule de la grille de Kakuro.
 *
 * Fournit l'interface commune pour toutes les cellules (noires, blanches, triangulaires).
 * Gère la position, les indicateurs de type et les valeurs d'indice.
 */
class Cell {
protected:
    int posx, posy; ///< Position x et y de la cellule dans la grille.
    bool is_master = false; ///< Indique si la cellule est une cellule maître (indice triangulaire).
    bool is_white = false;  ///< Indique si la cellule est une cellule blanche (modifiable).
    int val_col = -1;      ///< Valeur de l'indice colonne (pour les cellules triangulaires).
    int val_row = -1;      ///< Valeur de l'indice ligne (pour les cellules triangulaires).

public:
    /**
     * @brief Construit une cellule à la position (x, y).
     * @param x Coordonnée x de la cellule.
     * @param y Coordonnée y de la cellule.
     */
    Cell(int x, int y) : posx(x), posy(y) {}
    virtual ~Cell() = default;

    /**
     * @brief Affiche la cellule sur la sortie standard.
     */
    virtual void display() const = 0;
    /**
     * @brief Clone la cellule.
     * @return Un pointeur unique vers la nouvelle cellule clonée.
     */
    virtual std::unique_ptr<Cell> clone() const = 0;
    /**
     * @brief Retourne une représentation textuelle de la cellule.
     * @return Un token (ex: "#", "_", "3", "4/5").
     */
    virtual std::string toString() const = 0;

    /**
     * @brief Indique si la cellule est une cellule maître (indice triangulaire).
     * @return true si la cellule est maître, false sinon.
     */
    bool isMaster() const noexcept { return is_master; }
    /**
     * @brief Indique si la cellule est une cellule blanche (modifiable).
     * @return true si la cellule est blanche, false sinon.
     */
    bool isWhite() const noexcept { return is_white; }
    /**
     * @brief Retourne la valeur de l'indice colonne.
     * @return Valeur de l'indice colonne.
     */
    int getValCol() const noexcept { return val_col; }
    /**
     * @brief Retourne la valeur de l'indice ligne.
     * @return Valeur de l'indice ligne.
     */
    int getValRow() const noexcept { return val_row; }
    /**
     * @brief Définit la valeur de l'indice colonne.
     * @param val Nouvelle valeur de l'indice colonne.
     */
    void setValCol(int val) noexcept { val_col = val; }
    /**
     * @brief Définit la valeur de l'indice ligne.
     * @param val Nouvelle valeur de l'indice ligne.
     */
    void setValRow(int val) noexcept { val_row = val; }
};

#endif