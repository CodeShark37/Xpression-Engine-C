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

> [!WARNING]
> **I strongly recommend downloading the latest release instead of cloning this repository.**
>
> The code in this repository is under active development and may contain incomplete changes, breaking updates, or code that **does not compile**.
>
> For a stable and tested version, please download the latest release from the **Releases** page.
>
> 🔗 **Releases:** https://github.com/CodeShark37/Xpression-Engine-C/releases

## Table of Contents

- What is Xpression Engine?
- Features
- Installation
- Quick Start
- Usage Guide
- Built-in Functions
- Context Variables
- Practical Examples
- Extensibility
- The Evolution of the Engine (and of a Programmer)
- Contributing

## What is Xpression Engine?

**Xpression Engine** is a C library that allows evaluating expressions encapsulated in the `${EXPRESSION}` format. It's perfect for systems that need dynamic processing of configurations, templates, or runtime calculations.

**Compatibility:**
- **Operating Systems:** Linux, Windows
- **Architectures:** x86, x86_64, ARM, AArch64
- **Standard:** C11+

## Features

| Category | Feature | Details | Status |#|
|----------|---------|---------|--------|--|
| **Core** | Expression evaluation | Robust parser with AST | Complete | ✅ |
| **Functions** | Built-in functions | Math, Text, Logic, Arrays | **20 functions** | ✅ |
| **Context** | Variable hierarchy | Deep chaining support | Complete | ✅ |
| **Export** | AST (JSON/XML) | Structured and grouped | Complete | ✅ |
| **Performance** | Zero dependencies | Only C stdlib | Complete | ✅ |
| **API** | Extensibility | Interface for custom functions | Complete | ✅ |
| **Robustness** | Error handling | Detailed error messages | **Improved** | ✅ |
| **Types** | Type support | Numbers, Strings, Booleans, Arrays | **Expanded** | ✅ |
| **Usability** | Real-time context | Context definition via CLI or file | **Coming Soon** | 🚀 |
| **Integration** | Database connectivity | Context definition via MySQL, etc. | **Coming Soon** | 🚀 |

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

