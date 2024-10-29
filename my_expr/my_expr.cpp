#include "my_expr.h"

#pragma region functions

const std::unordered_set<string_t> expr::operators_ = {
	"==", "!=", "<=", ">=", "&&", "||", "!", "<", ">", "+", "-", "*", "/", "%", "^"};

const std::unordered_set<string_t> expr::literals_ = {"true", "false", "null"};

// --------------------------------------------------

void expr::compile()
{
	try
	{
		auto tokens = this->tokenize();
		if (tokens.empty())
			return;
		// this->printTokens(tokens);
		auto outputQueue = this->shunting_yard(tokens);
		// this->printTokens(outputQueue);

		this->output_compiled_ = outputQueue;
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error during compilation: " << ex.what() << std::endl;
	}
}

parser_dtype expr::eval()
{
	auto result = this->evaluate_postfix(this->output_compiled_);
	return {result};
}

parser_dtype expr::eval(const string_t &expression)
{
	expr e(expression);
	e.compile();
	return e.eval();
}

void expr::print_tokens(const token_stream_t &tokens) const
{
	for (const auto &tok : tokens)
	{
		string_t tokValue;

		if (tok.value_type != data_type::NUMBER)
		{
			tokValue = std::get<string_t>(tok.value);
		}
		else
		{
			tokValue = std::to_string(std::get<double>(tok.value));
		}

		if (tok.type == token_types::LITERAL)
		{
			std::cout << "LITERAL: " << tokValue << std::endl;
		}
		else if (tok.type == token_types::OPERATOR)
		{
			std::cout << "OPERATOR: " << tokValue << std::endl;
		}
		else if (tok.type == token_types::GROUPING_OPERATOR)
		{
			std::cout << "GROUPING_OPERATOR: " << tokValue << std::endl;
		}
		else if (tok.type == token_types::FUNCTION)
		{
			std::cout << "FUNCTION: " << tokValue << std::endl;
		}
		else if (tok.type == token_types::ARGUMENT_SEPARATOR)
		{
			std::cout << "ARGUMENT_SEPARATOR: " << tokValue << std::endl;
		}
	}
	std::cout << std::endl;
};

const std::unordered_map<string_t, expr::operator_info_t> expr::operator_info_map_ = {
	{"+", {2, false}},
	{"-", {2, false}},
	{"*", {3, false}},
	{"/", {3, false}},
	{"%", {3, false}},
	{"^", {4, true}}, // La exponenciación es asociativa por la derecha
	{"==", {1, false}},
	{"!=", {1, false}},
	{"<", {1, false}},
	{"<=", {1, false}},
	{">", {1, false}},
	{">=", {1, false}},
	{"&&", {0, false}},
	{"||", {0, false}},
	{"!", {5, true}}, // Operadores unarios tienen mayor precedencia
};


#pragma endregion


#pragma region tokenization


