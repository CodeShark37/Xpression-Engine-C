cmake_minimum_required(VERSION 3.21)
project(xpression VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# =========================================================================
# 1. CONFIGURAÇÃO DE FLAGS GERAIS E DO COMPILADOR
# =========================================================================
add_compile_options(-Wmain -Wall -Wextra -Winline -Winit-self -Wunreachable-code -pedantic)
add_link_options(-static -static-libgcc)


# =========================================================================
# 2. RECOLHA DO CORE E DECLARAÇÃO DA BIBLIOTECA INTERNA (NOME CORRIGIDO)
# =========================================================================
file(GLOB FONTES_CORE CONFIGURE_DEPENDS "src/core/*.c")
list(FILTER FONTES_CORE EXCLUDE REGEX "Cópia")

# Alterado para _xpression para evitar conflito com xpression.exe
add_library(_xpression STATIC ${FONTES_CORE})
target_include_directories(_xpression PUBLIC include)

# =========================================================================
# 3. TARGET PRINCIPAL DO PROGRAMA
# =========================================================================
add_executable(xpression src/main.c)
target_link_libraries(xpression PRIVATE _xpression)

# Flags de Release para o executável principal
target_compile_options(xpression PRIVATE $<$<CONFIG:Release>:-O2 -ffunction-sections -fdata-sections>)
target_link_options(xpression PRIVATE $<$<CONFIG:Release>:-s -Wl,--gc-sections>)

# =========================================================================
# 4. ATIVAÇÃO E INCLUSÃO DOS TESTES
# =========================================================================
enable_testing()
include(CTest)
add_subdirectory(tests)
