#include "my_expr.h"

#pragma region functions

const std::unordered_set<string_t> expr::operators_ = {
    "==", "!=", "<=", ">=", "&&", "||", "!", "<", ">", "+", "-", "*", "/", "%", "^", ".", "[]"};

const std::unordered_set<string_t> expr::literals_ = {"true", "false", "null"};

// --------------------------------------------------

void expr::compile()
{
    try
    {
        auto tokens = this->tokenize();
        if (tokens.empty())
            return;
        // this->print_tokens(tokens);
        auto outputQueue = this->shunting_yard(tokens);
        // this->print_tokens(outputQueue);

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
        else if (tok.type == token_types::VARIABLE)
        {
            std::cout << "VARIABLE: " << tokValue << std::endl;
        }
        else if (tok.type == token_types::ARGUMENT_SEPARATOR)
        {
            std::cout << "ARGUMENT_SEPARATOR: " << tokValue << std::endl;
        }
    }
    std::cout << std::endl;
};

const std::unordered_map<string_t, operator_info_t> expr::operator_info_map_ = {
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

#pragma endregion

#pragma region tokenization

// tokenize the expression
// Steps (in loop, order):
// 1. Skip whitespaces
// 2. Check for operators
// 3. Check for grouping operators, i.e. ( ), [ ]
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
token_stream_t expr::tokenize() const
{
    std::vector<token_t> tokens;
    std::stack<bool> functionContextStack;
    std::stack<char> groupingContextStack; // To track both '(' and '['
    const size_t length = expression_.length();
    size_t pos = 0;
    bool isNegative = false;

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
        //
        bool matched = false;
        for (const auto &opStr : operators_)
        {
            size_t opLength = opStr.length();
            if (expression_.compare(pos, opLength, opStr) == 0)
            {
                // if - is start, or previous token is an operator, then is a negative number
                isNegative = opStr == "-" && (tokens.empty() || tokens.back().type == token_types::OPERATOR);
                if (!isNegative)
                {
                    tokens.emplace_back(token_types::OPERATOR, data_type::NULL_TYPE, token_data_t{opStr});
                }

                pos += opLength;
                matched = true;
                break;
            }
        }
        if (matched)
            continue;

        // Check grouping operators (parentheses and brackets)
        if (currentChar == '(' || currentChar == '[')
        {
            // For '(' check if previous token is a function
            if (currentChar == '(')
            {
                bool isFunctionContext = !tokens.empty() && tokens.back().type == token_types::FUNCTION;
                functionContextStack.push(isFunctionContext);
            }

            groupingContextStack.push(currentChar);
            tokens.emplace_back(token_types::GROUPING_OPERATOR, data_type::NULL_TYPE, token_data_t(string_t(1, currentChar)));
            ++pos;
            continue;
        }
        else if (currentChar == ')' || currentChar == ']')
        {
            // Ensure there is a corresponding opening symbol
            if (groupingContextStack.empty() ||
                (currentChar == ')' && groupingContextStack.top() != '(') ||
                (currentChar == ']' && groupingContextStack.top() != '['))
            {
                throw std::runtime_error("Mismatched grouping symbols");
            }

            // Pop the opening symbol
            groupingContextStack.pop();

            // Pop function context if necessary
            if (currentChar == ')' && !functionContextStack.empty() && functionContextStack.top())
            {
                functionContextStack.pop();
            }

            tokens.emplace_back(token_types::GROUPING_OPERATOR, data_type::NULL_TYPE, token_data_t(string_t(1, currentChar)));
            ++pos;
            continue;
        }

        // Literals, variables, functions, and strings
        if (std::isdigit(currentChar) || currentChar == '.' || std::isalpha(currentChar) || currentChar == '_' || currentChar == '"' || currentChar == '\'')
        {
            // Handle string literals
            if (currentChar == '"')
            {
                std::string literal;
                ++pos;
                while (pos < length && expression_[pos] != '"')
                {
                    literal += expression_[pos++];
                }

                if (pos < length && expression_[pos] == '"')
                    ++pos;
                else
                    throw std::runtime_error("Unterminated string literal");
                tokens.emplace_back(token_types::LITERAL, data_type::STRING, token_data_t(literal));
                continue;
            }

            if (currentChar == '\'')
            {
                std::string literal;
                ++pos;
                while (pos < length && expression_[pos] != '\'')
                {
                    literal += expression_[pos++];
                }

                if (pos < length && expression_[pos] == '\'')
                    ++pos;
                else
                    throw std::runtime_error("Unterminated string literal");
                tokens.emplace_back(token_types::LITERAL, data_type::STRING, token_data_t(literal));
                continue;
            }

            // Handle numbers and variables
            if (std::isdigit(currentChar) || currentChar == '.')
            {
                // Parse number
                std::string numberStr;
                bool decimalPointEncountered = false;

                while (pos < length && (std::isdigit(expression_[pos]) || expression_[pos] == '.'))
                {
                    if (expression_[pos] == '.')
                    {
                        if (decimalPointEncountered)
                        {
                            throw std::runtime_error("Invalid number format: multiple decimal points");
                        }
                        decimalPointEncountered = true;
                    }
                    numberStr += expression_[pos++];
                }

                // Ensure number is valid
                num_t number = stringToNumber2(numberStr);
                if (!std::isnan(number))
                {
                    number = isNegative ? -number : number;
                    isNegative = false;
                    tokens.emplace_back(token_types::LITERAL, data_type::NUMBER, token_data_t(number));
                }
                else
                    throw std::runtime_error("Invalid number format");

                continue;
            }
            else // Handle variables and functions
            {
                std::string identifier;
                while (pos < length && (std::isalnum(expression_[pos]) || expression_[pos] == '_'))
                {
                    identifier += expression_[pos++];
                }

                // Check if the identifier is a function (next char is '(')
                if (pos < length && expression_[pos] == '(')
                {
                    tokens.emplace_back(token_types::FUNCTION, data_type::NULL_TYPE, token_data_t(identifier));
                    continue;
                }

                // Handle literals like "true", "false", "null" else are variables
                if (literals_.find(identifier) != literals_.end())
                    tokens.emplace_back(token_types::LITERAL, data_type::STRING, token_data_t(identifier));
                else
                    tokens.emplace_back(token_types::VARIABLE, data_type::STRING, token_data_t(identifier));

                continue;
            }
        }

        // Argument separator (',')
        if (currentChar == ',')
        {
            // Ensure we are inside a function context or bracket context
            if ((!functionContextStack.empty() && functionContextStack.top()) ||
                (!groupingContextStack.empty() && groupingContextStack.top() == '['))
            {
                tokens.emplace_back(token_types::ARGUMENT_SEPARATOR, data_type::NULL_TYPE, token_data_t(string_t(",")));
                ++pos;
                continue;
            }
            else
                throw std::runtime_error("Comma found outside function or bracket context");
        }

        // If an invalid character
        throw std::runtime_error(std::string("Invalid character in expression: ") + currentChar);
    }

    // At the end, ensure all grouping symbols are matched
    if (!groupingContextStack.empty())
        throw std::runtime_error("Unmatched grouping symbols in expression");

    return tokens;
}