// tokenize the expression
// Steps (in loop, order):
// 1. Skip whitespaces
// 2. Check for operators
// 3. Check for grouping operators, i.e. ( )
//   3.1. If the previous token is a function, we are in a function context TAKE CARE OF THIS
//   3.2. Otherwise, we are in a grouping context
// 4. Check for literals (variables, numbers, etc.)
//   4.1. Check if the literal is a function (if the next character is '(')
//   4.2. Check if the literal is a string (if the next character is '"' and ends with '"')
//   4.3. Else:
//   4.3.1. Check if the literal is a number (if it is a number, convert it to a number)
//   4.3.2. Else the literal is a variable
// 5. Check for ',' (argument separator) and validate that it is inside a function context
// 6. If we find an invalid character, return an empty token stream
expr::token_stream_t expr::tokenize() const
{
    std::vector<token_t> tokens;
    std::stack<bool> functionContextStack;
    const size_t length = expression_.length();
    size_t pos = 0;

    while (pos < length)
    {
        char currentChar = expression_[pos];

        // Skip whitespaces
        if (std::isspace(currentChar))
        {
            ++pos;
            continue;
        }

        // Check for operators
        bool matched = false;
        for (const auto &opStr : operators_)
        {
            size_t opLength = opStr.length();
            if (expression_.compare(pos, opLength, opStr) == 0)
            {
                tokens.emplace_back(token_types::OPERATOR, data_type::NULL_TYPE, token_data_t{opStr});

                pos += opLength;
                matched = true;
                break;
            }
        }

        if (matched)
            continue;

        // Check grouping operators (parentheses)
        if (currentChar == '(')
        {
            // If previous token is a function, we are in a function context
            functionContextStack.push(!tokens.empty() && tokens.back().type == token_types::FUNCTION);
            tokens.emplace_back(token_types::GROUPING_OPERATOR, data_type::NULL_TYPE, token_data_t(string_t("(")));
            ++pos;
            continue;
        }
        else if (currentChar == ')')
        {
            // Ensure there is a corresponding '(' in functionContextStack
            if (functionContextStack.empty())
            {
                throw std::runtime_error("Mismatched parentheses");
            }
            functionContextStack.pop();
            tokens.emplace_back(token_types::GROUPING_OPERATOR, data_type::NULL_TYPE, token_data_t(string_t(")")));
            ++pos;
            continue;
        }

        // Literals, variables, functions, and strings
        if (std::isalnum(currentChar) || currentChar == '_' || currentChar == '"')
        {
            std::string literal;
            bool isString = false;

            // If it starts with a quote, it's a string literal
            if (currentChar == '"')
            {
                isString = true;
                ++pos;
                while (pos < length && expression_[pos] != '"')
                {
                    literal += expression_[pos++];
                }
                // If end quote found, include it; otherwise, throw an error
                if (pos < length && expression_[pos] == '"')
                {
                    ++pos;
                }
                else
                {
                    throw std::runtime_error("Unterminated string literal");
                }
                tokens.emplace_back(token_types::LITERAL, data_type::STRING, token_data_t(literal));
                continue;
            }
            else
            {
                // Accumulate alphanumeric characters, underscores, and periods for literal
                while (pos < length && (std::isalnum(expression_[pos]) || expression_[pos] == '_' || expression_[pos] == '.'))
                {
                    literal += expression_[pos++];
                }

                // Check if the literal is a function (next char is '(')
                if (pos < length && expression_[pos] == '(')
                {
                    tokens.emplace_back(token_types::FUNCTION, data_type::NULL_TYPE, token_data_t(literal));
                    continue;
                }

                // Check if the literal is a number
                num_t number = stringToNumber(literal);
                if (!std::isnan(number))
                {
                    tokens.emplace_back(token_types::LITERAL, data_type::NUMBER, token_data_t(number));
                }
                else if (literals_.find(literal) != literals_.end())
                {
                    // Handle special literals (e.g., "true", "false", "null")
                    tokens.emplace_back(token_types::LITERAL, data_type::STRING, token_data_t(literal));
                }
                else
                {
                    // Otherwise, it's a variable
                    tokens.emplace_back(token_types::VARIABLE, data_type::STRING, token_data_t(literal));
                }
                continue;
            }
        }

        // Argument separator (',')
        if (currentChar == ',')
        {
            // Ensure we are inside a function context
            if (!functionContextStack.empty() && functionContextStack.top())
            {
                tokens.emplace_back(token_types::ARGUMENT_SEPARATOR, data_type::NULL_TYPE, token_data_t(string_t(",")));
                ++pos;
                continue;
            }
            else
            {
                throw std::runtime_error("Comma found outside function context");
            }
        }

        // If an invalid character is found, throw an error
        throw std::runtime_error(std::string("Invalid character in expression: ") + currentChar);
    }

    return tokens;
}

