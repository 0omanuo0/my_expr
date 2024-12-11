# Define la biblioteca estática o compartida para my_expr
file(GLOB MY_EXPR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/my_expr/*.cpp
)

# Crear la biblioteca (estática o compartida)
add_library(my_expr_lib ${MY_EXPR_SRCS})

# Incluir los encabezados necesarios para my_expr
target_include_directories(my_expr_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/my_expr)

set(DEBUG_CXXFLAGS "-Wall -g -Og -O0 -DDEBUG -rdynamic -fdiagnostics-color=always")
set(CMAKE_BUILD_TYPE Debug)

# Opcional: Añadir compiladores específicos o flags, si se necesitan
# target_compile_options(my_expr_lib PRIVATE -Wall -Wextra)
