#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <variant>

#include "json.hpp"
#include "tools.h"
#include "my_expr_dtypes.h"

// create token struct

namespace m_parser_builtins
{

	// generic m_function type validator (check that all the args variant are a number)
	// return the arguments as a vector of doubles
	inline std::vector<num_t> m_function_validator(const token_data_t *args, const int num_args, const int expected_args)
	{
		if (args == nullptr)
		{
			throw std::runtime_error("Invalid arguments for function");
		}
		if (num_args != expected_args)
		{
			throw std::runtime_error("Invalid number of arguments for function");
		}
		std::vector<num_t> result;
		for (int i = 0; i < num_args; i++)
		{

			if (args[i].index() == 0)
			{
				result.push_back(std::get<num_t>(args[i]));
				continue;
			}
			// try to convert string to number
			else if (args[i].index() == 1)
			{
				result.push_back(stringToNumber(std::get<string_t>(args[i])));
				continue;
			}
			else if (args[i].index() == 2)
			{
				// convert the json to a number
				auto j = std::get<json_t>(args[i]);
				if (json_is_number(j))
				{
					result.push_back(j.get<num_t>());
				}
				else
				{
					result.push_back(std::numeric_limits<double>::quiet_NaN());
				}
				continue;
			}
		}
		return result;
	}

	constexpr num_t cos_f(const num_t *args) noexcept
	{
		return std::cos(args[0]);
	}

	constexpr num_t sin_f(const num_t *args) noexcept
	{
		return std::sin(args[0]);
	}

	constexpr num_t pow_f(const num_t *args) noexcept
	{
		return std::pow(args[0], args[1]);
	}
	constexpr num_t tan_f(const num_t *args) noexcept
	{
		return std::tan(args[0]);
	}
	constexpr num_t cot_f(const num_t *args) noexcept
	{
		return 1 / std::tan(args[0]);
	}
	constexpr num_t abs_f(const num_t *args) noexcept
	{
		return std::abs(args[0]);
	}
	constexpr num_t log_f(const num_t *args) noexcept
	{
		return std::log(args[0]);
	}
	constexpr num_t log10_f(const num_t *args) noexcept
	{
		return std::log10(args[0]);
	}
	constexpr num_t exp_f(const num_t *args) noexcept
	{
		return std::exp(args[0]);
	}
	constexpr num_t sqrt_f(const num_t *args) noexcept
	{
		return std::sqrt(args[0]);
	}
	constexpr num_t sinh_f(const num_t *args) noexcept
	{
		return std::sinh(args[0]);
	}
	constexpr num_t cosh_f(const num_t *args) noexcept
	{
		return std::cosh(args[0]);
	}
	constexpr num_t tanh_f(const num_t *args) noexcept
	{
		return std::tanh(args[0]);
	}
	constexpr num_t asin_f(const num_t *args) noexcept
	{
		return std::asin(args[0]);
	}
	constexpr num_t acos_f(const num_t *args) noexcept
	{
		return std::acos(args[0]);
	}
	constexpr num_t atan_f(const num_t *args) noexcept
	{
		return std::atan(args[0]);
	}
	constexpr num_t atan2_f(const num_t *args) noexcept
	{
		return std::atan2(args[0], args[1]);
	}
	constexpr num_t ceil_f(const num_t *args) noexcept
	{
		return std::ceil(args[0]);
	}
	constexpr num_t floor_f(const num_t *args) noexcept
	{
		return std::floor(args[0]);
	}
	constexpr num_t clamp_f(const num_t *args) noexcept
	{
		return std::clamp(args[0], args[1], args[2]);
	}
	constexpr num_t fac_f(const num_t *args) noexcept
	{
		return std::tgamma(args[0] + 1);
	}
	constexpr num_t max_f(const num_t *args) noexcept
	{
		return std::max(args[0], args[1]);
	}
	constexpr num_t min_f(const num_t *args) noexcept
	{
		return std::min(args[0], args[1]);
	}
	constexpr num_t round_f(const num_t *args) noexcept
	{
		return std::round(args[0]);
	}
	constexpr num_t trunc_f(const num_t *args) noexcept
	{
		return std::trunc(args[0]);
	}
	constexpr num_t fmod_f(const num_t *args) noexcept
	{
		num_t intpart = 0;
		return std::modf(args[0], &intpart);
	}
	constexpr num_t modf_f(const num_t *args) noexcept
	{
		return std::modf(args[0], nullptr);
	}
	constexpr num_t rem_f(const num_t *args) noexcept
	{
		return std::remainder(args[0], args[1]);
	}
	constexpr num_t hypot_f(const num_t *args) noexcept
	{
		return std::hypot(args[0], args[1]);
	}
}

