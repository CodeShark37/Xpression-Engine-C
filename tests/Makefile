# =========================================
# Makefile — CLI Engine Test Suite
# =========================================
# Usage:
#   make            → compila todos os binários de teste
#   make run        → executa todos os testes
#   make clean      → limpa artefatos
# =========================================

# Variáveis do compilador
CC       := gcc
CFLAGS   := -Os -Wall -g -I..
LDFLAGS  := 

# Diretório de fontes principais da engine
ENGINE_SRCS := \
    ../cli.c \
    ../parser.c \
    ../value.c \
    ../functions.c \
    ../evaluator.c \
    ../printer.c \
    ../error.c \
    ../context.c \
    ../ast.c \
	Xtest.c

# Diretórios dos testes
TEST_SRCS := \
    test_cli.c \
    test_cli_cases.c

# Objetos gerados automaticamente
OBJS := $(TEST_SRCS:.c=.o)

# =========================================
# Regras de compilação
# =========================================
.PHONY: all clean run

# Regra para compilar todos os binários de teste
all: $(TEST_SRCS:.c=.exe)

# Regra para compilar um binário para cada arquivo de teste
%.exe: %.c
	@echo "Compilando $< ..."
	$(CC) $(CFLAGS) $(ENGINE_SRCS) $< -o $@ $(LDFLAGS)
	@echo "Binário $@ gerado."

# =========================================
# Execução de testes
# =========================================
run: $(TEST_SRCS:.c=.out)
	@echo "Executando todos os testes..."
	@for test in $(TEST_SRCS:.c=.out); do ./$$test; done

# =========================================
# Limpeza
# =========================================
clean:
	@echo "Limpando arquivos de build..."
	rm -f $(OBJS) $(TEST_SRCS:.c=.out) *.log cli_case_tmp_* cli_stress_tmp_* stress_report.log
	@echo "Limpeza concluída."
