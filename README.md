<div align="center">
  
![XpressionLogo](/img/XpressionLogo2.png)

[[Read this page in English](README_en.md)]|[[Leia esta página em português](README.md)]
</div>
<div align="center">
  
[![Made with love in Angola](https://madewithlove.now.sh/ao?heart=true&template=for-the-badge)](#)

</div>
<div align="center">
  
[![GitHub release](https://img.shields.io/github/v/release/CodeShark37/Xpression-Engine-C)](#)
[![GitHub release date](https://img.shields.io/github/release-date/CodeShark37/Xpression-Engine-C)](#)
[![Language](https://img.shields.io/badge/language-C-blue)](#)

</div>

# 🚀 Xpression Engine

> Um motor modular de avaliação de expressões em C com suporte a funções built-in, variáveis de contexto e export da AST em JSON/XML.

## 📋 Índice

- 🎯 O que é o Xpression Engine?
- ✨ Features
-  Instalação
- 🚀 Início Rápido
- 📖 Guia de Uso
- 🔧 Funções Built-in
- 🌳 Variáveis de Contexto
- 💡 Exemplos Práticos
- Extensibilidade
- Contribuição

## 🎯 O que é o Xpression Engine?

O **Xpression Engine** é uma biblioteca em C que permite avaliar expressões encapsuladas no formato `${EXPRESSION}`. É perfeito para sistemas que precisam de processamento dinâmico de configurações, templates ou cálculos em tempo de execução.

**Compatibilidade:**
- **Sistemas Operacionais:** Linux, Windows
- **Arquiteturas:** x86, x86_64, ARM, AArch64
- **Padrão:** C99+

## ✨ Features

| Feature | Status |#|
|---------|--------|-|
| Avaliação de expressões | Implementado | ✅ |
| Funções built-in | 6 funções disponíveis| ✅ |
| Contexto hierárquico | Suporte completo | ✅ |
| Export AST (JSON/XML) | Implementado | ✅ |
| Extensibilidade | API para funções customizadas | ✅ |
| Zero dependências | Apenas bibliotecas padrão C | ✅ |

### Sintaxe Suportada

- **Identificadores**: `CONFIG`, `SYSTEM`, `USER`
- **Encadeamento**: `CONFIG.DB.USER` 
- **Funções**: `SUM(a, b, c)`
- **Literais**: números (`123`, `-4.56`), strings (`"abc"`, `'xyz'`), listas (`[1, 2, 'a']`)
- **Aninhamento**: `SUM(MAX(1,2), MIN(3,4))`

## Instalação

### Pré-requisitos
- Compilador C99+ (gcc, clang)
- CMake (opcional)

### Instalação Rápida

```bash
# Clone o repositório
git clone https://github.com/CodeShark37/Xpression-Engine-C.git

# Entre no diretório
cd Xpression-Engine-C

# Compile
gcc -o xpression *.c

# Execute
./xpression
```

## 🚀 Início Rápido

### Seu Primeiro Comando

```bash
# Avaliar uma soma simples
./xpression -eval "${SUM(10, 20, 5)}"
# Output: 35

# Ver a AST em JSON
./xpression -json -eval "${MAX(1, 9, 4)}"
# Output: AST + Resultado: 9
```

### Sintaxe Básica

```bash
./xpression [opções] "${EXPRESSION}"
```

## 📖 Guia de Uso

### Opções Disponíveis

| Opção | Descrição | Exemplo |
|-------|-----------|---------|
| `-eval` | Avalia e retorna o resultado | `./xpression -eval "${SUM(1,2)}"` |
| `-json` | Exporta AST em JSON | `./xpression -json "${MAX(5,3)}"` |
| `-xml` | Exporta AST em XML | `./xpression -xml "${MIN(2,8)}"` |
| `-group` ou `-g` | AST agrupada | `./xpression -json -g "${SUM(1,2)}"` |
| `-f arquivo` | Lê expressão de arquivo | `./xpression -f input.txt` |

### Exemplos de Comandos

```bash
# Avaliação simples
./xpression -eval "${SUM(10, 20, 30)}"          # → 60

# Com contexto
./xpression -eval "${CONFIG.DB.USER}"           # → "db_user"

# AST em JSON
./xpression -json "${UPPERCASE('hello')}"       # → AST + "HELLO"

# Funções aninhadas
./xpression -eval "${MAX(SUM(1,2), MIN(5,3))}"  # → 3
```

## 🔧 Funções Built-in

### Matemáticas

| Função | Descrição | Exemplo | Resultado |
|--------|-----------|---------|-----------|
| `SUM(...)` | Soma todos os argumentos | `SUM(1, 2, 3, 4)` | `10` |
| `MAX(...)` | Retorna o maior valor | `MAX(5, 2, 8, 1)` | `8` |
| `MIN(...)` | Retorna o menor valor | `MIN(5, 2, 8, 1)` | `1` |

### Texto

| Função | Descrição | Exemplo | Resultado |
|--------|-----------|---------|-----------|
| `UPPERCASE(texto)` | Converte para maiúsculas | `UPPERCASE('hello')` | `"HELLO"` |
| `TO_UPPER(texto)` | Alias para UPPERCASE | `TO_UPPER('world')` | `"WORLD"` |
| `MIXED(...)` | Concatena com separador | `MIXED(42, 'abc')` | `"42\|abc"` |

## 🌳 Variáveis de Contexto

O sistema possui um contexto hierárquico pré-definido:

```
root
├─ KEYWORD
│  └─ FUNCTION = "FUNCTION_VALUE"
├─ CONFIG
│  └─ DB
│     └─ USER
│        └─ NAME = "db_user"
├─ SYSTEM
│  └─ CONFIG
│     └─ NETWORK
│        └─ PROXY
│           └─ HOST = "10.0.0.1"
├─ IDENTIFIER = "X123"
├─ A = 10
├─ B = 20
├─ C = 5
├─ D = 15
├─ PARAMETERS_LIST = "char*,int"
└─ STATEMENTS = "return 0;"
```

### Acessando Variáveis

```bash
# Variável simples
./xpression -eval "${A}"                    # → 10

# Encadeamento
./xpression -eval "${CONFIG.DB.USER.NAME}"  # → "db_user"

# Usando em funções
./xpression -eval "${SUM(A, B, C)}"         # → 35
```

## 💡 Exemplos Práticos

### Exemplos Básicos

```bash
# 1. Operações matemáticas
./xpression -eval "${SUM(10, 20, 12)}"      # → 42
./xpression -eval "${MAX(1, 9, 4, 7)}"      # → 9
./xpression -eval "${MIN(5, 8, 3, 10)}"     # → 3

# 2. Processamento de texto
./xpression -eval "${UPPERCASE('hello')}"    # → "HELLO"
./xpression -eval "${TO_UPPER('world')}"     # → "WORLD"
./xpression -eval "${MIXED(42, 'abc')}"      # → "42|abc"

# 3. Valores negativos
./xpression -eval "${SUM(-5, 10, 7)}"        # → 12
./xpression -eval "${MAX(-1, -3, -7)}"       # → -1
./xpression -eval "${MIN(0, 5, -2, 7)}"      # → -2
```

### Exemplos Avançados

```bash
# 4. Funções aninhadas
./xpression -eval "${SUM(SUM(1,2,3), 10)}"           # → 16
./xpression -eval "${MAX(2, MAX(5,1), 3)}"           # → 5
./xpression -eval "${SUM(10, MIN(5,2,8))}"           # → 12

# 5. Combinações complexas
./xpression -eval "${SUM(MIN(1,9,3), MAX(4,6,2))}"   # → 7
./xpression -eval "${MIXED(SUM(1,2,3), 'xyz')}"      # → "6|xyz"
./xpression -eval "${MIXED(MAX(1,9), MIN(2,8))}"     # → "9|2"

# 6. Com variáveis de contexto
./xpression -eval "${SUM(A, B)}"                     # → 30
./xpression -eval "${MIXED(IDENTIFIER, A)}"          # → "X123|10"
```

### Exemplos com Export AST

```bash
# JSON simples
./xpression -json -eval "${SUM(1, 2, 3)}"
# Output:
# {
#   "type": "function",
#   "value": "SUM",
#   "children": [
#     {"type": "number", "value": "1"},
#     {"type": "number", "value": "2"},
#     {"type": "number", "value": "3"}
#   ]
# }
# EVALUATED: 6

# XML agrupado
./xpression -xml -g -eval "${MIXED(UPPERCASE('test'),SUM(1, 2, 3))}"
# Output:
# <expressions>
#  <function value="MIXED">
#    <function value="UPPERCASE">
#      <string value="test" />
#    </function>
#    <function value="SUM">
#      <number value="1" />
#      <number value="2" />
#      <number value="3" />
#    </function>
#  </function>
#  <evaluated>TEST|6</evaluated>
# </expressions>
```

## Extensibilidade

### Adicionando Funções Personalizadas

```c
#include "functions.h"

// Definir nova função
Value fn_multiply(Value *args, int argc) {
    if (argc != 2) return val_num(0);
    return val_num(args[0].num * args[1].num);
}

// Registar função
register_func_cb("MULTIPLY", fn_multiply);
```

### Criando Contextos Customizados

```c
#include "context.h"

CtxNode *build_custom_context(void) {
    CtxNode *root = ctx_new("root");
    
    // Adicionar nó filho
    CtxNode *config = ctx_new("CONFIG");
    ctx_add_child(root, config);
    
    // Definir propriedade
    ctx_set_prop(config, "VERSION", val_str("1.0.0"));
    
    return root;
}
```

## 🔧 Troubleshooting

### Problemas Comuns

| Problema | Causa | Solução |
|----------|-------|---------|
| `Malformed placeholders` | Expressão mal formada | Verifique `${}` e parênteses |
| `Parse Failed` | Sintaxe inválida dentro de `${}` | Verifique caracteres inválidos |

### Dicas de Debug

```bash
# Ver AST para debug
./xpression -json "${SUA_EXPRESSAO}"

# Testar passo a passo
./xpression -eval "${SUM(1,2)}"     # Teste básico
./xpression -eval "${SUM(A,B)}"     # Com variáveis
```

## Contribuição

Contribuições são muito bem-vindas! 

### Como Contribuir

1. **Fork** o repositório
2. **Crie** uma branch para sua feature (`git checkout -b feature/nova-funcionalidade`)
3. **Commit** suas mudanças (`git commit -am 'Adiciona nova funcionalidade'`)
4. **Push** para a branch (`git push origin feature/nova-funcionalidade`)
5. **Abra** um Pull Request

### Diretrizes

- ✅ Código em C99+
- ✅ Testes para novas funcionalidades
- ✅ Documentação atualizada
- ✅ Commits descritivos

### Reportar Issues

Encontrou um bug ou tem uma sugestão? [Abra uma issue](https://github.com/CodeShark37/Xpression-Engine-C/issues)!

---

<div align="center">

**Feito com ❤️ em Angola**

[![Stars](https://img.shields.io/github/stars/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)
[![Forks](https://img.shields.io/github/forks/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)

</div>
