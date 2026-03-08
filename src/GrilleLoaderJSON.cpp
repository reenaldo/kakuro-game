#include "GrilleLoaderJSON.h"
#include <fstream>
#include "nlohmann/json.hpp"
#include "Cell.h"
#include "Cell_white.h"
#include "Cell_black.h"
#include "Cell_triangulaire.h"

using json = nlohmann::json;

/**
 * @brief Loads a grid from a JSON file.
 *
 * Opens the specified JSON file, parses its content, and constructs a Grille object.
 * Performs validation on the JSON structure and cell data. Throws std::runtime_error on errors.
 *
 * @param filename Path to the JSON file.
 * @return The loaded Grille object.
 * @throws std::runtime_error if the file cannot be opened, parsed, or is malformed.
 */
Grille GrilleLoaderJSON::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Impossible d'ouvrir le fichier JSON : " + filename);
    // Activer les exceptions pour les erreurs d'E/S
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erreur lors de l'analyse du fichier JSON '") + filename + "' : " + e.what());
    }
    // Gestion des erreurs pour les clés requises
    if (!j.contains("rows")) throw std::runtime_error("Le JSON ne contient pas la clé requise : rows");
    if (!j.contains("cols")) throw std::runtime_error("Le JSON ne contient pas la clé requise : cols");
    if (!j.contains("cells")) throw std::runtime_error("Le JSON ne contient pas la clé requise : cells");
    int rows = j.at("rows").get<int>();
    int cols = j.at("cols").get<int>();
    Grille grille(rows, cols);
    const auto& cells = j.at("cells");
    // Validation supplémentaire de la structure des cellules
    if (!cells.is_array() || cells.size() != static_cast<size_t>(rows)) {
        throw std::runtime_error("Le champ 'cells' du JSON doit être un tableau de longueur 'rows'");
    }
    for (size_t i = 0; i < static_cast<size_t>(rows); ++i) {
        if (!cells[i].is_array() || cells[i].size() != static_cast<size_t>(cols)) {
            throw std::runtime_error("Le champ 'cells[" + std::to_string(i) + "]' du JSON doit être un tableau de longueur 'cols'");
        }
        for (size_t j2 = 0; j2 < static_cast<size_t>(cols); ++j2) {
            if (!cells[i][j2].is_string()) {
                throw std::runtime_error("Le champ 'cells[" + std::to_string(i) + "][" + std::to_string(j2) + "]' du JSON doit être une chaîne de caractères");
            }
            std::string cellData = cells[i][j2];
            // La validation des indices négatifs est effectuée dans Grille::parseCellString
            try {
                grille.setCell(static_cast<int>(i), static_cast<int>(j2), Grille::parseCellString(cellData, static_cast<int>(i), static_cast<int>(j2)));
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Donnée de case invalide à (") + std::to_string(i) + "," + std::to_string(j2) + ") : " + cellData + ". Raison : " + e.what());
            }
        }
    }
    return grille;
}
