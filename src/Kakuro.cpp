/**
 * @file Kakuro.cpp
 * @brief Point d'entrée principal du solveur de Kakuro.
 *
 * Ce programme permet à l'utilisateur de charger une grille de Kakuro depuis différentes sources (saisie manuelle, fichier texte, fichier JSON, base de données),
 * de la résoudre à l'aide de la classe Solver, et d'afficher la solution. Il gère également les interactions utilisateur et les erreurs courantes.
 */

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <filesystem>
#include "Grille.h"
#include "Solver.h"
#include "GrilleLoaderFactory.h"
#include "Colors.h"
using Colors::BOLD;
using Colors::CYAN;
using Colors::RESET;
using Colors::YELLOW;
using Colors::RED;
using Colors::BLUE;
using Colors::GREEN;

int main(int argc, char* argv[]) {
    bool quiet = false;
    bool useColor = true;
    bool deterministic = true; // toujours déterministe
    std::vector<std::string> args(argv+1, argv+argc);
    // ignorer le drapeau --deterministic, toujours déterministe
    for (auto& a : args) {
        if (a == "-q") quiet = true;
        if (a == "--no-color") useColor = false;
    }

    int playCount = 0;
    int retryCount = 0;
    const int maxRetries = 3;
    std::string initialSource;
    if (argc > 1) {
        // ignorer les drapeaux
        for (auto& a : args) if (a != "-q" && a != "--no-color") initialSource = a;
    }

    do {
        if (retryCount >= maxRetries) {
            std::cerr << BOLD << RED << "Trop de tentatives invalides. Fermeture du programme." << RESET << std::endl;
            return 1;
        }
        if (!quiet) {
            if (useColor) std::cout << BOLD << CYAN << "Bienvenue dans le solveur de Kakuro !" << RESET << std::endl;
            else std::cout << "Bienvenue dans le solveur de Kakuro !" << std::endl;
        }
        std::string source;
        bool showMenu = true;
        while (true) {
            if (playCount == 0 && !initialSource.empty()) {
                source = initialSource;
            } else {
                if (!quiet && showMenu) {
                    std::cout << BOLD << YELLOW
                        << "Veuillez sélectionner une source d'entrée :\n"
                        << "  - Tapez 'input' pour saisir une grille manuellement\n"
                        << "  - Entrez un nom de fichier (se terminant par .txt ou .json) pour charger une grille depuis un fichier\n"
                        << "  - Tapez 'db' pour charger une grille depuis la base de données\n"
                        << "  - Tapez 'quit' pour quitter le programme\n"
                        << "Votre choix : " << RESET;
                } else if (!quiet && !showMenu) {
                    std::cout << BOLD << YELLOW << "Votre choix : " << RESET;
                }
                std::cin >> source;
            }
            std::string sourceLower = source;
            for (auto& c : sourceLower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (sourceLower == "quit" || sourceLower == "exit") return 0;
            if (sourceLower == "input") break;
            if (sourceLower == "db") {
                /**
                 * Gestion de la sélection de grille.
                 * Affiche la liste des grilles disponibles selon le type choisi (txt/json),
                 * puis permet à l'utilisateur de sélectionner une grille par numéro ou nom.
                 */
                // Demander à l'utilisateur de choisir entre txt ou json
                std::string typeChoice;
                while (true) {
                    std::cout << BOLD << YELLOW << "Choisissez le type de fichier ('txt' ou 'json') : " << RESET;
                    std::cin >> typeChoice;
                    for (auto& c : typeChoice) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    if (typeChoice == "txt" || typeChoice == "json") break;
                    std::cerr << BOLD << RED << "Type invalide. Veuillez entrer 'txt' ou 'json'." << RESET << std::endl;
                }
                // Lister les fichiers du type choisi
                std::vector<std::string> dbFiles;
                if (typeChoice == "json") {
                    dbFiles = {
                        "ressource/json/12_10_facile.json",
                        "ressource/json/5_5_intermediaire.json",
                        "ressource/json/9_9_intermediaire.json",
                        "ressource/json/13_13_medium.json",
                        "ressource/json/18_10_difficile.json",
                        "ressource/json/5_4.json",
                        "ressource/json/5_5_expert.json",
                        "ressource/json/7_7_difficile.json",
                        "ressource/json/7_7_intermediaire.json",
                        "ressource/json/sample.json"
                    };
                } else {
                    dbFiles = {
                        "ressource/txt/12_10_facile.txt",
                        "ressource/txt/5_5_intermediaire.txt",
                        "ressource/txt/9_9_intermediaire.txt",
                        "ressource/txt/13_13_medium.txt",
                        "ressource/txt/18_10_difficile.txt",
                        "ressource/txt/5_4.txt",
                        "ressource/txt/5_5_expert.txt",
                        "ressource/txt/7_7_difficile.txt",
                        "ressource/txt/7_7_intermediaire.txt",
                        "ressource/txt/sample.txt"
                    };
                }
                if (!quiet) {
                    /**
                     * Affichage de la liste des grilles disponibles dans la base de données.
                     * Affiche le nom de chaque fichier avec un numéro d'index pour la sélection.
                     */
                    std::cout << BOLD << CYAN << "Grilles " << typeChoice << " disponibles dans la base de données :" << RESET << std::endl;
                    for (size_t i = 0; i < dbFiles.size(); ++i) {
                        size_t pos = dbFiles[i].find_last_of("/");
                        std::string fname = (pos != std::string::npos) ? dbFiles[i].substr(pos+1) : dbFiles[i];
                        std::cout << "  [" << (i+1) << "] " << fname << std::endl;
                    }
                    std::cout << "Entrez le numéro ou le nom du fichier de la grille à charger : ";
                }
                std::string dbChoice;
                std::cin >> dbChoice;
                int dbIndex = -1;
                try { dbIndex = std::stoi(dbChoice); } catch (...) {}
                if (dbIndex >= 1 && (size_t)dbIndex <= dbFiles.size()) {
                    /**
                     * L'utilisateur a sélectionné une grille par numéro d'index.
                     * On construit le chemin relatif et on quitte la boucle de sélection.
                     */
                    source = dbFiles[static_cast<size_t>(dbIndex-1)];
                    source = ("./" + source);
                    break;
                } else {
                    /**
                     * Recherche si l'utilisateur a entré un nom de fichier valide.
                     * Si trouvé, on utilise ce fichier comme source.
                     */
                    bool found = false;
                    for (const auto& f : dbFiles) {
                        size_t pos = f.find_last_of("/");
                        std::string fname = (pos != std::string::npos) ? f.substr(pos+1) : f;
                        if (dbChoice == fname) {
                            source = f;
                            source = ("./" + source);
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                    std::cerr << BOLD << RED << "Sélection invalide. Veuillez réessayer." << RESET << std::endl;
                    continue;
                }
            }
            auto dot = source.find_last_of('.');
            std::string ext = (dot != std::string::npos) ? source.substr(dot) : "";
            if (ext == ".txt" || ext == ".json") break;
            if (retryCount < maxRetries - 1) {
                std::cerr << BOLD << RED << "\n🚫 Oups ! Ce n'est pas une option valide." << RESET << std::endl;
                std::cerr << BOLD << YELLOW << "👉 Veuillez choisir l'une des options suivantes :\n"
                          << "   • Tapez 'input' pour saisir une grille manuellement\n"
                          << "   • Entrez un nom de fichier se terminant par .txt ou .json pour charger une grille depuis un fichier\n"
                          << "   • Tapez 'db' pour charger une grille depuis la base de données\n"
                          << "   • Tapez 'quit' pour quitter le programme\n" << RESET << std::endl;
                showMenu = false;
            } else {
                showMenu = true;
            }
            initialSource.clear();
            retryCount++;
            if (retryCount >= maxRetries) break;
        }
        if (retryCount >= maxRetries) {
            std::cerr << BOLD << RED << "Trop de tentatives invalides. Fermeture du programme." << RESET << std::endl;
            return 1;
        }
        playCount++;
        try {
            /**
             * Chargement de la grille, validation, affichage et résolution.
             * Affiche la solution et le temps de résolution, ou un message d'erreur si la grille est invalide ou non résoluble.
             */
            auto loader = GrilleLoaderFactory::createLoader(source);
            Grille grid = loader->load(source);

            if (!grid.validateStructure()) {
                std::cerr << BOLD << RED << "Structure de grille invalide !" << RESET << std::endl;
                initialSource.clear();
                retryCount++;
                continue;
            }

            if (!quiet) {
                std::cout << BOLD << BLUE << "Grille initiale :" << RESET << std::endl;
                grid.display();
            }

            Solver solver(deterministic);
            auto t_start = std::chrono::high_resolution_clock::now();
            bool success = solver.solve(grid);
            auto t_end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(t_end - t_start).count();

            if (success) {
                if (!quiet) {
                    std::cout << BOLD << GREEN << "Grille résolue :" << RESET << std::endl;
                    grid.display();
                    std::cout << BOLD << GREEN << "La solution est valide !" << RESET << std::endl;
                }
            } else {
                if (!quiet) std::cout << BOLD << RED << "Aucune solution trouvée." << RESET << std::endl;
            }
            if (!quiet) {
                std::cout << BOLD << CYAN
                          << "Grille " << grid.getRows() << "x" << grid.getCols() << " résolue en "
                          << std::scientific << elapsed << " secondes" << std::defaultfloat
                          << RESET << std::endl;
            }

            std::cout << "\nRejouer ? (o/n) : ";
            std::string again;
            std::cin >> again;
            if (again != "o" && again != "O") break;
        } catch (const std::exception& e) {
            /**
             * Gestion des exceptions lors du chargement ou de la résolution de la grille.
             * Affiche le message d'erreur à l'utilisateur.
             */
            std::cerr << BOLD << RED << "Exception : " << e.what() << RESET << std::endl;
            initialSource.clear();
            retryCount++;
            continue;
        }
    } while (true);

    return 0;
}