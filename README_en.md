<div align="center">
  
![XpressionLogo](/img/XpressionLogo2.png)

English | [Português](README.md)
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

> A modular expression evaluation engine in C with built-in functions, context variables, and AST export in JSON/XML.

## 📋 Table of Contents

- 🎯 What is Xpression Engine?
- ✨ Features
- 🛠️ Installation
- 🚀 Quick Start
- 📖 Usage Guide
- 🔧 Built-in Functions
- 🌳 Context Variables
- 💡 Practical Examples
- 🧩 Extensibility
- 🤝 Contributing

## 🎯 What is Xpression Engine?

**Xpression Engine** is a C library that allows evaluating expressions encapsulated in the `${EXPRESSION}` format. It's perfect for systems that need dynamic processing of configurations, templates, or runtime calculations.

**Compatibility:**
- **Operating Systems:** Linux, Windows
- **Architectures:** x86, x86_64, ARM, AArch64
- **Standard:** C99+

## ✨ Features

| Category | Feature | Details | Status |#|
|----------|---------|---------|--------|--|
| **Core** | Expression evaluation | Robust parser with AST | Complete | ✅ |
| 🔧 **Functions** | Built-in functions | Math, Text, Logic, Arrays | **20 functions** | ✅ |
| 🌳 **Context** | Variable hierarchy | Deep chaining support | Complete | ✅ |
| 📊 **Export** | AST (JSON/XML) | Structured and grouped | Complete | ✅ |
| 🚀 **Performance** | Zero dependencies | Only C stdlib | Complete | ✅ |
| 🔌 **API** | Extensibility | Interface for custom functions | Complete | ✅ |
| **Robustness** | Error handling | Detailed error messages | **Improved** | ✅ |
| 📝 **Types** | Type support | Numbers, Strings, Booleans, Arrays | **Expanded** | ✅ |
| **Usability** | Real-time context | Context definition via CLI or file | **Coming Soon** | 🚀 |
| 🌐 **Integration** | Database connectivity | Context definition via MySQL, etc. | **Coming Soon** | 🚀 |

### Rich Supported Syntax

```bash
# Supported data types
Numbers:      123, -4.56, 3.14159
Strings:      "hello", 'world', "text with spaces"
Booleans:     true, false
Arrays:       [1, 2, 3], ["a", "b", "c"], [true, false]

# Supported structures
Identifiers:   CONFIG, SYSTEM, USER_DATA
Chaining:     CONFIG.DB.USER.SETTINGS
Multi-access: OBJ.[prop1, prop2, prop3]
Functions:    SUM(a, b, c), MAX([1,2,3])
Nesting:      IF(GT(A,B), SUM(A,10), MUL(B,5))
```

## 🛠️ Installation

### Prerequisites
- C99+ compiler (gcc, clang)
- CMake (optional)

### Quick Installation

```bash
# Clone the repository
git clone https://github.com/CodeShark37/Xpression-Engine-C.git

# Enter the directory
cd Xpression-Engine-C

# Compile
gcc -o xpression -O2 -s *.c

# Run
./xpression
```

## 🚀 Quick Start

### Your First Command

```bash
# Evaluate a simple sum
./xpression -eval "${SUM(10, 20, 5)}"
# Output: 35

# View AST in JSON
./xpression -json -eval "${MAX(1, 9, 4)}"
# Output: AST + Result: 9

# Logical operations
./xpression -eval "${IF(GT(10,5), 'yes', 'no')}"
# → "yes"

# Array manipulation
./xpression -eval "${MAX([1, 9, 4])}"
# → 9

# Text processing
./xpression -eval "${CONCAT('Hello', ' ', 'World')}"
# → "Hello World"

```
### Basic Syntax

```bash
./xpression [options] "${EXPRESSION}"
```

## 📖 Usage Guide

### Available Options