// # Shunting yard algorithm
// ### Steps:
// Create an empty output queue
// Create an empty operator stack
//
// For each token in the input stream:
//   1. If the token is a number, add it to the output queue
//   2. If the token is a variable, add it to the output queue
//   3. If the token is a function, push it onto the stack
//   4. If the token is an operator, then:
//     4.1. While there is an operator at the top of the stack with greater precedence
//     4.2. Or the operator at the top of the stack has equal precedence and is left associative
//     4.3. Pop operators from the stack onto the output queue
//     4.4. Push the current operator onto the stack
//   5. If the token is a left parenthesis, push it onto the stack
//   6. If the token is a right parenthesis:
//     6.1. Until the token at the top of the stack is a left parenthesis, pop operators from the stack onto the output queue
//     6.2. Pop the left parenthesis from the stack
//     6.3. If the token at the top of the stack is a function, pop it onto the output queue
// When there are no more tokens to read:
//  - While there are still operator tokens in the stack:
//        Pop the operator onto the output queue or throw the error
expr::token_stream_t expr::shunting_yard(const expr::token_stream_t &tokens) const
{
    token_stream_t outputQueue;
    std::stack<token_t> operatorStack;
    std::stack<int> argCountStack;
    std::stack<bool> functionContextStack;

    for (const auto &tok : tokens)
    {
        switch (tok.type)
        {
        case token_types::LITERAL:
        case token_types::VARIABLE:
            outputQueue.emplace_back(tok);
            break;

        case token_types::FUNCTION:
            operatorStack.push(tok);
            argCountStack.push(1);           // Initialize function argument count
            functionContextStack.push(true); // Enter function context
            break;

        case token_types::OPERATOR:
        {
            const string_t &op1 = std::get<string_t>(tok.value);
            auto it1 = operator_info_map_.find(op1);
            if (it1 == operator_info_map_.end())
            {
                throw std::runtime_error("Unknown operator: " + op1);
            }
            const auto &opInfo1 = it1->second;

            // Process operators based on precedence and associativity
            while (!operatorStack.empty() && operatorStack.top().type == token_types::OPERATOR)
            {
                const string_t &op2 = std::get<string_t>(operatorStack.top().value);
                auto it2 = operator_info_map_.find(op2);
                if (it2 == operator_info_map_.end())
                {
                    throw std::runtime_error("Unknown operator: " + op2);
                }
                const auto &opInfo2 = it2->second;

                if ((opInfo1.right_associative && opInfo1.precedence < opInfo2.precedence) ||
                    (!opInfo1.right_associative && opInfo1.precedence <= opInfo2.precedence))
                {
                    outputQueue.emplace_back(operatorStack.top());
                    operatorStack.pop();
                }
                else
                {
                    break;
                }
            }
            operatorStack.push(tok);
            break;
        }

        case token_types::GROUPING_OPERATOR:
        {
            const string_t &tokenValue = std::get<string_t>(tok.value);
            if (tokenValue == "(")
            {
                operatorStack.push(tok);
            }
            else if (tokenValue == ")")
            {
                // Pop until matching left parenthesis is found
                bool foundLeftParen = false;
                while (!operatorStack.empty())
                {
                    const auto &topTok = operatorStack.top();
                    if (topTok.type == token_types::GROUPING_OPERATOR && std::get<string_t>(topTok.value) == "(")
                    {
                        operatorStack.pop();
                        foundLeftParen = true;
                        break;
                    }
                    else
                    {
                        outputQueue.emplace_back(topTok);
                        operatorStack.pop();
                    }
                }
                if (!foundLeftParen)
                {
                    throw std::runtime_error("Mismatched parentheses");
                }

                // If the top of the stack is a function, pop it to the output queue
                if (!operatorStack.empty() && operatorStack.top().type == token_types::FUNCTION)
                {
                    outputQueue.emplace_back(operatorStack.top());
                    operatorStack.pop();
                    argCountStack.pop();
                    functionContextStack.pop();
                }
            }
            break;
        }

        case token_types::ARGUMENT_SEPARATOR:
        {
            // Ensure argument separator is within function context
            if (functionContextStack.empty() || !functionContextStack.top())
            {
                throw std::runtime_error("Argument separator ',' used outside of function arguments.");
            }

            // Pop all operators until a left parenthesis is found
            while (!operatorStack.empty())
            {
                const auto &topTok = operatorStack.top();
                if (topTok.type == token_types::GROUPING_OPERATOR && std::get<string_t>(topTok.value) == "(")
                {
                    break;
                }
                else
                {
                    outputQueue.emplace_back(topTok);
                    operatorStack.pop();
                }
            }

            // Increment argument count if in function context
            if (!argCountStack.empty())
            {
                ++argCountStack.top();
            }
            break;
        }

        default:
            throw std::runtime_error("Unknown token type");
        }
    }

    // Pop remaining operators from the stack
    while (!operatorStack.empty())
    {
        const auto &topTok = operatorStack.top();
        if (topTok.type == token_types::GROUPING_OPERATOR)
        {
            throw std::runtime_error("Mismatched parentheses");
        }
        outputQueue.emplace_back(topTok);
        operatorStack.pop();
    }

    return outputQueue;
}