## Installation

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
```

### Basic Syntax

```bash
./xpression [options] "${EXPRESSION}"
```

## Usage Guide

### Available Options

| Option | Description | Example |
|--------|-------------|---------|
| `-eval` | Evaluates and returns the result | `./xpression -eval "${SUM(1,2)}"` |
| `-json` | Exports AST in JSON | `./xpression -json "${MAX(5,3)}"` |
| `-xml` | Exports AST in XML | `./xpression -xml "${MIN(2,8)}"` |
| `-group` or `-g` | Grouped AST | `./xpression -json -g "${SUM(1,2)}"` |
| `-f file` | Reads expression from file | `./xpression -eval -f input.txt` |


## Built-in Functions

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

## Context Variables

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

## Practical Examples

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

## Extensibility

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

## Troubleshooting

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

## The Evolution of the Engine (and of a Programmer)

Almost a year ago, while developing a C project, I ran into the need to store some data in a hierarchical context and access it using a placeholder like `${obj.prop}`.

What was supposed to be just another feature turned into an expression engine that pushed me to learn more and more, deepening my knowledge of parsers, compilers, architecture, and software documentation.

Today, heading toward the 3rd version (v3.0), I look back and I'm glad for the journey so far.

### Before v1.0: learning to write a parser

The first obstacle wasn't deciding what the engine should do, but figuring out how to write it.

I started by studying approaches that, at first, seemed natural for this kind of problem: tokenizers, parser combinators, and Pratt parsers. I tried each of them, but none fit exactly the expression language I wanted to build. Tokenizers ended up introducing a separation that didn't bring me any real advantage, and the other approaches didn't offer the balance between simplicity and flexibility I was looking for either.

After several experiments, I ended up following a different path: an ad-hoc parser, responsible for analyzing the expression directly and building the AST without going through a separate tokenization phase. It was a decision shaped by the project's evolution, but it turned out to be the solution that best fit the engine's grammar.

That decision forced me to confront problems that no isolated feature prepares you for: interpreting a sequence of characters without ambiguity, designing a grammar capable of supporting chaining (`CONFIG.DB.USER.SETTINGS`), multi-access (`OBJ.[prop1, prop2]`), and nested functions, and turning all of that into a coherent AST that could later be validated and evaluated.

At that stage, `Value` was still a simple struct with a dedicated field for each data type — `struct Val { double number; char *string; struct Val list; ... }`. It worked, but every `Value` carried the weight of all possible types, even when only one was actually in use.

This is when I realized I was no longer building a simple feature, but an expression language, with its own syntax, precedence, and evaluation rules. More than learning to write a parser, I learned to compare approaches, weigh their trade-offs, and choose the solution that made sense for that problem, instead of following a pattern just because it was the most well-known one.

### From v1.0 to maturity: growing without breaking

With the parser working, the challenge changed nature: it was no longer about writing from scratch, but about adding functionality without breaking what was already in place. Every new built-in function, every new data type, and every new way of accessing the context was an opportunity to introduce a silent regression somewhere in the grammar.

It was also during this phase that the CLI module became the project's first real headache. I wanted to enforce a strict order for the command-line options (`-eval`, `-json`, `-xml`, `-group`, `-f`), and nothing I tried felt correct or clean, until I realized I needed a finite state machine (FSM). It wasn't a new concept to me, but I had never seen it applied to this kind of problem. It was the first time that theoretical idea clicked so naturally with a concrete problem I'd already been trying to solve in other ways for a while.

Even so, even after being disciplined by the FSM, the CLI module (`cli.c`) still did too much: it parsed the arguments, called the expression parser, ran the evaluation, and handled printing the result. At the time I accepted that solution, but the sense that there were too many responsibilities crammed in there stayed with me, waiting for v3.0.

It was also around then that I started to realize that many problems stop looking complicated once you find the right model to represent them.

The result of this phase was an engine with 20 built-in functions for math, text, logic, and array operations, support for multiple data types, AST export to JSON and XML, and a more disciplined CLI. The error system started reporting line, column, and specific messages for each type of failure, all of this with no external dependencies beyond the C standard library, while keeping compatibility with C11+ and multiple platforms.

More than a list of features, this phase taught me that growing a project doesn't just mean adding code. It means making sure that what already exists stays correct, predictable, and easy to evolve.

### v3.0: consolidation and internal robustness

Once the engine started to stabilize, I realized it was time to look less at what it did and more at how it was built.

With around twenty built-in functions manipulating `Value`, the functions module (`functions.c`) had accumulated the same pattern repeated dozens of times: walking arguments, validating types, extracting values, and only then running each function's specific logic. The solution was to create a Traverse API, responsible for all of that shared work, leaving each built-in to worry only about its own logic.

That change naturally opened up space for function descriptors, where each built-in started explicitly declaring its return type, parameter types, and arity. With that information available, the validate module (`validate.c`) started validating calls automatically, freeing each function from that responsibility.

More important than eliminating repeated code was realizing the difference between abstracting code and creating an abstraction that actually simplifies the architecture.

That same concern for simplicity also drove the evolution of `Value`. After its first representation as a struct dedicated to each type, and the later phase based on `void *`, v3.0 adopted NaN Boxing, turning `Value` into a compact 8-byte type, always passed by value.

More important than shrinking the size of `Value` was realizing how the internal representation of data shapes the entire architecture of the engine, the simplicity of the API, and even the way the rest of the code evolves.

That same search for clarity extended to the AST as well. I introduced the `NODE_ACCESS` node as an explicit container to eliminate existing ambiguities, moved semantic validations from `parser.c` to `validate.c`, and set a depth limit to guard against excessive recursion. Individually these were small changes; together, they made the system more predictable and each module more responsible for its own job.

The biggest change, however, came when I finally decided to tackle the CLI module (`cli.c`).

Looking closer at the problem, I realized the module wasn't complicated just because it did too much. The real problem was that several modules implicitly depended on one another, creating a chain of dependencies that was hard to maintain. Separating responsibilities required first creating a common place for all the shared information.

That need is what gave birth to `XpressionContext`, a central context that replaces the `ErrorManager` singleton and eliminates most of that coupling between modules. Parser, evaluation, error system, printing, and CLI finally started communicating through a shared context, without depending directly on one another.

In the end, I realized the real problem was rarely a file that was too big. It was almost always the invisible dependencies between modules. Solving those dependencies taught me far more about software architecture than any isolated optimization ever did.

### Documentation

During this same phase, another challenge came up: properly documenting the engine with Doxygen.

It was a completely different exercise from writing code. There were no algorithms to optimize, no bugs to fix. The challenge was explaining decisions, justifying choices, and organizing information so that someone else could understand the project without needing to ask the author.

I wanted the documentation to be useful both for someone who had never worked with parsers and for someone who just needed to quickly understand a specific module. That forced me to explain decisions that, until then, only existed in my head.

Interestingly, documenting revealed areas where I myself hadn't organized my ideas well enough yet. Several times, writing the documentation led me to simplify interfaces, rename structures, and make parts of the architecture clearer.

That's also when I realized documenting had stopped being a task done at the end of development. It had become part of the process of designing software itself.

### What Xpression Engine taught me

Looking back, from that first `${obj.prop}` placeholder to here, what remains isn't just a more robust engine. Every limitation I ran into ended up pushing me toward the right concept: a proper parser, a finite state machine, more solid abstractions, a decentralized architecture, and documentation designed for whoever comes next.

It all started because I needed to solve a concrete problem. A year later, I realize the biggest evolution wasn't just the Xpression Engine's, but my own as a programmer.

Today I understand that programming stopped being just about writing code that works. It became about designing systems that are simple to understand, maintain, and evolve, even months later, and even by someone who has never seen that code before.

Version 3.0 is still under development, but it has already become proof that it's worth stopping and asking:

"Why is this still difficult?"

More often than not, the answer isn't that you need to write more code—it's that your architecture needs to evolve.

## Contributing

Contributions are very welcome! 

### How to Contribute

1. **Fork** the repository
2. **Create** a branch for your feature (`git checkout -b feature/new-functionality`)
3. **Commit** your changes (`git commit -am 'Add new functionality'`)
4. **Push** to the branch (`git push origin feature/new-functionality`)
5. **Open** a Pull Request

### Guidelines

- ✅ C11+ code
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
