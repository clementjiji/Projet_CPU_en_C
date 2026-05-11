# ============================================================
# Makefile - Simulateur de CPU
# ============================================================

# Compilateur et options
CC      = gcc
CFLAGS  = -Wall -g -Iinclude
LDFLAGS =

# Répertoires
SRC_DIR  = src
INC_DIR  = include
OBJ_DIR  = build

# Nom de l'exécutable
TARGET   = tme

# Liste automatique des fichiers sources et objets
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# ============================================================
# Règles principales
# ============================================================

# Cible par défaut
all: $(TARGET)

# Édition de liens
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier build si nécessaire
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ============================================================
# Cibles utilitaires
# ============================================================

# Exécuter le programme
run: $(TARGET)
	./$(TARGET)

# Nettoyer les fichiers objets et l'exécutable
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Nettoyage complet (pareil que clean ici, mais conservé pour clarté)
mrproper: clean

.PHONY: all run clean mrproper
