#pragma once
#include "IGrilleLoader.h"
#include <string>
#include "Grille.h"

/**
 * @class GrilleLoaderJSON
 * @brief Loader for Kakuro grids from JSON files.
 *
 * Implements IGrilleLoader to load a grid from a JSON file using the nlohmann::json library.
 */
class GrilleLoaderJSON : public IGrilleLoader {
public:
    /**
     * @brief Loads a grid from a JSON file.
     * @param filename Path to the JSON file.
     * @return The loaded Grille object.
     * @throws std::runtime_error if the file cannot be opened or is malformed.
     */
    Grille load(const std::string& filename) override;
};
