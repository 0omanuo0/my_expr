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