namespace f_parser_builtins
{
	// functions that can transform any token_data_t to other token_data_t despite of the type
	inline token_data_t to_num(const token_data_t *args)
	{
		if (args[0].index() == 0)
		{
			return args[0];
		}
		else if (args[0].index() == 1)
		{
			num_t r = static_cast<num_t>(stringToNumber(std::get<string_t>(args[0])));
			return r;
		}
		else if (args[0].index() == 2)
		{
			auto j = std::get<json_t>(args[0]);
			if (json_is_number(j))
			{
				return j.get<num_t>();
			}
			else
			{
				return std::numeric_limits<num_t>::quiet_NaN();
			}
		}
		return std::numeric_limits<num_t>::quiet_NaN();
	}

	inline token_data_t to_str(const token_data_t *args)
	{
		if (args[0].index() == 0)
		{
			return std::to_string(std::get<num_t>(args[0]));
		}
		else if (args[0].index() == 1)
		{
			return args[0];
		}
		else if (args[0].index() == 2)
		{
			return std::get<json_t>(args[0]).dump();
		}
		return string_t("");
	}

	inline token_data_t to_json(const token_data_t *args)
	{
		if (args[0].index() == 0)
		{
			return json_t(std::get<num_t>(args[0]));
		}
		else if (args[0].index() == 1)
		{
			return json_t(std::get<string_t>(args[0]));
		}
		else if (args[0].index() == 2)
		{
			return args[0];
		}
		return json_t();
	}
	
	inline token_data_t len_f(const token_data_t *args){
		if(args[0].index() == 1){
			return static_cast<num_t>(std::get<string_t>(args[0]).size());
		}
		else if(args[0].index() == 2){
			return static_cast<num_t>(std::get<json_t>(args[0]).size());
		}
		return (num_t)0;
	}