// # Shunting Yard Algorithm
// ### Steps:
// Create an empty output queue
// Create an empty operator stack
//
// For each token in the input stream:
//   1. If the token is a number, add it to the output queue
//   2. If the token is a variable, add it to the output queue
//   3. If the token is a function:
//       3.1. Push it onto the operator stack
//       3.2. Initialize the function argument count to 1
//       3.3. Mark the context as within a function
//   4. If the token is an operator, then:
//       4.1. While there is an operator at the top of the stack with greater precedence
//       4.2. Or the operator at the top of the stack has equal precedence and is left associative
//       4.3. Pop operators from the stack onto the output queue
//       4.4. Push the current operator onto the stack
//   5. If the token is a left parenthesis, push it onto the stack
//   6. If the token is a right parenthesis:
//       6.1. Until the token at the top of the stack is a left parenthesis, pop operators from the stack onto the output queue
//       6.2. Pop the left parenthesis from the stack
//       6.3. If the token at the top of the stack is a function, pop it onto the output queue
//       6.4. Exit the function context if applicable
//   7. If the token is a left bracket ("["):
//       7.1. Before pushing it, process operators on the stack with greater or equal precedence
//       7.2. Push the bracket onto the stack
//   8. If the token is a right bracket ("]"):
//       8.1. Until the token at the top of the stack is a left bracket, pop operators from the stack onto the output queue
//       8.2. Pop the left bracket from the stack
//       8.3. Add the "[]" indexing operator to the stack
//   9. If the token is an argument separator (","):
//       9.1. Ensure it is within a function or index context
//       9.2. Pop operators from the stack onto the output queue until a left parenthesis or bracket is encountered
//       9.3. Increment the argument count for the current function if applicable
//
// When there are no more tokens to read:
//  - While there are still operator tokens in the stack:
//        Pop the operator onto the output queue
//        If the token is a grouping symbol (e.g., parenthesis or bracket), throw an error
token_stream_t expr::shunting_yard(const token_stream_t &tokens) const
{
    token_stream_t outputQueue;
    std::stack<token_t> operatorStack;
    std::stack<int> argCountStack;
    std::stack<bool> functionContextStack;

    // Helper function to get operator info
    auto get_operator_info = [this](const string_t &op) -> const operator_info_t &
    {
        auto it = operator_info_map_.find(op);
        if (it == operator_info_map_.end())
        {
            throw std::runtime_error("Unknown operator: " + op);
        }
        return it->second;
    };

    // Helper function to process operator stack based on precedence and associativity
    auto process_operator_stack = [&](const string_t &currentOp, const operator_info_t &currentOpInfo)
    {
        while (!operatorStack.empty() && operatorStack.top().type == token_types::OPERATOR)
        {
            const string_t &opOnStack = std::get<string_t>(operatorStack.top().value);
            const auto &opOnStackInfo = get_operator_info(opOnStack);

            if ((opOnStackInfo.precedence > currentOpInfo.precedence) ||
                (opOnStackInfo.precedence == currentOpInfo.precedence && !currentOpInfo.right_associative))
            {
                outputQueue.emplace_back(operatorStack.top());
                operatorStack.pop();
            }
            else
            {
                break;
            }
        }
    };

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
            const auto &opInfo1 = get_operator_info(op1);
            process_operator_stack(op1, opInfo1);
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
                // Pop operators until matching '(' is found
                bool foundMatchingOpening = false;
                while (!operatorStack.empty())
                {
                    if (operatorStack.top().type == token_types::GROUPING_OPERATOR &&
                        std::get<string_t>(operatorStack.top().value) == "(")
                    {
                        operatorStack.pop();
                        foundMatchingOpening = true;
                        break;
                    }
                    else
                    {
                        outputQueue.emplace_back(operatorStack.top());
                        operatorStack.pop();
                    }
                }
                if (!foundMatchingOpening)
                    throw std::runtime_error("Mismatched parentheses");

                // If the token at the top of the stack is a function, pop it onto the output queue
                if (!operatorStack.empty() && operatorStack.top().type == token_types::FUNCTION)
                {
                    outputQueue.emplace_back(operatorStack.top());
                    operatorStack.pop();
                    argCountStack.pop();
                    functionContextStack.pop();
                }
            }
            else if (tokenValue == "[")
            {
                const auto &opInfo1 = get_operator_info("[");
                process_operator_stack("[", opInfo1);
                operatorStack.push(tok);
            }
            else if (tokenValue == "]")
            {
                // Pop operators until matching '[' is found
                bool foundMatchingOpening = false;
                while (!operatorStack.empty())
                {
                    if (operatorStack.top().type == token_types::GROUPING_OPERATOR &&
                        std::get<string_t>(operatorStack.top().value) == "[")
                    {
                        operatorStack.pop();
                        foundMatchingOpening = true;
                        break;
                    }
                    else
                    {
                        outputQueue.emplace_back(operatorStack.top());
                        operatorStack.pop();
                    }
                }
                if (!foundMatchingOpening)
                {
                    throw std::runtime_error("Mismatched brackets");
                }

                // Add the '[]' postfix operator to the output queue
                token_t indexOperator(token_types::OPERATOR, data_type::NULL_TYPE, string_t("[]"));
                const auto &opInfo1 = get_operator_info("[]");
                process_operator_stack("[]", opInfo1);
                operatorStack.push(indexOperator);
            }
            break;
        }

        case token_types::ARGUMENT_SEPARATOR:
        {
            // Ensure argument separator is within function or index context
            if ((!functionContextStack.empty() && functionContextStack.top()) ||
                (!operatorStack.empty() && operatorStack.top().type == token_types::GROUPING_OPERATOR &&
                 std::get<string_t>(operatorStack.top().value) == "["))
            {
                // Pop operators until a left parenthesis or bracket is found
                while (!operatorStack.empty())
                {
                    const auto &topTok = operatorStack.top();
                    if (topTok.type == token_types::GROUPING_OPERATOR &&
                        (std::get<string_t>(topTok.value) == "(" || std::get<string_t>(topTok.value) == "["))
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
                if (!argCountStack.empty() && functionContextStack.top())
                {
                    ++argCountStack.top();
                }
            }
            else
                throw std::runtime_error("Argument separator ',' used outside of function or index arguments.");
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
            throw std::runtime_error("Mismatched grouping symbols");
        }
        outputQueue.emplace_back(topTok);
        operatorStack.pop();
    }

    return outputQueue;
}

