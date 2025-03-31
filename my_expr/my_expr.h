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
#include "my_expr_functions.hpp"

// create token struct

class parser_dtype
{
public:
	token_data_t value;

	int getType() const noexcept
	{
		return value.index();
	}

	bool isNumber() const noexcept
	{
		if (value.index() == 0)
		{
			return true;
		}
		else if (value.index() == 2)
		{
			return json_is_number(std::get<json_t>(value));
		}
	}

	bool isString() const noexcept
	{
		if (value.index() == 1)
		{
			return true;
		}
		else if (value.index() == 2)
		{
			return std::get<json_t>(value).is_string();
		}
		return false;
	}

	bool isJson() const noexcept
	{
		return value.index() == 2;
	}

	bool is_array() const noexcept
	{
		if (value.index() == 2)
		{
			return std::get<json_t>(value).is_array();
		}
		return false;
	}

	bool is_object() const noexcept
	{
		if (value.index() == 2)
		{
			return std::get<json_t>(value).is_object();
		}
		return false;
	}

	string_t toString(bool no_quotes = false) const
	{
		switch (value.index())
		{
		case 0:
		{
			// if is int return whitout decimals comparing with the floor value
			auto a = std::get<num_t>(value);
			return (std::floor(a) == a) ? std::to_string(static_cast<int64_t>(a)) : std::to_string(a);
		}
		case 1:
		{
			auto a = std::get<string_t>(value);
			if (no_quotes)
				return (char)a[0] == '"' ? a.substr(1, a.size() - 2) : a;
			return a;
		}
		case 2:
		{
			auto a = std::get<json_t>(value).dump();
			if (no_quotes)
				return (char)a[0] == '"' ? a.substr(1, a.size() - 2) : a;

			return a;
		}
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
			return stringToNumber2(std::get<string_t>(value));
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
	void compile(const string_t &exp)
	{
		expression_ = exp;
		compile();
	}
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

	expr &operator=(const expr &other)
	{
		if (this == &other)
			return *this;
		expression_ = other.expression_;
		variables_ = other.variables_;
		functions_ = other.functions_;
		return *this;
	}

	void set_variables(const std::unordered_map<string_t, token_data_t> &variables)
	{
		if (variables.empty())
		{
			return;
		}
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

inline const std::unordered_set<string_t> expr::operators_ = {
	"==", "!=", "<=", ">=", "&&", "||", "!", "<", ">", "+", "-", "*", "/", "%", "^", ".", "[]"};

inline const std::unordered_set<string_t> expr::literals_ = {"true", "false", "null"};

inline const std::unordered_map<string_t, operator_info_t> expr::operator_info_map_ = {
	{"+", {2, false, 2}},
	{"-", {2, false, 2}},
	{"*", {3, false, 2}},
	{"/", {3, false, 2}},
	{"%", {3, false, 2}},
	{"^", {4, true, 2}}, // La exponenciación es asociativa por la derecha
	{"==", {1, false, 2}},
	{"!=", {1, false, 2}},
	{"<", {1, false, 2}},
	{"<=", {1, false, 2}},
	{">", {1, false, 2}},
	{">=", {1, false, 2}},
	{"&&", {0, false, 2}},
	{"||", {0, false, 2}},
	{"!", {5, true, 1}}, // Operadores unarios tienen mayor precedencia
	{".", {7, false, 2}},
	{"[", {7, false, 2}},
	{"[]", {7, false, 2}}};