	inline token_data_t sum_f(const token_data_t *args){
		if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(j.is_array()){
				num_t sum = 0;
				for(auto &v : j){
					if(v.is_number()){
						sum += v.get<num_t>();
					}
					else if(v.is_string()){
						sum += stringToNumber(v.get<string_t>());
					}
				}
				return sum;
			}
		}
		return (num_t)0;
	}
	
	inline token_data_t capitalize_f(const token_data_t *args){
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			if(s.size() > 0){
				s[0] = std::toupper(s[0]);
			}
			return s;
		}
		return string_t("");
	}

	inline token_data_t lower_f(const token_data_t *args){
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
			return s;
		}
		return string_t("");
	}

	inline token_data_t upper_f(const token_data_t *args){
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			std::transform(s.begin(), s.end(), s.begin(), ::toupper);
			return s;
		}
		return string_t("");
	}

	inline token_data_t split_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto delim = std::get<string_t>(args[1]);
			std::vector<string_t> result;
			size_t pos = 0;
			std::string token;
			while ((pos = s.find(delim)) != std::string::npos) {
				token = s.substr(0, pos);
				result.push_back(token);
				s.erase(0, pos + delim.length());
			}
			result.push_back(s);
			return result;
		}
		return json_t();
	}

	inline token_data_t join_f(const token_data_t *args){
		if(args[0].index() == 2 && args[1].index() == 1){
			auto j = std::get<json_t>(args[0]);
			auto delim = std::get<string_t>(args[1]);
			string_t result = "";
			for(auto &v : j){
				result += v.get<string_t>() + delim;
			}
			if(result.size() > 0){
				result.erase(result.size() - delim.size(), delim.size());
			}
			return result;
		}
		return string_t("");
	}

	inline token_data_t replace_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1 && args[2].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto old = std::get<string_t>(args[1]);
			auto new_ = std::get<string_t>(args[2]);
			size_t start_pos = 0;
			while((start_pos = s.find(old, start_pos)) != std::string::npos) {
				s.replace(start_pos, old.length(), new_);
				start_pos += new_.length();
			}
			return s;
		}
		return string_t("");
	}

	inline token_data_t find_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			return static_cast<num_t>(s.find(sub));
		}
		else if(args[0].index() == 2 && args[1].index() == 1){
			auto j = std::get<json_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && j[i].get<string_t>().find(sub) != std::string::npos){
					return static_cast<num_t>(i);
				}
			}
		}
		return (num_t)-1;
	}

	inline token_data_t count_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			size_t count = 0;
			size_t pos = 0;
			while ((pos = s.find(sub, pos)) != std::string::npos) {
				++count;
				pos += sub.length();
			}
			return static_cast<num_t>(count);
		}
		else if(args[0].index() == 2 && args[1].index() == 1){
			auto j = std::get<json_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			size_t count = 0;
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && j[i].get<string_t>().find(sub) != std::string::npos){
					++count;
				}
			}
			return static_cast<num_t>(count);
		}
		return (num_t)0;
	}

	inline token_data_t startswith_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			return static_cast<num_t>(s.find(sub) == 0);
		}
		else if(args[0].index() == 2 && args[1].index() == 1){
			auto j = std::get<json_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && j[i].get<string_t>().find(sub) == 0){
					return static_cast<num_t>(1);
				}
			}
		}
		return (num_t)0;
	}

	inline token_data_t endswith_f(const token_data_t *args){
		if(args[0].index() == 1 && args[1].index() == 1){
			auto s = std::get<string_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			return static_cast<num_t>(s.rfind(sub) == s.size() - sub.size());
		}
		else if(args[0].index() == 2 && args[1].index() == 1){
			auto j = std::get<json_t>(args[0]);
			auto sub = std::get<string_t>(args[1]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && j[i].get<string_t>().rfind(sub) == j[i].get<string_t>().size() - sub.size()){
					return static_cast<num_t>(1);
				}
			}
		}
		return (num_t)0;
	}

	inline token_data_t isdigit_f(const token_data_t *args){
		if(args[0].index() == 0){
			return static_cast<num_t>(1);
		}
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			return static_cast<num_t>(std::all_of(s.begin(), s.end(), ::isdigit));
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && !std::all_of(j[i].get<string_t>().begin(), j[i].get<string_t>().end(), ::isdigit)){
					return static_cast<num_t>(0);
				}
			}
			return static_cast<num_t>(1);
		}
		return (num_t)0;
	}

	inline token_data_t isalpha_f(const token_data_t *args){
		if(args[0].index() == 0){
			return static_cast<num_t>(0);
		}
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			return static_cast<num_t>(std::all_of(s.begin(), s.end(), ::isalpha));
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && !std::all_of(j[i].get<string_t>().begin(), j[i].get<string_t>().end(), ::isalpha)){
					return static_cast<num_t>(0);
				}
			}
			return static_cast<num_t>(1);
		}
		return (num_t)0;
	}

	inline token_data_t isalnum_f(const token_data_t *args){
		if(args[0].index() == 0){
			return static_cast<num_t>(0);
		}
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			return static_cast<num_t>(std::all_of(s.begin(), s.end(), ::isalnum));
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			for(size_t i = 0; i < j.size(); i++){
				if(j[i].is_string() && !std::all_of(j[i].get<string_t>().begin(), j[i].get<string_t>().end(), ::isalnum)){
					return static_cast<num_t>(0);
				}
			}
			return static_cast<num_t>(1);
		}
		return (num_t)0;
	}

	inline token_data_t isnan_f(const token_data_t *args){
		if(args[0].index() == 0){
			return static_cast<num_t>(std::isnan(std::get<num_t>(args[0])));
		}
		else if(args[0].index() == 1){
			return static_cast<num_t>(std::isnan(stringToNumber(std::get<string_t>(args[0]))));
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(json_is_number(j)){
				return static_cast<num_t>(std::isnan(j.get<num_t>()));
			}
			return static_cast<num_t>(1);
		}
		return (num_t)0;
	}

	inline token_data_t isinf_f(const token_data_t *args){
		if(args[0].index() == 0){
			return static_cast<num_t>(std::isinf(std::get<num_t>(args[0])));
		}
		else if(args[0].index() == 1){
			return static_cast<num_t>(std::isinf(stringToNumber(std::get<string_t>(args[0]))));
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(json_is_number(j)){
				return static_cast<num_t>(std::isinf(j.get<num_t>()));
			}
			return static_cast<num_t>(1);
		}
		return (num_t)0;
	}
	
	inline token_data_t reverse_f(const token_data_t *args){
		if(args[0].index() == 1){
			auto s = std::get<string_t>(args[0]);
			std::reverse(s.begin(), s.end());
			return s;
		}
		else if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(j.is_array()){
				std::reverse(j.begin(), j.end());
				return j;
			}
		}
		return json_t();
	}

	inline token_data_t sort_f(const token_data_t *args){
		if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(j.is_array()){
				std::sort(j.begin(), j.end());
				return j;
			}
		}
		return json_t();
	}

	inline token_data_t keys_f(const token_data_t *args){
		if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(j.is_object()){
				std::vector<string_t> keys;
				for(auto it = j.begin(); it != j.end(); ++it){
					keys.push_back(it.key());
				}
				return keys;
			}
		}
		return json_t();
	}

	inline token_data_t values_f(const token_data_t *args){
		if(args[0].index() == 2){
			auto j = std::get<json_t>(args[0]);
			if(j.is_object()){
				std::vector<json_t> values;
				for(auto it = j.begin(); it != j.end(); ++it){
					values.push_back(it.value());
				}
				return values;
			}
		}
		return json_t();
	}
};

