# =========================================================================
# Compilador e Padrão C
# =========================================================================
CC = gcc
CFLAGS = -std=c11 -Wmain -Wall -Wextra -Winline -Winit-self -Wunreachable-code -pedantic

# Tratamento do Tipo de Compilação (Padrão: release)
TYPE ?= release
ifeq ($(TYPE),debug)
    BUILD_FLAGS = -g3 -ggdb -O0
    LDFLAGS     = -static -static-libgcc
else
    BUILD_FLAGS = -O2 -ffunction-sections -fdata-sections
    LDFLAGS     = -static -static-libgcc -s -Wl,--gc-sections
endif

# Diretorias lógicas do projeto
INC_DIR = include
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = bin/obj/$(TYPE)

# Ficheiros Fonte
MAIN_SRCS = $(SRC_DIR)/main.c
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)

# Mapeamento de Objetos
MAIN_OBJ  = $(OBJ_DIR)/main.o
CORE_OBJS = $(patsubst $(SRC_DIR)/core/%.c, $(OBJ_DIR)/core/%.o, $(CORE_SRCS))

TARGET = $(BIN_DIR)/xpression.exe

ifeq ($(SHELL),cmd.exe)
    MKDIR  = if not exist $(1) mkdir $(subst /,\\,$(1))
    RM_DIR = if exist $(1) rmdir /s /q $(subst /,\\,$(1))
else
    MKDIR  = mkdir -p $(1)
    RM_DIR = rm -rf $(1)
endif

# =========================================================================
# REGRAS PRINCIPAIS
# =========================================================================
.PHONY: all clean clean-obj clean-bin

all: $(TARGET)
	@echo "[Makefile] Processamento do xpression.exe concluido com sucesso! [Modo: $(TYPE)]"

$(TARGET): $(MAIN_OBJ) $(CORE_OBJS) | $(BIN_DIR)
	@echo "[GCC - $(TYPE)] Linkando o executavel final: $@"
	$(CC) $(CFLAGS) $(BUILD_FLAGS) -I$(INC_DIR) $^ -o $@ $(LDFLAGS)

# =========================================================================
# COMPILAÇÃO INCREMENTAL DOS OBJETOS
# =========================================================================
$(MAIN_OBJ): $(MAIN_SRCS) | $(OBJ_DIR)
	@echo "[GCC - $(TYPE)] Compilando: $<"
	$(CC) $(CFLAGS) $(BUILD_FLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/core/%.o: $(SRC_DIR)/core/%.c | $(OBJ_DIR)/core
	@echo "[GCC - $(TYPE)] Compilando componente Core: $<"
	$(CC) $(CFLAGS) $(BUILD_FLAGS) -I$(INC_DIR) -c $< -o $@

# =========================================================================
# CRIAÇÃO DE DIRETORIAS
# =========================================================================
$(BIN_DIR):
	@$(call MKDIR,$(BIN_DIR))

$(OBJ_DIR):
	@$(call MKDIR,$(OBJ_DIR))

$(OBJ_DIR)/core:
	@$(call MKDIR,$(OBJ_DIR)/core)

# =========================================================================
# LIMPEZA
# =========================================================================
clean: clean-obj clean-bin
	@echo "[Makefile] Limpeza completa concluida!"

clean-obj:
	@echo "[Makefile] A remover pasta completa de objetos (obj/)..."
	@$(call RM_DIR,obj)

clean-bin:
	@echo "[Makefile] A remover pasta de binarios (bin/)..."
	@$(call RM_DIR,$(BIN_DIR))