#pragma endregion

// ## Token resolver
// Validate types and assure that the functions and variables are defined and have the correct number of arguments
// this function is called before the shuting yard algorithm
// ### Steps(Loop):
// 1. If the token is a number, add it to the output queue
// 2. If the token is a variable, add it to the output queue,
//      but check if it is defined,
//          else call the unknown symbol resolver,
//          else set the flag error and chage it to NaN
// 3. If the token is a function, push it onto the stack,
//      but check if it is defined,
//          else call the unknown symbol resolver,
//          else set the flag error and chage the sequence until close the function to NaN
// 4. If the token is an operator, add it to the output queue
expr::token_stream_t expr::token_resolver(const expr::token_stream_t &tokens)
{
    // dummy implementation
    return tokens;

    // token_stream_t outputQueue;
    // std::stack<token_t> operatorStack;
    // std::stack<int> argCountStack;
    // std::stack<bool> functionContextStack;

    // for (size_t ti = 0; ti < tokens.size(); ti++){
    //     auto tok = tokens[ti];
    //     switch (tok.type)
    //     {
    //     case token_types::LITERAL:
    //         outputQueue.emplace_back(tok);
    //         break;

    //     case token_types::VARIABLE:
    //     {
    //         const string_t &varName = std::get<string_t>(tok.value);
    //         auto var = variables_.find(varName);
    //         if (var != variables_.end())
    //         {
    //             outputQueue.emplace_back(tok);
    //         }
    //         else
    //         {
    //             if (unknown_var_resolver_)
    //             {
    //                 token_data_t value = unknown_var_resolver_(varName);
    //                 outputQueue.emplace_back(token_types::LITERAL, data_type::NUMBER, value);
    //             }
    //             else
    //             {
    //                 throw std::runtime_error("Undefined variable: " + varName);
    //             }
    //         }
    //         break;
    //     }
    //     case token_types::FUNCTION:
    //     {
    //         const string_t &funcName = std::get<string_t>(tok.value);
    //         auto func = functions_.find(funcName);
    //         if (func != functions_.end())
    //         {
    //             operatorStack.push(tok);
    //             argCountStack.push(1); // Initialize function argument count
    //             functionContextStack.push(true); // Enter function context
    //         }
    //         else
    //         {
    //             if (unknown_function_resolver_)
    //             {
    //                 token_data_t value = unknown_function_resolver_(funcName);
    //                 outputQueue.emplace_back(token_types::LITERAL, data_type::NUMBER, value);
    //             }
    //             else
    //             {
    //                 throw std::runtime_error("Undefined function: " + funcName);
    //             }
    //         }
    //         break;
    //     }

    //     default:
    //         break;
    //     }
    // }
}