| Option | Description | Example |
|--------|-------------|---------|
| `-eval` | Evaluates and returns the result | `./xpression -eval "${SUM(1,2)}"` |
| `-json` | Exports AST in JSON | `./xpression -json "${MAX(5,3)}"` |
| `-xml` | Exports AST in XML | `./xpression -xml "${MIN(2,8)}"` |
| `-group` or `-g` | Grouped AST | `./xpression -json -g "${SUM(1,2)}"` |
| `-f file` | Reads expression from file | `./xpression -eval -f input.txt` |


## 🔧 Built-in Functions

### Mathematical

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `SUM(...)` | Sums all arguments | `SUM([5, 2, 8, 1])` | `16` |
| `MUL(...)` | Multiplies all arguments | `MUL([5, 2, 8, 1])` | `80` |
| `MAX(...)` | Returns the largest value | `MAX(5, 2, 8, 1)` | `8` |
| `MIN(...)` | Returns the smallest value | `MIN(5, 2, 8, 1)` | `1` |
| `AVG(...)` | Calculates the average of values | `AVG([5, 2, 8, 1])` | `4` |

### Text

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `UPPERCASE(text)` | Converts to uppercase | `UPPERCASE('hello')` | `"HELLO"` |
| `CONCAT(...)` | Concatenates strings | `CONCAT('hello',' Xpression')` | `"hello Xpression"` |
| `MIXED(...)` | Concatenates with separator | `MIXED(42, 'abc')` | `"42\|abc"` |

### Logical

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `IF(cond,YES,NO)` | Conditional If | `IF(EQ(2,4),SUM(1,1),7)` | `7` |
| `EQ(x,y)` | Logical Equal | `EQ(2,4)` | `false` |
| `NEQ(x,y)` | Logical Not Equal | `NEQ(2,4)` | `true` |
| `LT(x,y)` | Logical Less Than | `LT(2,4)` | `true` |
| `GT(x,y)` | Logical Greater Than | `GT(2,4)` | `false` |
| `LTE(x,y)` | Logical Less Than or Equal | `LTE(2,4)` | `true` |
| `GTE(x,y)` | Logical Greater Than or Equal | `GTE(2,4)` | `false` |

### Miscellaneous

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `COUNT([...],x)` | How many times x appears in Array | `COUNT([4,'a',3,3],3)` | `2` |
| `CONTAINS([...],x)` | If x is contained in Array | `CONTAINS([4,'a',3,3],'a')` | `true` |
| `MAP([...],FUNC)` | Applies FUNC to every elem in Array | `MAP([[1,5,2,1],[3,3,4],[5,5]],UNIQUE)` | `[[1,5,2],[3,4],[5]]` |
| `SORT([...])` | Sorts the Array | `SORT(UNIQUE([1,2,34,4,3,432,22,2,3,1]))` | `[1,2,3,4,22,34,432]` |
| `UNIQUE([...])` | Generates Array without repeated elements | `UNIQUE([1,2,34,4,3,432,22,2,3,1])` | `[1,2,34,4,3,432,22]` |

## 🌳 Context Variables

The system has a pre-defined hierarchical context:

```
root
🔑 KEYWORD
│  └─ FUNCTION = "FUNCTION_VALUE"
├─ ⚙️ CONFIG
│  └─ 🗄️ DB
│     └─ 👤 USER
│        └─ NAME = "db_user"
├─ 🖥️ SYSTEM
│  └─ ⚙️ CONFIG
│     └─ 🌐 NETWORK
│        └─ 🛡️ PROXY
│           └─ HOST = "10.0.0.1"
├─ IDENTIFIER = "X123"
├─ A = 10
├─ B = 20
├─ C = 5
├─ D = 15
├─ PARAMETERS_LIST = "char*,int"
└─ STATEMENTS = "return 0;"
```

### Accessing Variables

```bash
# Simple variable
./xpression -eval "${A}"                    # → 10

# Chaining
./xpression -eval "${CONFIG.DB.USER.NAME}"  # → "db_user"

# Using in functions
./xpression -eval "${SUM(A, B, C)}"         # → 35
```

## 💡 Practical Examples

### Basic Examples

