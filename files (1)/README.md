# 🖥️ Simulateur de CPU en C

> Implémentation d'une version simplifiée d'un processeur en langage C, avec gestion de la mémoire, parseur d'assembleur et plusieurs modes d'adressage.

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build](https://img.shields.io/badge/build-make-green.svg)](https://www.gnu.org/software/make/)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg)](#-licence)

---

## 📋 Table des matières

- [Description du projet](#-description-du-projet)
- [Structure du projet](#-structure-du-projet)
- [Installation et compilation](#-installation-et-compilation)
- [Utilisation](#-utilisation)
- [Architecture du code](#-architecture-du-code)
- [Détail des exercices](#-détail-des-exercices)
- [Performances](#-performances)
- [Documentation](#-documentation)
- [Auteurs](#-auteurs)
- [Licence](#-licence)

---

## 🎯 Description du projet

Ce projet a pour objectif de créer une **version simplifiée d'un CPU** en langage C, en se concentrant sur un nombre réduit de registres et de modes d'adressage, tout en conservant les principes fondamentaux des instructions et de la gestion mémoire.

Il simule :

- 🧠 Une **mémoire virtuelle** segmentée et gérée dynamiquement
- 📥 Un **parseur** capable de lire un pseudo-assembleur (`.DATA`, `.CODE`, labels…)
- ⚙️ Un **CPU** avec registres (`AX`, `BX`, `CX`, `DX`) et plusieurs modes d'adressage
- 📚 Une **pile** (`push`/`pop`) et la gestion d'un segment supplémentaire `ES`

---

## 📁 Structure du projet

```
cpu-simulator/
│
├── 📄 README.md            # Ce fichier
├── 📄 Makefile             # Configuration de compilation
├── 📄 .gitignore           # Fichiers ignorés par Git
│
├── 📂 src/                 # Code source (.c)
│   ├── main.c              # Jeux de tests des 8 exercices
│   ├── soutenance.c        # Programme principal (point d'entrée)
│   ├── ex1.c               # Table de hachage
│   ├── ex2.c               # Gestionnaire de mémoire
│   ├── ex3.c               # Parseur de pseudo-assembleur
│   ├── ex4.c               # Structure CPU
│   ├── ex5.c               # Modes d'adressage
│   ├── ex6.c               # Exécution d'instructions
│   ├── ex7.c               # Pile (push/pop)
│   └── ex8.c               # Segment ES (alloc/free)
│
├── 📂 include/             # Fichiers d'en-tête (.h)
│   ├── ex1.h               # ↳ HashMap, HashEntry
│   ├── ex2.h               # ↳ Segment, MemoryHandler
│   ├── ex3.h               # ↳ Instruction, ParserResult
│   ├── ex4.h               # ↳ CPU
│   ├── ex5.h               # ↳ Adressage
│   ├── ex6.h               # ↳ Exécution
│   ├── ex7.h               # ↳ Pile
│   └── ex8.h               # ↳ Segment ES
│
├── 📂 tests/               # Fichiers de test
│   └── parser.tkt          # Exemple de programme pseudo-assembleur
│
└── 📂 docs/                # Documentation
    └── Rapport.pdf         # Rapport détaillé du projet
```

---

## 🚀 Installation et compilation

### Prérequis

- **GCC** (ou tout compilateur C compatible)
- **GNU Make**
- Un système **Unix-like** (Linux, macOS, WSL)

### Étapes

```bash
# 1. Cloner le dépôt
git clone https://github.com/<votre-utilisateur>/cpu-simulator.git
cd cpu-simulator

# 2. Compiler le projet
make

# 3. Exécuter le programme
./tme
```

### Commandes Make disponibles

| Commande      | Description                                              |
| ------------- | -------------------------------------------------------- |
| `make`        | Compile le projet (équivalent à `make all`)              |
| `make run`    | Compile puis exécute le programme                        |
| `make clean`  | Supprime les fichiers objets et l'exécutable             |

---

## 💻 Utilisation

Une fois compilé, lancez simplement :

```bash
./tme
```

Le programme exécute `soutenance.c`, qui teste la création de plusieurs segments mémoire et affiche leur état avant et après suppression.

### Exemple de pseudo-assembleur

Le fichier `tests/parser.tkt` montre la syntaxe acceptée par le parseur :

```asm
.DATA
x DW 42
arr DB 20,21,22,23
y DB 10
.CODE
start: MOV AX, x
loop:  ADD AX, y
       JMP loop
```

> 📝 **Note** : Le fichier `src/main.c` contient un ensemble exhaustif de tests pour les 8 exercices, actuellement encadré par des commentaires `/* ... */`. Pour les activer, ouvrez `src/main.c` et retirez les commentaires d'ouverture et de fermeture (ainsi que la fonction `main` de `soutenance.c` pour éviter le conflit).

---

## 🏗️ Architecture du code

Le projet est organisé autour de **8 modules** (`ex1` → `ex8`), chacun bâti sur les précédents :

```
ex1 (HashMap)
 └─► ex2 (MemoryHandler) ──┐
                            ├─► ex4 (CPU) ──► ex5 (Adressage) ──► ex6 (Exécution)
ex3 (Parseur) ─────────────┘                                       │
                                                                    ├─► ex7 (Pile)
                                                                    └─► ex8 (Segment ES)
```

### Principales structures de données

| Structure       | Rôle                                                                         |
| --------------- | ---------------------------------------------------------------------------- |
| `HashEntry`     | Une case d'une table de hachage : associe une clé à une valeur générique     |
| `HashMap`       | Table de hachage `clé → valeur` (sondage linéaire)                            |
| `Segment`       | Bloc mémoire avec position de départ, taille et pointeur vers le suivant     |
| `MemoryHandler` | Gestionnaire mémoire : tableau, liste libre, table des segments alloués     |
| `Instruction`   | Une instruction pseudo-assembleur (`mnemonic`, `operand1`, `operand2`)       |
| `ParserResult`  | Sortie de l'analyseur : instructions `.DATA`, `.CODE`, labels, adresses     |
| `CPU`           | Simulateur du processeur : mémoire, registres, table des constantes          |

---

## 📚 Détail des exercices

### **EX1** — Table de hachage
Table de hachage à taille fixe (`TABLESIZE = 12`) avec **sondage linéaire** pour la résolution des collisions.

- `hashmap_create()` — Θ(1)
- `hashmap_insert()` — Ω(1), O(tablesize)
- `hashmap_get()` — Ω(1), O(tablesize)
- `hashmap_remove()` — Θ(tablesize)
- `hashmap_destroy()` — Θ(tablesize)

### **EX2** — Gestionnaire de mémoire
Implémente une mémoire segmentée avec liste chaînée des segments libres et table des segments alloués.

- `memory_init(size)` — Θ(size)
- `find_free_segment()` — Θ(n)
- `create_segment()` — Θ(1)
- `remove_segment()` — Ω(1), O(n)

### **EX3** — Parseur de pseudo-assembleur
Parseur lisant des fichiers `.tkt` contenant des sections `.DATA` et `.CODE`, avec gestion des labels.

- `parse(filename)` — O(n)

### **EX4** — Structure CPU
Définit la structure `CPU` avec son gestionnaire mémoire, ses registres (`AX`, `BX`, `CX`, `DX`) et sa table de constantes.

- `load()` / `store()` — Ω(1), O(tablesize)

### **EX5** — Modes d'adressage
Implémente plusieurs modes d'adressage : immédiat, registre, mémoire direct, indirect par registre.

- `resolve_addressing()` — Θ(1)

### **EX6** — Exécution d'instructions
Boucle d'exécution principale (`fetch-decode-execute`).

- `handle_instruction()` — Ω(1), O(tablesize)
- `run_program()` — O(n)

### **EX7** — Pile
Implémente les opérations `push` et `pop` sur la pile.

- `push_value()` — Ω(1), O(tablesize)
- `pop_value()` — Ω(1), O(n)

### **EX8** — Segment ES
Allocation et libération d'un segment supplémentaire `ES` avec stratégies **First Fit** et **Best Fit**.

- `alloc_es_segment()` — Ω(n), O(n²)
- `free_es_segment()` — Ω(n), O(n²)

---

## 📊 Performances

Mesures réalisées avec `/usr/bin/time -v ./tme` sur **30 essais** :

| Métrique             | Valeur                |
| -------------------- | --------------------- |
| ⏱️ Temps moyen        | **2,68 ± 0,01 s**      |
| 💾 Mémoire utilisée  | **~1,8 Mo** (stable)   |
| 🖥️ Utilisation CPU    | Quasi nulle           |

---

## 📖 Documentation

Un rapport détaillé du projet est disponible dans [`docs/Rapport.pdf`](docs/Rapport.pdf). Il contient :

- La reformulation du sujet
- La description complète des structures utilisées
- La complexité de chaque fonction
- La description des jeux de tests
- L'analyse de performance

---

## 👥 Auteurs

- **Clément Jiang**
- **Cidalia De Araujo**

---

## 📜 Licence

Ce projet est distribué sous licence **MIT**. Vous êtes libre de l'utiliser, le modifier et le redistribuer.

---

<p align="center">
  Fait avec ❤️ et beaucoup de <code>malloc()</code>
</p>
