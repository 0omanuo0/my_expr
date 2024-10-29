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
		auto typeA = (op_data_types)a.index();
		auto typeB = (op_data_types)b.index();

		// case 1: number + number => direct sum
		// case 2: string + string => concatenate
		// case 3: json_t + json_t => merge
		// case 4: number + string => concatenate
		// case 5: number + json_t => if json_t is a number, sum, else if is an array, push_back, if is string concatenate else if is an object, error
		// case 7: string + json_t => if json_t is a number, concatenate, else if is an array, push_back, if is string concatenate else if is an object, error

		if (typeA == op_data_types::NUMBER && typeB == op_data_types::NUMBER)
		{
			return std::get<num_t>(a) + std::get<num_t>(b);
		}
		else if (typeA == op_data_types::STRING && typeB == op_data_types::STRING)
		{
			return std::get<string_t>(a) + std::get<string_t>(b);
		}
		else if (typeA == op_data_types::json_t && typeB == op_data_types::json_t)
		{
			auto jsonA = std::get<json_t>(a);
			auto jsonB = std::get<json_t>(b);
			jsonA.merge_patch(jsonB);
			return jsonA;
		}
		else if (typeA == op_data_types::NUMBER && typeB == op_data_types::STRING)
		{
			return std::to_string(std::get<num_t>(a)) + std::get<string_t>(b);
		}
		else if (typeA == op_data_types::NUMBER && typeB == op_data_types::json_t)
		{
			auto jsonB = std::get<json_t>(b);
			if (jsonB.is_number())
			{
				return std::get<num_t>(a) + jsonB.get<num_t>();
			}
			else if (jsonB.is_array())
			{
				jsonB.push_back(std::get<num_t>(a));
				return jsonB;
			}
			else if (jsonB.is_string())
			{
				return std::to_string(std::get<num_t>(a)) + jsonB.get<string_t>();
			}
			else
			{
				return std::numeric_limits<num_t>::quiet_NaN();
			}
		}
		else if (typeA == op_data_types::STRING && typeB == op_data_types::NUMBER)
		{
			return std::get<string_t>(a) + std::to_string(std::get<num_t>(b));
		}
		else if (typeA == op_data_types::STRING && typeB == op_data_types::json_t)
		{
			auto jsonB = std::get<json_t>(b);
			if (jsonB.is_number())
			{
				return std::get<string_t>(a) + std::to_string(jsonB.get<num_t>());
			}
			else if (jsonB.is_array())
			{
				jsonB.push_back(std::get<string_t>(a));
				return jsonB;
			}
			else if (jsonB.is_string())
			{
				return std::get<string_t>(a) + jsonB.get<string_t>();
			}
			else
			{
				return std::numeric_limits<num_t>::quiet_NaN();
			}
		}
		else
		{
			return std::numeric_limits<num_t>::quiet_NaN();
		}
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
		auto typeB = (op_data_types)b.index();

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
		auto typeB = (op_data_types)b.index();

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
			return std::get<num_t>(a) == 0 ? 1 : 0;
		}
		else if (typeA == op_data_types::STRING)
		{
			return std::get<string_t>(a) == "" ? 1 : 0;
		}
		else if (typeA == op_data_types::json_t)
		{
			return std::get<json_t>(a).empty() ? 1 : 0;
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

	// math functions that can be used in the expression (needs to be validated that are numbers)
	using m_generic_function = num_t (*)(const num_t *args);

	struct m_function_info
	{
		m_generic_function func;
		int num_args;
	};

	const std::unordered_map<string_t, m_function_info> m_functions_builtin = {
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

	// functions that can transform any token_data_t to other token_data_t despite of the type
	using f_generic_function = token_data_t (*)(const token_data_t *args);

	struct f_function_info
	{
		f_generic_function func;
		int num_args;
	};

	const std::unordered_map<string_t, f_function_info> f_functions_builtin = {
		{"toNum", {f_parser_builtins::to_num, 1}},
		{"toStr", {f_parser_builtins::to_str, 1}},
		{"toJon", {f_parser_builtins::to_json, 1}}};

#pragma endregion

private:
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
	};

	using token_stream_t = std::vector<token_t>;
	using function_resolver_t = std::function<token_data_t(const std::string_view &symbol)>;

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

	void print_tokens(const token_stream_t &tokens) const;

	// Map of operators and their information
	static const std::unordered_map<string_t, operator_info_t> operator_info_map_;
	static const std::unordered_set<string_t> operators_;
	static const std::unordered_set<string_t> literals_;

public:
	explicit expr(const string_t &exp) : expression_(exp) {};

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



