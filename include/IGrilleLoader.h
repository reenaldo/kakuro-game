#ifndef IGRILLELOADER_H
#define IGRILLELOADER_H

#include <string>
#include "Grille.h"

/**
 * @interface IGrilleLoader
 * @brief Interface pour les chargeurs de grilles Kakuro.
 *
 * Définit la méthode à implémenter pour charger une grille à partir d'une source donnée (fichier, saisie utilisateur, etc).
 */
class IGrilleLoader {
public:
    /**
     * @brief Charge une grille à partir d'une source.
     * @param source Chemin du fichier ou autre source de données.
     * @return Grille chargée.
     * @throw std::runtime_error en cas d'échec du chargement.
     */
    virtual Grille load(const std::string& source) = 0;
    virtual ~IGrilleLoader() = default;
};

#endif // IGRILLELOADER_H