#pragma endregion

// # TODO:
// 1. Evaluar funciones en el stack de operadores con los argumentos en tipado dinamico
// 2. Asegurar los tipos
token_data_t expr::evaluate_postfix(const token_stream_t &postfixTokens) const
{
    std::stack<token_t> evaluationStack;

    auto resolve_operand = [&](const token_t &operand) -> token_data_t
    {
        if (operand.type == token_types::LITERAL)
        {
            return operand.value;
        }
        else if (operand.type == token_types::VARIABLE)
        {
            auto var_name = std::get<string_t>(operand.value);
            auto var = variables_.find(var_name);
            if (var != variables_.end())
            {
                return var->second;
            }
            else
            {
                if (unknown_var_resolver_)
                {
                    token_data_t value = unknown_var_resolver_(var_name);
                    json_to_correct_dtype(value);
                    return value;
                }
                else
                {
                    throw std::runtime_error("Undefined variable: " + var_name);
                }
            }
        }
        else
            return std::numeric_limits<num_t>::quiet_NaN();
    };

    for (const auto &tok : postfixTokens)
    {
        switch (tok.type)
        {
        case token_types::VARIABLE:
        case token_types::LITERAL:
        {
            // Convert the literal to a double and push it onto the stack
            evaluationStack.push(tok);
            break;
        }
        case token_types::FUNCTION:
        {
            auto func_name = std::get<string_t>(tok.value);
            auto func_m = m_functions_builtin.find(func_name);
            if (func_m != m_functions_builtin.end())
            {
                std::vector<token_data_t> args;
                for (int i = 0; i < func_m->second.num_args; i++)
                {
                    if (evaluationStack.empty())
                    {
                        throw std::runtime_error("Not enough arguments for function: " + std::get<string_t>(tok.value));
                    }
                    args.push_back(resolve_operand(evaluationStack.top()));
                    // args.push_back(evaluationStack.top()); /////////////////////NO ESTA BIEN///////////////////////
                    evaluationStack.pop();
                }

                // Call the function and push the result back onto the stack
                std::reverse(args.begin(), args.end());
                auto args_validated = m_parser_builtins::m_function_validator(args.data(), args.size(), func_m->second.num_args);

                num_t result = func_m->second.func(args_validated.data());
                auto result_t = token_t(token_types::LITERAL, data_type::NUMBER, result);
                evaluationStack.push(result_t);
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
                    args.push_back(resolve_operand(evaluationStack.top()));
                    // args.push_back(evaluationStack.top());
                    evaluationStack.pop();
                }

                // Call the function and push the result back onto the stack
                std::reverse(args.begin(), args.end());
                auto result = func_f->second.func(args.data());
                auto dtype = result.index();
                auto result_t = token_t(token_types::LITERAL, (data_type)dtype, result);
                evaluationStack.push(result_t);
            }
            break;
        }
        case token_types::OPERATOR:
        {
            auto op = std::get<string_t>(tok.value);
            // Pop the operands
            if (evaluationStack.size() < operator_info_map_.at(op).num_args)
            {
                throw std::runtime_error("Not enough operands for operator: " + std::get<string_t>(tok.value));
            }
            token_data_t op2 = evaluationStack.top().value;
            if (op != ".")
                op2 = resolve_operand(evaluationStack.top());
            evaluationStack.pop();
            auto op1 = resolve_operand(evaluationStack.top());
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
                result = operators_builtins::not_f(op2);
                evaluationStack.push(token_t(token_types::LITERAL, (data_type)op1.index(), op1));
            }
            else if (op == ".")
            {
                result = operators_builtins::access_f(op1, op2);
            }
            else if (op == "[]")
            {
                result = operators_builtins::index_f(op1, op2);
            }
            else
            {
                throw std::runtime_error("Unknown operator: " + op);
            }
            // Push the result back onto the stack
            auto dtype = result.index();
            auto result_t = token_t(token_types::LITERAL, (data_type)dtype, result);
            evaluationStack.push(result_t);

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
            return evaluationStack.top().value;
        }
    }

    // if the top is a variable, resolve it
    if (evaluationStack.size() == 1 && evaluationStack.top().type == token_types::VARIABLE)
    {
        return resolve_operand(evaluationStack.top());
    }
    return evaluationStack.top().value;
}
