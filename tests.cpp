
#include "my_expr/my_expr.h"
// #include <chrono>

#define assertion(condition, message) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " << message << ", file " << __FILE__ << ", line " << __LINE__ << std::endl; \
        std::exit(1); \
    }

auto fiumba = [](const token_data_t *args) -> token_data_t
{
    const num_t &arg = std::get<num_t>(args[0]);
    const string_t f = "fiumba";
    return token_data_t(f + std::to_string((int)arg));
};

const nlohmann::json var2 = R"(
 {
    "hola": [
        1,
        2,
        {
            "b": "HOLAAA"
        }
    ]
}

    )"_json;

const auto var = R"(
    {
        "name": "John",
        "age": 30,
        "cars": [
            {
                "name": "Ford",
                "models": [
                    "Fiesta",
                    "Focus"
                    ]
            },
            {
                "name": "BMW",
                "models": [
                    "320",
                    "X3"
                    ]
            }
        ],
        "friends":[
            "Anna",
            "Peter",
            "Tom",
            "Lucas",
            "Jenny",
            "Sandra",
            "Samantha",
            "Sophie"
        ]
    }
)"_json;


int main()
{
    // make tests for every possible situation in the parser

    std::string exp_MATH = R"(pow(2, cos(50)) * (8 + 3) * (4 / (2 - 7))  )";
    std::string exp_STRING = R"( "hola " + "mundo" )";
    std::string exp_JSON = R"( toStr(keys(var)) + keys(var)[2] )";
    std::string exp_JSON2 = R"( var.friends[len(var.cars[0].models)*2] )";

    std::string exp_DIFFICULT_UNKNOWN = R"( 2 + ("hola" + unknown_var + len(var.cars[0].models) * 2) * 9 )";
    std::string exp = R"(pow(2, cos(50)) + 8 * 4 + "hola" + fiumba(1) * ey + a.hola[ey-3].b)";
    std::string exp_INVALID = R"( 2 + )";


    auto e1 = expr(exp_MATH);
    auto e2 = expr(exp_STRING);
    auto e4 = expr(exp_JSON);
    e4.set_variables({{"var", var}});
    auto e5 = expr(exp_JSON2);
    e5.set_variables({{"var", var}});
    auto e6 = expr(exp_DIFFICULT_UNKNOWN);
    e6.set_variables({{"var", var}});
    e6.set_unknown_var_resolver([](const std::string_view &symbol) -> token_data_t {
        if (symbol == "unknown_var")
        {
            return 3.0;
        }
        return std::numeric_limits<double>::quiet_NaN();
    }, true);
    auto e7 = expr(exp_INVALID);
    auto e8 = expr(exp);
    e8.set_variables({{"ey", 5}, {"a", var2}});
    e8.set_functions({
        {"fiumba", {fiumba, 1}},
    });
    e8.set_unknown_var_resolver(
        [](const std::string_view &symbol) -> token_data_t
        {
            return 3;
        },
        true);

    e1.compile();
    e2.compile();
    e4.compile();
    e5.compile();
    e6.compile();
    e7.compile();
    e8.compile();

    std::cout << "Result MATH: " << e1.eval().toString() << ", " << std::pow(2, std::cos(50)) * (8.0 + 3.0) * (4.0 / (2.0 - 7.0)) << std::endl;
    std::cout << "Result STRING: " << e2.eval().toString() << std::endl;
    std::cout << "Result JSON: " << e4.eval().toString() << std::endl;
    std::cout << "Result JSON2: " << e5.eval().toString() << ", " << var["friends"][2 * (3 - 2)].get<std::string>() << std::endl;
    std::cout << "Result DIFFICULT_UNKNOWN: " << e6.eval().toString() << ", " << std::endl;
    std::cout << "Result: " << e8.eval().toString() << std::endl;
    // std::cout << "Result INVALID: " << e7.eval().toString() << std::endl;

    return 0;
}