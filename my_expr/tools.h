
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <stack>
#include <variant>
#include "json.hpp"
#include "my_expr_dtypes.h"

inline double stringToNumber(const std::string &str)
{
    char *ptr;
    auto r = std::strtod(str.c_str(), &ptr);
    return !*ptr ? r : std::numeric_limits<double>::quiet_NaN();
}

inline bool json_is_number(const nlohmann::json &j, std::string key)
{
    if (!j.contains(key))
        return false;
    return j[key].is_number_integer() || j[key].is_number_unsigned() || j[key].is_number_float();
}

inline bool json_is_number(const nlohmann::json &j)
{
    return j.is_number_integer() || j.is_number_unsigned() || j.is_number_float();
}

inline void json_to_correct_dtype(token_data_t &v_data)
{
    if (v_data.index() == 2)
    {
        auto j = std::get<json_t>(v_data);
        if (json_is_number(j))
        {
            v_data = j.get<num_t>();
        }
        else if (j.is_string())
        {
            v_data = j.get<string_t>();
        }
    }
}