// # TODO:
// 1. Evaluar funciones en el stack de operadores con los argumentos en tipado dinamico
// 2. Asegurar los tipos
token_data_t expr::evaluate_postfix(const expr::token_stream_t &postfixTokens) const
{
    std::stack<token_data_t> evaluationStack;

    for (const auto &tok : postfixTokens)
    {
        switch (tok.type)
        {
        case token_types::LITERAL:
        {
            // Convert the literal to a double and push it onto the stack
            evaluationStack.push(tok.value);
            break;
        }
        case token_types::FUNCTION:
        {
            auto func_name = std::get<string_t>(tok.value);
            auto func_m = m_functions_builtin.find(func_name);
            if(func_m != m_functions_builtin.end())
            {
                std::vector<token_data_t> args;
                for (int i = 0; i < func_m->second.num_args; i++)
                {
                    if (evaluationStack.empty())
                    {
                        throw std::runtime_error("Not enough arguments for function: " + std::get<string_t>(tok.value));
                    }
                    args.push_back(evaluationStack.top()); /////////////////////NO ESTA BIEN///////////////////////
                    evaluationStack.pop();
                }

                // Call the function and push the result back onto the stack
                std::reverse(args.begin(), args.end());
                auto args_validated = m_parser_builtins::m_function_validator(args.data(), args.size(), func_m->second.num_args);

                num_t result = func_m->second.func(args_validated.data());
                evaluationStack.push(result);
            }
            else
            {
                auto func_f = f_functions_builtin.find(func_name);
                if (func_f == f_functions_builtin.end())
                {
                    // check in functions_
                    func_f = functions_.find(func_name);
                    if (func_f == functions_.end())
                    {
                        throw std::runtime_error("Undefined function: " + func_name);
                    }
                }

                std::vector<token_data_t> args;
                for (int i = 0; i < func_f->second.num_args; i++)
                {
                    if (evaluationStack.empty())
                    {
                        throw std::runtime_error("Not enough arguments for function: " + std::get<string_t>(tok.value));
                    }
                    args.push_back(evaluationStack.top());
                    evaluationStack.pop();
                }

                // Call the function and push the result back onto the stack
                std::reverse(args.begin(), args.end());
                auto result = func_f->second.func(args.data());
                evaluationStack.push(result);
            }
            break;
        }
        case token_types::OPERATOR:
        {
            auto op = std::get<string_t>(tok.value);
            // Pop the operands
            if (evaluationStack.size() < 2)
            {
                throw std::runtime_error("Not enough operands for operator: " + std::get<string_t>(tok.value));
            }
            auto op2 = evaluationStack.top();
            evaluationStack.pop();
            auto op1 = evaluationStack.top();
            evaluationStack.pop();

            // Perform the operation
            token_data_t result = 0;
            if (op == "+")
            {
                result = operators_builtins::add_f(op1, op2);
            }
            else if (op == "-")
            {
                result = operators_builtins::sub_f(op1, op2);
            }
            else if (op == "*")
            {
                result = operators_builtins::mult_f(op1, op2);
            }
            else if (op == "/")
            {
                result = operators_builtins::div_f(op1, op2);
            }
            else if (op == "%")
            {
                result = operators_builtins::mod_f(op1, op2);
            }
            else if (op == "^")
            {
                result = operators_builtins::pow_f(op1, op2);
            }
            else if (op == "==")
            {
                result = operators_builtins::eq_f(op1, op2);
            }
            else if (op == "!=")
            {
                result = operators_builtins::neq_f(op1, op2);
            }
            else if (op == "<")
            {
                result = operators_builtins::lt_f(op1, op2);
            }
            else if (op == "<=")
            {
                result = operators_builtins::lte_f(op1, op2);
            }
            else if (op == ">")
            {
                result = operators_builtins::gt_f(op1, op2);
            }
            else if (op == ">=")
            {
                result = operators_builtins::gte_f(op1, op2);
            }
            else if (op == "&&")
            {
                result = operators_builtins::and_f(op1, op2);
            }
            else if (op == "||")
            {
                result = operators_builtins::or_f(op1, op2);
            }
            else if (op == "!")
            {
                result = operators_builtins::not_f(op1);
            }
            else
            {
                throw std::runtime_error("Unknown operator: " + op);
            }
            // Push the result back onto the stack
            evaluationStack.push(result);

            break;
        }
        case token_types::VARIABLE:
        {
            auto var_name = std::get<string_t>(tok.value);
            auto var = variables_.find(var_name);
            if (var != variables_.end())
            {
                auto a = var->second;
                evaluationStack.push(a);
            }
            else
            {
                if (unknown_var_resolver_)
                {
                    token_data_t value = unknown_var_resolver_(var_name);
                    json_to_correct_dtype(value);
                    evaluationStack.push(value);
                }
                else
                {
                    throw std::runtime_error("Undefined variable: " + var_name);
                }
                
            }
            break;
        } 
        default:
        {
            throw std::runtime_error("Unknown token type");
        }

            // The final result should be on the stack
            if (evaluationStack.size() != 1)
            {
                throw std::runtime_error("Invalid this->expression_");
            }
            return evaluationStack.top();
        }
    }
    auto result = evaluationStack.top();
    return result;
}

