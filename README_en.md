<div align="center">
  
![XpressionLogo](/img/XpressionLogo2.png)
English | [Portuguese](README.md)
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

| Feature | Status |#|
|---------|--------|-|
| Expression evaluation | Implemented | ✅ |
| Built-in functions | 6 functions available| ✅ |
| Hierarchical context | Full support | ✅ |
| AST export (JSON/XML) | Implemented | ✅ |
| Extensibility | API for custom functions | ✅ |
| Zero dependencies | Only standard C libraries | ✅ |

### Supported Syntax

- **Identifiers**: `CONFIG`, `SYSTEM`, `USER`
- **Chaining**: `CONFIG.DB.USER` 
- **Functions**: `SUM(a, b, c)`
- **Literals**: numbers (`123`, `-4.56`), strings (`"abc"`, `'xyz'`), arrays (`[1, 2, 'a']`)
- **Nesting**: `SUM(MAX(1,2), MIN(3,4))`

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
gcc -o xpression *.c

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
| `-f file` | Reads expression from file | `./xpression -f input.txt` |

### Command Examples

```bash
# Simple evaluation
./xpression -eval "${SUM(10, 20, 30)}"          # → 60

# With context
./xpression -eval "${CONFIG.DB.USER}"           # → "db_user"

# AST in JSON
./xpression -json "${UPPERCASE('hello')}"       # → AST + "HELLO"

# Nested functions
./xpression -eval "${MAX(SUM(1,2), MIN(5,3))}"  # → 3
```

## 🔧 Built-in Functions

### Mathematical

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `SUM(...)` | Sums all arguments | `SUM(1, 2, 3, 4)` | `10` |
| `MAX(...)` | Returns the largest value | `MAX(5, 2, 8, 1)` | `8` |
| `MIN(...)` | Returns the smallest value | `MIN(5, 2, 8, 1)` | `1` |

### Text

| Function | Description | Example | Result |
|----------|-------------|---------|--------|
| `UPPERCASE(text)` | Converts to uppercase | `UPPERCASE('hello')` | `"HELLO"` |
| `TO_UPPER(text)` | Alias for UPPERCASE | `TO_UPPER('world')` | `"WORLD"` |
| `MIXED(...)` | Concatenates with separator | `MIXED(42, 'abc')` | `"42\|abc"` |

## 🌳 Context Variables

The system has a pre-defined hierarchical context:

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
./xpression -eval "${TO_UPPER('world')}"     # → "WORLD"
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
register_func_cb("MULTIPLY", fn_multiply);
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

| Problem | Cause | Solution |
|---------|-------|----------|
| `Malformed placeholders` | Malformed expression | Check `${}` and parentheses |
| `Parse Failed` | Invalid syntax inside `${}` | Check for invalid characters |

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

**Made with ❤️ in Angola**

[![Stars](https://img.shields.io/github/stars/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)
[![Forks](https://img.shields.io/github/forks/CodeShark37/Xpression-Engine-C?style=social)](https://github.com/CodeShark37/Xpression-Engine-C)

</div>
