#ifndef GRILLE_LOADER_FACTORY_H
#define GRILLE_LOADER_FACTORY_H

#include <memory>
#include <algorithm>
#include <fstream>
#include "IGrilleLoader.h"
#include "Cell_black.h"
#include "Cell_white.h"
#include "Cell_triangulaire.h"
#include "GrilleLoaderJSON.h"

/**
 * @file GrilleLoaderFactory.h
 * @brief Déclarations des classes de chargement de grilles Kakuro.
 *
 * Définit les interfaces et les classes pour charger des grilles Kakuro à partir de différentes sources :
 * - Saisie utilisateur (UserInputLoader)
 * - Fichier texte (TextFileLoader)
 * - Fichier JSON (GrilleLoaderJSON)
 * Fournit également une fabrique (GrilleLoaderFactory) pour sélectionner dynamiquement le bon chargeur.
 */

/**
 * @class UserInputLoader
 * @brief Loader for grids entered by the user via standard input.
 *
 * Implements IGrilleLoader to allow interactive grid entry.
 */
class UserInputLoader : public IGrilleLoader {
public:
    /**
     * @brief Loads a grid from user input.
     * @param source Ignored parameter.
     * @return The loaded Grille object.
     */
    Grille load(const std::string&) override {
        Grille grid(0, 0);
        grid.initializeFromInput();
        return grid;
    }
};

/**
 * @class TextFileLoader
 * @brief Loader for grids from plain text files.
 *
 * Implements IGrilleLoader to load a grid from a text file with a specific format.
 */
class TextFileLoader : public IGrilleLoader {
public:
    /**
     * @brief Loads a grid from a text file.
     * @param source Path to the text file.
     * @return The loaded Grille object.
     * @throws std::runtime_error if the file cannot be opened or is malformed.
     */
    Grille load(const std::string& source) override {
        std::ifstream infile(source);
        if (!infile) {
            throw std::runtime_error("Impossible d'ouvrir le fichier : " + source);
        }
        int rows, cols;
        if (!(infile >> rows >> cols)) {
            throw std::runtime_error("Le fichier doit commencer par deux entiers : lignes colonnes");
        }
        if (rows < 1 || cols < 1) {
            throw std::runtime_error("Les dimensions de la grille doivent être positives");
        }
        Grille grid(rows, cols);
        std::string cellData;
        int cellCount = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (!(infile >> cellData)) {
                    throw std::runtime_error("Le fichier s'est terminé prématurément : plus de données attendues pour la case (" + std::to_string(i) + "," + std::to_string(j) + ")");
                }
                try {
                    grid.setCell(i, j, Grille::parseCellString(cellData, i, j));
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string("Donnée de case invalide à (") + std::to_string(i) + "," + std::to_string(j) + ") : " + cellData + ". Raison : " + e.what());
                }
                ++cellCount;
            }
        }
        // Vérifier les données supplémentaires
        std::string extra;
        if (infile >> extra) {
            throw std::runtime_error("Le fichier contient des données supplémentaires après les " + std::to_string(rows * cols) + " cases attendues (première donnée en trop : '" + extra + "')");
        }
        if (cellCount != rows * cols) {
            throw std::runtime_error("Le fichier ne contient pas exactement lignes*colonnes entrées de cases");
        }
        return grid;
    }
};

/**
 * @class GrilleLoaderFactory
 * @brief Factory for creating appropriate grid loaders based on the source.
 *
 * Provides a static method to create a loader for user input, JSON, or text files.
 */
class GrilleLoaderFactory {
public:
    /**
     * @brief Creates a loader based on the source type.
     * @param source The source identifier or file path.
     * @return A unique_ptr to an IGrilleLoader.
     * @throws std::runtime_error if the source cannot be opened or is unknown.
     */
    static std::unique_ptr<IGrilleLoader> createLoader(const std::string& source) {
        if (source == "input") {
            return std::make_unique<UserInputLoader>();
        }
        // Case-insensitive check for JSON file extension
        if (source.size() >= 5) {
            std::string ext = source.substr(source.find_last_of(".") + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == "json") {
                std::ifstream infile(source);
                if (infile) {
                    return std::make_unique<GrilleLoaderJSON>();
                }
            }
        }
        // Attempt to open the source as a text file
        std::ifstream infile(source);
        if (infile) {
            return std::make_unique<TextFileLoader>();
        }
        throw std::runtime_error("Cannot open file or unknown source: " + source);
    }
};

#endif // GRILLE_LOADER_FACTORY_H