```bash
# 1. Mathematical operations
./xpression -eval "${SUM(10, 20, 12)}"      # → 42
./xpression -eval "${MAX(1, 9, 4, 7)}"      # → 9
./xpression -eval "${MIN(5, 8, 3, 10)}"     # → 3

# 2. Text processing
./xpression -eval "${UPPERCASE('hello')}"    # → "HELLO"
./xpression -eval "${MAP([[1,2],[3,4],[5]],SUM)}"     # → [3,7,5]
./xpression -eval "${MIXED(42, 'abc')}"      # → "42|abc"

# 3. Negative values
./xpression -eval "${SUM(-5, 10, 7)}"        # → 12
./xpression -eval "${MAX(-1, -3, -7)}"       # → -1
./xpression -eval "${MIN(0, 5, -2, 7)}"      # → -2
```

### Advanced Examples

```bash
# 4. Nested functions
./xpression -eval "${SUM(SUM(1,2,3), 10)}"           # → 16
./xpression -eval "${MAX(2, MAX(5,1), 3)}"           # → 5
./xpression -eval "${SUM(10, MIN(5,2,8))}"           # → 12

# 5. Complex combinations
./xpression -eval "${SUM(MIN(1,9,3), MAX(4,6,2))}"   # → 7
./xpression -eval "${MIXED(SUM(1,2,3), 'xyz')}"      # → "6|xyz"
./xpression -eval "${MIXED(MAX(1,9), MIN(2,8))}"     # → "9|2"

# 6. With context variables
./xpression -eval "${SUM(A, B)}"                     # → 30
./xpression -eval "${MIXED(IDENTIFIER, A)}"          # → "X123|10"

# 7. Complex mathematical operations
./xpression -eval "${AVG(MAP([[1,2,3],[4,5,6],[7,8,9]], SUM))}"
# → 15 (average of sums: [6,15,24] → 15)

# 8. Chained conditional processing
./xpression -eval "${IF(CONTAINS(UNIQUE([1,2,2,3,1]), 3), SUM([1,2,3]), MUL([1,2,3]))}"
# → 6 (contains 3, so sum)

# 9. Complex string analysis
./xpression -eval "${MIXED(UPPERCASE('status'), COUNT(['ok','ok','error','ok'], 'ok'))}"
# → "STATUS|3"

# 10. Nested array validation
./xpression -eval "${SORT(MAP([[5,2],[8,1],[3,4]], MAX))}"
# → [4,5,8] (maximum of each sub-array, then sorted)

# 11. Check production environment
./xpression -eval "${IF(EQ(SYSTEM.ENV, 'prod'), CONFIG.PROD.DB, CONFIG.DEV.DB)}"

# 12. Calculate system capacity
./xpression -eval "${MUL(SYSTEM.CPU_CORES, SYSTEM.MEMORY_GB, 0.8)}"

# 13. Validate user permissions
./xpression -eval "${CONTAINS(USER.ROLES, 'admin')}"

# 14. Calculate final score
./xpression -eval "${SUM(MUL(PLAYER.KILLS, 100), MUL(PLAYER.ASSISTS, 50), PLAYER.BONUS)}"

# 15. Determine ranking
./xpression -eval "${IF(GT(PLAYER.SCORE, 1000), 'EXPERT', IF(GT(PLAYER.SCORE, 500), 'INTERMEDIATE', 'BEGINNER'))}"

# 16. Validate achievement
./xpression -eval "${IF(AND(GT(PLAYER.LEVEL, 10), CONTAINS(PLAYER.ITEMS, 'rare_sword')), 'achievement_unlocked', 'keep_playing')}"
```

### Examples with AST Export

```bash
# Simple JSON
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

# Grouped XML
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

## 🧩 Extensibility

### Adding Custom Functions

```c
#include "functions.h"

// Define new function
Value *fn_multiply(Value* this, Value **args, size_t argc) {
    if (argc != 2) return val_num(0);
    return val_num(args[0]->num * args[1]->num);
}

