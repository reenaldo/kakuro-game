#ifndef COLORS_H
#define COLORS_H

/**
 * @namespace Colors
 * @brief Définit des constantes pour les codes de couleurs ANSI utilisés dans la console.
 *
 * Utilisez ces constantes pour colorer le texte dans la sortie standard.
 * Exemple : std::cout << Colors::RED << "Texte rouge" << Colors::RESET;
 */
namespace Colors {
    /**
     * @brief Réinitialise la couleur et le style du texte.
     */
    constexpr const char* RESET   = "\033[0m";
    /**
     * @brief Met le texte en gras.
     */
    constexpr const char* BOLD    = "\033[1m";
    /**
     * @brief Texte rouge.
     */
    constexpr const char* RED     = "\033[31m";
    /**
     * @brief Texte vert.
     */
    constexpr const char* GREEN   = "\033[32m";
    /**
     * @brief Texte jaune.
     */
    constexpr const char* YELLOW  = "\033[33m";
    /**
     * @brief Texte bleu.
     */
    constexpr const char* BLUE    = "\033[34m";
    /**
     * @brief Texte magenta.
     */
    constexpr const char* MAGENTA = "\033[35m";
    /**
     * @brief Texte cyan.
     */
    constexpr const char* CYAN    = "\033[36m";
    /**
     * @brief Texte blanc.
     */
    constexpr const char* WHITE   = "\033[37m";
    /**
     * @brief Fond noir.
     */
    constexpr const char* BG_BLACK = "\033[40m";
}

#endif // COLORS_H