namespace operators_builtins
{
	using token_data_t = std::variant<num_t, string_t, json_t>;
	enum class op_data_types
	{
		NUMBER = 0,
		STRING = 1,
		json_t = 2
	};

	inline token_data_t add_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = static_cast<op_data_types>(a.index());
		auto typeB = static_cast<op_data_types>(b.index());

		// case 1: number + number => direct sum
		// case 2: string + string => concatenate
		// case 3: json_t + json_t => merge
		// case 4: number + string => concatenate
		// case 5: number + json_t => if json_t is a number, sum, else if is an array, push_back, if is string concatenate else if is an object, error
		// case 7: string + json_t => if json_t is a number, concatenate, else if is an array, push_back, if is string concatenate else if is an object, error

		// Helper function to convert numbers to strings, handling integers cleanly
		auto num_to_string = [](num_t num)
		{
			return (std::floor(num) == num) ? std::to_string(static_cast<int64_t>(num)) : std::to_string(num);
		};

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) + std::get<num_t>(b);
		}

		if (typeA == op_data_types::STRING && typeB == op_data_types::STRING)
		{
			return std::get<string_t>(a) + std::get<string_t>(b);
		}

		if (typeA == op_data_types::json_t && typeB == op_data_types::json_t)
		{
			auto jsonA = std::get<json_t>(a);
			auto jsonB = std::get<json_t>(b);
			jsonA.merge_patch(jsonB);
			return jsonA;
		}

		if ((typeA == op_data_types::NUMBER && typeB == op_data_types::STRING) ||
			(typeA == op_data_types::STRING && typeB == op_data_types::NUMBER))
		{
			return (typeA == op_data_types::NUMBER)
					   ? num_to_string(std::get<num_t>(a)) + std::get<string_t>(b)
					   : std::get<string_t>(a) + num_to_string(std::get<num_t>(b));
		}

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::json_t)
		{
			auto jsonB = std::get<json_t>(b);
			if (jsonB.is_number())
				return std::get<num_t>(a) + jsonB.get<num_t>();
			if (jsonB.is_array())
			{
				jsonB.push_back(std::get<num_t>(a));
				return jsonB;
			}
			if (jsonB.is_string())
				return num_to_string(std::get<num_t>(a)) + jsonB.get<string_t>();
			return std::numeric_limits<num_t>::quiet_NaN();
		}

		if (typeA == op_data_types::STRING && typeB == op_data_types::json_t)
		{
			auto jsonB = std::get<json_t>(b);
			if (jsonB.is_number())
				return std::get<string_t>(a) + num_to_string(jsonB.get<num_t>());
			if (jsonB.is_array())
			{
				jsonB.push_back(std::get<string_t>(a));
				return jsonB;
			}
			if (jsonB.is_string())
				return std::get<string_t>(a) + jsonB.get<string_t>();
			return std::numeric_limits<num_t>::quiet_NaN();
		}

		if (typeA == op_data_types::json_t && typeB == op_data_types::NUMBER)
		{
			auto jsonA = std::get<json_t>(a);
			if (jsonA.is_number())
				return jsonA.get<num_t>() + std::get<num_t>(b);
			if (jsonA.is_array())
			{
				jsonA.push_back(std::get<num_t>(b));
				return jsonA;
			}
			if (jsonA.is_string())
				return jsonA.get<string_t>() + num_to_string(std::get<num_t>(b));
			return std::numeric_limits<num_t>::quiet_NaN();
		}

		if (typeA == op_data_types::json_t && typeB == op_data_types::STRING)
		{
			auto jsonA = std::get<json_t>(a);
			if (jsonA.is_number())
				return num_to_string(jsonA.get<num_t>()) + std::get<string_t>(b);
			if (jsonA.is_array())
			{
				jsonA.push_back(std::get<string_t>(b));
				return jsonA;
			}
			if (jsonA.is_string())
				return jsonA.get<string_t>() + std::get<string_t>(b);
			return std::numeric_limits<num_t>::quiet_NaN();
		}

		return std::numeric_limits<num_t>::quiet_NaN();
	}

	inline token_data_t sub_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) - std::get<num_t>(b);
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t mult_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) * std::get<num_t>(b);
		}
		else if (typeA == op_data_types::NUMBER && typeB == op_data_types::STRING)
		{
			string_t result = "";
			for (int i = 0; i < std::get<num_t>(a); i++)
			{
				result += std::get<string_t>(b);
			}
			return result;
		}
		else if (typeA == op_data_types::STRING && typeB == op_data_types::NUMBER)
		{
			string_t result = "";
			for (int i = 0; i < std::get<num_t>(b); i++)
			{
				result += std::get<string_t>(a);
			}
			return result;
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t div_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) / std::get<num_t>(b);
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t mod_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::fmod(std::get<num_t>(a), std::get<num_t>(b));
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t pow_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::pow(std::get<num_t>(a), std::get<num_t>(b));
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t eq_f(const token_data_t &a, const token_data_t &b)
	{
		return a == b;
	}

	inline token_data_t neq_f(const token_data_t &a, const token_data_t &b)
	{
		return a != b;
	}

	inline token_data_t gt_f(const token_data_t &a, const token_data_t &b)
	{
		return a > b;
	}

	inline token_data_t gte_f(const token_data_t &a, const token_data_t &b)
	{
		return a >= b;
	}

	inline token_data_t lt_f(const token_data_t &a, const token_data_t &b)
	{
		return a < b;
	}

	inline token_data_t lte_f(const token_data_t &a, const token_data_t &b)
	{
		return a <= b;
	}

	inline token_data_t and_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();

		// python aproach to non boolean values
		if (typeA == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) != 0 ? b : 0;
		}
		else if (typeA == op_data_types::STRING)
		{
			return std::get<string_t>(a) != "" ? b : string_t("");
		}
		else if (typeA == op_data_types::json_t)
		{
			return !std::get<json_t>(a).empty() ? b : json_t();
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t or_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();

		if (typeA == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) != 0 ? a : b;
		}
		else if (typeA == op_data_types::STRING)
		{
			return std::get<string_t>(a) != "" ? a : b;
		}
		else if (typeA == op_data_types::json_t)
		{
			return !std::get<json_t>(a).empty() ? a : b;
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t not_f(const token_data_t &a)
	{
		auto typeA = (op_data_types)a.index();

		if (typeA == op_data_types::NUMBER)
		{
			return static_cast<num_t>(std::get<num_t>(a) == 0 ? 1 : 0);
		}
		else if (typeA == op_data_types::STRING)
		{
			return static_cast<num_t>(std::get<string_t>(a) == "" ? 1 : 0);
		}
		else if (typeA == op_data_types::json_t)
		{
			return static_cast<num_t>(std::get<json_t>(a).empty() ? 1 : 0);
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t access_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::json_t && typeB == op_data_types::STRING)
		{
			auto jsonA = std::get<json_t>(a);
			auto key = std::get<string_t>(b);
			if (jsonA.contains(key))
			{
				return jsonA[key];
			}
			else
			{
				return json_t();
			}
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	inline token_data_t index_f(const token_data_t &a, const token_data_t &b)
	{
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		if (typeA == op_data_types::json_t && typeB == op_data_types::NUMBER)
		{
			auto jsonA = std::get<json_t>(a);
			auto index = static_cast<int>(std::get<num_t>(b));
			if (jsonA.is_array() && index >= 0 && index < jsonA.size())
			{
				return jsonA[index];
			}
			else
			{
				return json_t();
			}
		}
		else if (typeA == op_data_types::STRING && typeB == op_data_types::NUMBER)
		{
			auto strA = std::get<string_t>(a);
			auto index = static_cast<int>(std::get<num_t>(b));
			if (index >= 0 && index < strA.size())
			{
				return string_t(1, strA[index]);
			}
			else
			{
				return string_t("");
			}
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

};

struct parser_dtype
{
	token_data_t value;

	int getType() const noexcept
	{
		return value.index();
	}

	string_t toString() const
	{
		switch (value.index())
		{
		case 0:
			return std::to_string(std::get<num_t>(value));
		case 1:
			return std::get<string_t>(value);
		case 2:
			return std::get<json_t>(value).dump();
		default:
			return "";
		}
	}

	num_t toNumber() const noexcept
	{
		switch (value.index())
		{
		case 0:
			return std::get<num_t>(value);
		case 1:
			return stringToNumber(std::get<string_t>(value));
		default:
			return std::numeric_limits<num_t>::quiet_NaN();
		}
	}

	json_t toJson() const
	{
		switch (value.index())
		{
		case 0:
			return std::get<num_t>(value);
		case 1:
			return std::get<string_t>(value);
		case 2:
			return std::get<json_t>(value);
		default:
			return json_t();
		}
	}
};

class expr
{

#pragma region functions
	// built-in functions, these functions are the ones that can be used in the expression

	const std::unordered_map<string_t, m_function_info> m_functions_builtin = {
		{"pi", {[](const num_t *args) { return 3.14159265358979323846; }, 0}},
		{"e", {[](const num_t *args) { return 2.71828182845904523536; }, 0}},
		{"cos", {m_parser_builtins::cos_f, 1}},
		{"sin", {m_parser_builtins::sin_f, 1}},
		{"pow", {m_parser_builtins::pow_f, 2}},
		{"tan", {m_parser_builtins::tan_f, 1}},
		{"cot", {m_parser_builtins::cot_f, 1}},
		{"abs", {m_parser_builtins::abs_f, 1}},
		{"log", {m_parser_builtins::log_f, 1}},
		{"log10", {m_parser_builtins::log10_f, 1}},
		{"exp", {m_parser_builtins::exp_f, 1}},
		{"sqrt", {m_parser_builtins::sqrt_f, 1}},
		{"sinh", {m_parser_builtins::sinh_f, 1}},
		{"cosh", {m_parser_builtins::cosh_f, 1}},
		{"tanh", {m_parser_builtins::tanh_f, 1}},
		{"asin", {m_parser_builtins::asin_f, 1}},
		{"acos", {m_parser_builtins::acos_f, 1}},
		{"atan", {m_parser_builtins::atan_f, 1}},
		{"atan2", {m_parser_builtins::atan2_f, 2}},
		{"ceil", {m_parser_builtins::ceil_f, 1}},
		{"floor", {m_parser_builtins::floor_f, 1}},
		{"clamp", {m_parser_builtins::clamp_f, 3}},
		{"fac", {m_parser_builtins::fac_f, 1}},
		{"max", {m_parser_builtins::max_f, 2}},
		{"min", {m_parser_builtins::min_f, 2}},
		{"round", {m_parser_builtins::round_f, 1}},
		{"trunc", {m_parser_builtins::trunc_f, 1}},
		{"fmod", {m_parser_builtins::fmod_f, 2}},
		{"modf", {m_parser_builtins::modf_f, 1}},
		{"rem", {m_parser_builtins::rem_f, 2}},
		{"hypot", {m_parser_builtins::hypot_f, 2}}};

	const std::unordered_map<string_t, f_function_info> f_functions_builtin = {
		{"toNum", {f_parser_builtins::to_num, 1}},
		{"toStr", {f_parser_builtins::to_str, 1}},
		{"toJson", {f_parser_builtins::to_json, 1}},
		{"len", {f_parser_builtins::len_f, 1}},
		{"sum", {f_parser_builtins::sum_f, 1}}, ///// keys y values
		{"capitalize", {f_parser_builtins::capitalize_f, 1}},
		{"lower", {f_parser_builtins::lower_f, 1}},
		{"upper", {f_parser_builtins::upper_f, 1}},
		{"split", {f_parser_builtins::split_f, 2}},
		{"join", {f_parser_builtins::join_f, 2}},
		{"replace", {f_parser_builtins::replace_f, 3}},
		{"find", {f_parser_builtins::find_f, 2}},
		{"count", {f_parser_builtins::count_f, 2}},
		{"startswith", {f_parser_builtins::startswith_f, 2}},
		{"endswith", {f_parser_builtins::endswith_f, 2}},
		{"isalnum", {f_parser_builtins::isalnum_f, 1}},
		{"isalpha", {f_parser_builtins::isalpha_f, 1}},
		{"isdigit", {f_parser_builtins::isdigit_f, 1}},
		{"isnan", {f_parser_builtins::isnan_f, 1}},
		{"isinf", {f_parser_builtins::isinf_f, 1}},
		{"reverse", {f_parser_builtins::reverse_f, 1}},
		{"sort", {f_parser_builtins::sort_f, 1}},
		{"keys", {f_parser_builtins::keys_f, 1}},
		{"values", {f_parser_builtins::values_f, 1}}};


#pragma endregion

private:
	string_t expression_;
	token_stream_t tokens_;
	token_stream_t output_compiled_;

	std::unordered_map<string_t, token_data_t> variables_;
	std::unordered_map<string_t, f_function_info> functions_;

	function_resolver_t unknown_function_resolver_;
	bool keep_unknown_functions_;
	function_resolver_t unknown_var_resolver_;
	bool keep_unknown_vars_;

	void parse();
	token_stream_t tokenize() const;
	token_stream_t shunting_yard(const token_stream_t &tokens) const;
	inline token_data_t evaluate_postfix(const token_stream_t &postfix_tokens) const;
	token_stream_t token_resolver(const token_stream_t &tokens);

	// Map of operators and their information
	static const std::unordered_map<string_t, operator_info_t> operator_info_map_;
	static const std::unordered_set<string_t> operators_;
	static const std::unordered_set<string_t> literals_;

public:
	explicit expr(const string_t &exp) : expression_(exp) {};

	void print_tokens(const token_stream_t &tokens) const;
	token_stream_t get_tokens() const { return output_compiled_; }

	void compile();
	void set_unknown_function_resolver(function_resolver_t resolver, bool keep)
	{
		this->unknown_function_resolver_ = resolver;
		this->keep_unknown_functions_ = keep;
	}
	void set_unknown_var_resolver(function_resolver_t resolver, bool keep)
	{
		this->unknown_var_resolver_ = resolver;
		this->keep_unknown_vars_ = keep;
	}

	void set_variables(const std::unordered_map<string_t, token_data_t> &variables)
	{
		for (const auto &f : variables)
		{
			auto v_name = f.first;
			auto v_data = f.second;

			// there is some types that are not exactly num_t(double) so they are converted to nlohmann::json_abi_v3_11_2::detail::value_t::number_integer
			// so we need to convert them to num_t
			json_to_correct_dtype(v_data);
			variables_[v_name] = v_data;
		}
	}

	void set_functions(const std::unordered_map<string_t, f_function_info> &functions)
	{
		for (const auto &f : functions)
		{
			functions_[f.first] = f.second;
		}
	}

	parser_dtype eval();

	static parser_dtype eval(const string_t &expression);
};
