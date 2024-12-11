#pragma once

#include <string>
#include <variant>
#include "json.hpp"

#if defined(TE_FLOAT) && defined(TE_LONG_DOUBLE)
#error TE_FLOAT and TE_LONG_DOUBLE compile options cannot be combined. Only one data type can be specified.
#endif

#ifdef EXPR_FLOAT
using num_t = float;
#elif defined(EXPR_LONG_DOUBLE)
using num_t = long double;
#else
using num_t = double;
#endif

using string_t = std::string;
using json_t = nlohmann::json;

using token_data_t = std::variant<num_t, string_t, json_t>;

enum class data_type
{
    NUMBER,
    STRING,
    ARRAY,
    JSON,
    NULL_TYPE
};

enum class token_types
{
    LITERAL,
    OPERATOR,
    GROUPING_OPERATOR,
    VARIABLE,
    FUNCTION,
    ARGUMENT_SEPARATOR
};

struct token_t
{
    token_types type;
    data_type value_type;
    token_data_t value;

    token_t(token_types t, data_type vt, token_data_t v) : type(t), value_type(vt), value(v) {}
};

struct operator_info_t
{
    int precedence;
    bool right_associative;
    int num_args;
};

using token_stream_t = std::vector<token_t>;
using function_resolver_t = std::function<token_data_t(const std::string_view &symbol)>;


// math functions that can be used in the expression (needs to be validated that are numbers)
using m_generic_function = num_t (*)(const num_t *args);

struct m_function_info
{
    m_generic_function func;
    int num_args;
};


// functions that can transform any token_data_t to other token_data_t despite of the type
using f_generic_function = std::function<token_data_t(const token_data_t *)>;

struct f_function_info
{
    f_generic_function func;
    int num_args;
};
