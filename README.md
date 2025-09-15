[XpressionLogo](https://imgur.com/xRdYlUM)
# Xpression Engine em C
O Xpression Engine até ao momento é motor modular de avaliação de expressões encapsuladas no formato `${EXPRESSION}` com suporte a funções built-in, variavéis de contexto e export da AST em (JSON/XML).
[![Language](https://img.shields.io/badge/language-C-blue)]()  
  

## Introdução
O **Xpression Engine** é um projecto escrito em C que avalia expressões encapsuladas no formato `${EXPRESSION}`.  
Ele pode:

- Avaliar expressões directamente na CLI.
- Exportar a árvore sintática abstrata (AST) em formato **JSON** ou **XML**.
- Registrar funções **built-in** customizadas (`SUM`, `MAX`, `MIN`, `UPPERCASE`, `TO_UPPER`, `MIXED`).
- Criar e manipular contextos hierárquicos de dados (via `CtxNode`).

---

## Dependências
- Compilador **C99+**
- **CMake** (opcional)
- Nenhuma biblioteca externa obrigatória além da standard C

---

## Instalação

```bash
git clone https://github.com/CodeShark37/Xpression-Engine-C.git
cd Xpression-Engine-c
make
```

O executável será gerado como:

```bash
./xpression
```

---

## Uso

### Sintaxe geral
```bash
./xpression [-json|-xml] [-group|-g] [-eval] [-f file] "${EXPRESSION}"
```

### Opções
- `-json` → saída da AST em JSON.
- `-xml` → saída da AST em XML.
- `-group|-g` → saída da AST agrupada.
- `-eval` → avalia a expressão passada.
- `-f` → avalia a expressão passada por ficheiro.

### Variavéis de contexto de exemplo Incluídas
 ```
root
 ├─ KEYWORD.FUNCTION = "FUNCTION_VALUE"
 ├─ CONFIG.DB.USER.NAME = "db_user"
 ├─ SYSTEM.CONFIG.NETWORK.PROXY.HOST = "10.0.0.1"
 ├─ RULE
 ├─ IDENTIFIER = "X123"
 ├─ A = 10, B = 20, C = 5, D = 15
 ├─ PARAMETERS_LIST = "char*,int"
 └─ STATEMENTS = "return 0;"
```
- Acessar as variavéis usando `${KEYWORD.FUNCTION}` , `${A}`
---

## Exemplos de uso

### Exemplos Simples

1. **Soma de constantes**
```bash
./expr -json -eval "${SUM(10,20,12)}"
```
**Saída AST (JSON):**
```json
{
  "type": "function",
  "value": "SUM",
  "children": [
    {"type": "number", "value": "10"},
    {"type": "number", "value": "20"},
    {"type": "number", "value": "12"}
  ]
}
```
**EVALUATED:** `42`

---

2. **Máximo de valores**
```bash
./expr -eval "${MAX(1,9,4,7)}"
```
**EVALUATED:** `9`

---

3. **Mínimo de valores**
```bash
./expr -eval "${MIN(5,8,3,10)}"
```
**EVALUATED:** `3`

---

4. **Uppercase de string simples**
```bash
./expr -eval "${UPPERCASE('hello')}"
```
**EVALUATED:** `HELLO`

---

5. **Uppercase alternativa**
```bash
./expr -eval "${TO_UPPER('world')}"
```
**EVALUATED:** `WORLD`

---

6. **Concatenação mista**
```bash
./expr -eval "${MIXED(42,'abc')}"
```
**EVALUATED:** `42|abc`

---

7. **SUM com negativo**
```bash
./expr -eval "${SUM(-5,10,7)}"
```
**EVALUATED:** `12`

---

8. **MAX com valores negativos**
```bash
./expr -eval "${MAX(-1,-3,-7)}"
```
**EVALUATED:** `-1`

---

9. **MIN com zeros**
```bash
./expr -eval "${MIN(0,5,-2,7)}"
```
**EVALUATED:** `-2`

---

10. **TO_UPPER em palavra com espaço**
```bash
./expr -eval "${TO_UPPER('hello world')}"
```
**EVALUATED:** `HELLO WORLD`

---

### Exemplos Complexos

11. **Soma em cascata**
```bash
./expr -eval "${SUM(SUM(1,2,3),10)}"
```
**EVALUATED:** `16`

---

12. **Máximo aninhado**
```bash
./expr -eval "${MAX(2,MAX(5,1),3)}"
```
**EVALUATED:** `5`

---

13. **Mínimo dentro de soma**
```bash
./expr -eval "${SUM(10,MIN(5,2,8))}"
```
**EVALUATED:** `12`

---

14. **Uppercase de palavra com espaço**
```bash
./expr -eval "${UPPERCASE('foo bar')}"
```
**EVALUATED:** `FOO BAR`

---

15. **TO_UPPER em string com números**
```bash
./expr -eval "${TO_UPPER('abc123')}"
```
**EVALUATED:** `ABC123`

---

16. **MIXED com SUM**
```bash
./expr -eval "${MIXED(SUM(1,2,3),'xyz')}"
```
**EVALUATED:** `6|xyz`

---

17. **Soma grande**
```bash
./expr -eval "${SUM(100,200,300,400)}"
```
**EVALUATED:** `1000`

---

18. **Combinação de MIN e MAX**
```bash
./expr -eval "${SUM(MIN(1,9,3),MAX(4,6,2))}"
```
**EVALUATED:** `7`

---

19. **String nested em uppercase**
```bash
./expr -eval "${UPPERCASE(TO_UPPER('abc def'))}"
```
**EVALUATED:** `ABC DEF`

---

20. **MIXED combinado**
```bash
./expr -eval "${MIXED(MAX(1,9),MIN(2,8),SUM(3,4))}"
```
**EVALUATED:** `9|2|7`

---

## Contribuição
Contribuições são bem-vindas! Abra issues e PRs no repositório.

## Licença

