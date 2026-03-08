# Solveur de Kakuro

Ce projet fournit un solveur de grilles de Kakuro, développé en C++ et conçu pour être :

- **Modulaire** : prise en charge de plusieurs formats de grille (texte, JSON) via un patron _Factory_.
- **Robuste** : algorithme de backtracking optimisé par des heuristiques de propagation de contraintes.
- **Ergonomique** : interface en ligne de commande simple à utiliser.

---

## Prérequis

- GNU Make
- Un compilateur C++17 (g++, clang++)
- [nlohmann/json](https://github.com/nlohmann/json) (pour le parsing JSON)
- Linux ou macOS (tests validés sous Ubuntu 20.04 / macOS 11)

---

## Installation

```bash
# Cloner le dépôt
git clone <url-du-repo>
cd kakuro-game

# Compiler
make
```

Cela génère l’exécutable `kakuro` à la racine du projet.

---

## Utilisation

### Lancement du programme

Compilez puis lancez simplement le programme :

```bash
./kakuro
```

Vous pouvez ajouter des options :

- `--no-color` : désactive les couleurs dans la sortie

Vous pouvez aussi fournir directement un nom de fichier (ou une option spéciale) :

```bash
./kakuro ressource/json/9_9_intermediaire.json
```

---

### Menu interactif

Au lancement (sans argument ou avec un fichier non valide), un menu s’affiche :

```
Bienvenue dans le solveur de Kakuro !
Veuillez sélectionner une source d'entrée :
  - Tapez 'input' pour saisir une grille manuellement
  - Entrez un nom de fichier (se terminant par .txt ou .json) pour charger une grille depuis un fichier
  - Tapez 'db' pour charger une grille depuis la base de données
  - Tapez 'quit' pour quitter le programme
Votre choix :
```

- `input` : saisie manuelle d’une grille.
- `<fichier>.txt` ou `<fichier>.json` : charge une grille depuis le dossier `ressource/txt/` ou `ressource/json/`.
- `db` : permet de choisir une grille parmi celles proposées (après avoir choisi le type `txt` ou `json`).
- `quit` : quitte le programme.

Après la résolution, la solution s’affiche (ou un message d’erreur), puis il est proposé de rejouer :

```
Rejouer ? (o/n) :
```

---

### Mode non interactif

Vous pouvez lancer directement avec un fichier en argument :

```bash
./kakuro ressource/json/9_9_intermediaire.json
```

Le programme chargera et résoudra la grille, puis proposera de rejouer.

---

## Fichiers de test

Le dossier `ressource/` contient les grilles de test suivantes :

| Nom de fichier         | Format | Taille | Niveau        |
| ---------------------- | ------ | ------ | ------------- |
| 5_5_intermediaire.txt  | texte  | 5×5    | intermédiaire |
| 12_10_facile.json      | JSON   | 12×10  | facile        |
| 9_9_intermediaire.json | JSON   | 9×9    | intermédiaire |
| 13_13_medium.json      | JSON   | 13×13  | moyen         |
| 18_10_difficile.json   | JSON   | 18×10  | difficile     |
| 7_7_difficile.txt      | texte  | 7×7    | difficile     |

Pour ajouter vos propres grilles, placez-les dans `ressource/txt/` ou `ressource/json/` en respectant l’extension et le format.

---

## Organisation du code

```
.
├── ressource/                 # grilles de test (.txt, .json)
│   ├── txt/
│   └── json/
├── src/                       # code source (.cpp)
│   ├── Cell.h, Cell_black.h, Cell_white.h, Cell_triangulaire.h
│   ├── Grille.h, IGrilleLoader.h
│   ├── GrilleLoaderJSON.cpp/.h
│   ├── GrilleLoaderFactory.h
│   ├── Solver.h, Solver.cpp
│   └── Kakuro.cpp             # interface CLI principale
├── include/                   # headers (.h)
├── lib/                       # bibliothèques tierces (json)
├── Makefile
└── README.md
```

---

## Licence

Ce projet est distribué sous licence MIT.
Voir le fichier LICENSE pour plus de détails.

---

## Remerciements

Merci à Frey Gabriel et Haas Antoine pour leur encadrement et leurs conseils tout au long de ce projet.
Pour toute question ou suggestion : trashi.renaldo@unistra.fr.