// Register function
register_function("MULTIPLY", fn_multiply);
```

### Creating Custom Contexts

```c
#include "context.h"

CtxNode *build_custom_context(void) {
    CtxNode *root = ctx_new("root");
    
    // Add child node
    CtxNode *config = ctx_new("CONFIG");
    ctx_add_child(root, config);
    
    // Set property
    ctx_set_prop(config, "VERSION", val_str("1.0.0"));
    
    return root;
}
```

## 🔧 Troubleshooting

### Common Issues

| Category | Error | Example Input | Error Message | Description |
|----------|-------|---------------|---------------|-------------|
| **Syntax**|Malformed expression | `${EQ(1,X)`|`Malformed placeholders` | Check `${}` and parentheses |
| **Syntax**| Invalid syntax inside `${}` |`${UPPERCASE('test)}` |`Parse Failed` |Check for invalid characters or missing valid ones|
| **Syntax** | Extra content after expression | `"func() extra"` | `L1:C8: unexpected content after expression: 'e'` | Additional characters after complete valid expression |
| **Unexpected EOF** | End of input | `""` (empty string) | `L1:C1: unexpected EOF` | Input ended unexpectedly |
| **Invalid Characters** | Unrecognized character | `"user@name"` | `L1:C5: unexpected '@'` | Found character that cannot start an expression |
| **Strings** | Unterminated string | `"'hello world"` | `L1:C1: unterminated string` | String literal without closing quote |
| **Arrays** | Unterminated array | `"[1, 2, 3"` | `L1:C8: unterminated array` | Array without closing bracket `]` |
| **Arrays** | Invalid separator in array | `"[1 2 3]"` | `L1:C4: expected ',' or ']' in array` | Missing commas between array elements |
| **Functions** | Unterminated function | `"func(1, 2"` | `L1:C9: expected ',' or ')' in function call` | Function without closing parenthesis `)` |
| **Functions** | Invalid separator in function | `"func(a b c)"` | `L1:C7: expected ',' or ')' in function call` | Missing commas between function arguments |
| **Multi-access** | Unterminated multi-access | `"obj.[prop1, prop2"` | `L1:C16: unterminated multi-access` | Multi-access without closing bracket `]` |
| **Multi-access** | Invalid expression in multi-access | `"obj.[, prop2]"` | `L1:C6: expected expression in multi-access` | Missing or invalid expression inside `.[...]` |
| **Multi-access** | Invalid separator in multi-access | `"obj.[prop1 prop2]"` | `L1:C12: expected ',' or ']' in multi-access` | Missing commas between multi-access elements |
| **Properties** | Property missing after dot | `"obj."` | `L1:C5: expected property after '.'` | Dot not followed by valid property |
| **Memory** | Allocation failure | N/A (system error) | `L1:C1: memory allocation failed` | Internal memory allocation error |

### Debug Tips

```bash
# View AST for debugging
./xpression -json "${YOUR_EXPRESSION}"

# Test step by step
./xpression -eval "${SUM(1,2)}"     # Basic test
./xpression -eval "${SUM(A,B)}"     # With variables
```

## 🤝 Contributing

Contributions are very welcome! 

### How to Contribute

1. **Fork** the repository
2. **Create** a branch for your feature (`git checkout -b feature/new-functionality`)
3. **Commit** your changes (`git commit -am 'Add new functionality'`)
4. **Push** to the branch (`git push origin feature/new-functionality`)
5. **Open** a Pull Request

### Guidelines

- ✅ C99+ code
- ✅ Tests for new functionalities
- ✅ Updated documentation
- ✅ Descriptive commits

### Report Issues

Found a bug or have a suggestion? [Open an issue](https://github.com/CodeShark37/Xpression-Engine-C/issues)!

---

<div align="center">
  
**If this project helped you in any way, leave a star!**  
  
**Made with ❤️ in Angola**

[![Stars](https://img.shields.io/github/stars/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)
[![Forks](https://img.shields.io/github/forks/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)

</div>
