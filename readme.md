# expr Class Documentation

The `expr` class is a powerful expression parser that allows users to evaluate complex mathematical and string manipulation expressions. It supports various built-in functions and can be extended with custom functions and variables.

## How it works

The `expr` class takes a string input representing the expression to be evaluated. This expression can contain mathematical operations, string manipulations, and calls to built-in or user-defined functions. The class tokenizes the input string, applies the Shunting-yard algorithm for parsing, and then evaluates the expression in postfix notation.

## Built-in Functions

The `expr` class comes with a set of built-in functions for both mathematical and string manipulation operations. These include:

### Mathematical Functions

| Function Name | Description | Argument Count |
|-------------|-------------|---------------|
| `pi` | Returns the value of Pi. | 0 |
| `e` | Returns the base of the natural logarithm. | 0 |
| `cos`, `sin`, `tan`, `cot`, `asin`, `acos`, `atan`, `atan2` | Trigonometric functions. | 1-2 |
| `pow` | Raises the first argument to the power of the second argument. | 2 |
| `abs` | Returns the absolute value of the argument. | 1 |
| `log`, `log10` | Natural and base-10 logarithms. | 1 |
| `exp` | Exponential function. | 1 |
| `sqrt` | Square root. | 1 |
| `sinh`, `cosh`, `tanh` | Hyperbolic sine, cosine, and tangent. | 1 |
| `ceil`, `floor` | Ceiling and floor functions. | 1 |
| `clamp` | Clamps a value between two limits. | 3 |
| `fac` | Factorial. | 1 |
| `max`, `min` | Maximum and minimum of two values. | 2 |
| `round`, `trunc` | Rounding and truncation functions. | 1 |
| `fmod`, `modf`, `rem` | Floating-point remainder functions. | 2 |
| `hypot` | Euclidean norm, sqrt(sum(squares)), the length of the vector from the origin to point (x, y) == sqrt(x^2 + y^2). | 2 |

### String Functions

| Function Name | Description | Argument Count |
|-------------|-------------|---------------|
| `toNum`, `toStr`, `toJson` | Convert a value to a number, string, or JSON object. | 1 |
| `len` | Length of a string or array. | 1 |
| `sum` | Sum of elements in an array. | 1 |
| `capitalize`, `lower`, `upper` | Change the case of a string. | 1 |
| `split`, `join` | Split a string into an array or join an array into a string. | 2 |
| `replace` | Replace a substring in a string. | 3 |
| `find`, `count` | Find the position or count of a substring in a string. | 2 |
| `startswith`, `endswith` | Check if a string starts or ends with a substring. | 2 |
| `isalnum`, `isalpha`, `isdigit`, `isnan`, `isinf` | Check if a string is alphanumeric, alphabetic, numeric, NaN, or infinity. | 1 |
| `reverse`, `sort`, `keys`, `values` | Reverse, sort, get keys, or get values from an array or object. | 1 |

## Custom Functions and Variables

Users can extend the functionality of the `expr` class by setting custom functions and variables using the `set_functions` and `set_variables` methods. This allows for more complex and specific operations to be performed within the expression parser.

## Examples of Usage

### Setting Custom Variables

```cpp
std::unordered_map<string_t, token_data_t> custom_variables = {
    {"x", 5},
    {"y", 10}
};

expr parser("x + y");
parser.set_variables(custom_variables);
parser.compile();

std::cout << parser.eval() << std::endl; // Outputs: 15
```

### Setting Custom Functions

```cpp
std::unordered_map<string_t, f_function_info> custom_functions = {
    {"custom_func", {[](const num_t *args) { return args[0] * args[1]; }, 2}}
};

expr parser("custom_func(2, 3)");
parser.set_functions(custom_functions);
parser.compile();

std::cout << parser.eval() << std::endl; // Outputs: 6
```

### Using Built-in Functions

```cpp
expr parser("sin(pi/2)");
parser.compile();

std::cout << parser.eval() << std::endl; // Outputs: 1
```

### Using Resolver

```cpp
auto resolver = [](const string_t &name) {
    if (name == "custom_var") {
        return token_data_t(42);
    }
    throw std::runtime_error("Unknown variable: " + name);
};

expr parser("custom_var + 5");
parser.set_unknown_var_resolver(resolver, true);
parser.compile();

std::cout << parser.eval() << std::endl; // Outputs: 47
```


## Performance Benchmarks

### Experimental Results Summary


- **Basic Operations**: Simple arithmetic (`1 + 2`), variable arithmetic (`a * b + c`)
- **Mathematical Functions**: Square root, trigonometric operations, logarithmic and exponential functions
- **String Manipulation**: Concatenation, replacement, and splitting
- **JSON Access**: Length retrieval, array access, nested JSON element access


### Hardware and Software Specifications
The experiments were run on a machine with the following specifications:
- **OS**: Ubuntu 22.04.5 LTS x86_64
- **CPU**: Intel Xeon E5-2680 v4 (20 cores) @ 3.300GHz
- **GPU**: NVIDIA GeForce GTX 1050 Ti
- **Memory**: 24 GB
- **Kernel**: 6.8.12-1-pve

| All the tests are compiled with the next flags: `/usr/bin/g++ -O3 -ldl -rdynamic`

## Summary of Test Cases

### Simple Addition
- **Expression**: `1 + 2`
- **Compilation Time**: 107 µs
- **Total Evaluation Time (100000 iterations)**: 388,377 µs
- **Average Evaluation Time per Iteration**: 3.88377 µs
- **Direct Computation Result**: `3`
- **Total Direct Computation Time (100000 iterations)**: 1,302,466 µs
- **Average Direct Computation Time per Iteration**: 13.0247 µs
- **Overhead per Evaluation**: -9.14089 µs

---

### Variable Arithmetic
- **Expression**: `a * b + c`
- **Compilation Time**: 119 µs
- **Total Evaluation Time (100000 iterations)**: 793,645 µs
- **Average Evaluation Time per Iteration**: 7.93645 µs
- **Direct Computation Result**: `10.1578`
- **Total Direct Computation Time (100000 iterations)**: 1,313,248 µs
- **Average Direct Computation Time per Iteration**: 13.1325 µs
- **Overhead per Evaluation**: -5.19603 µs

---

### Square Root Calculation
- **Expression**: `sqrt(a^2 + b^2)`
- **Compilation Time**: 126 µs
- **Total Evaluation Time (100000 iterations)**: 1,211,000 µs
- **Average Evaluation Time per Iteration**: 12.11 µs
- **Direct Computation Result**: `4.15435`
- **Total Direct Computation Time (100000 iterations)**: 1,306,414 µs
- **Average Direct Computation Time per Iteration**: 13.0641 µs
- **Overhead per Evaluation**: -0.95414 µs

---

### Trigonometric Operations
- **Expression**: `sin(a) * cos(b) + tan(c)`
- **Compilation Time**: 140 µs
- **Total Evaluation Time (100000 iterations)**: 1,377,004 µs
- **Average Evaluation Time per Iteration**: 13.77 µs
- **Direct Computation Result**: `-21.1538`
- **Total Direct Computation Time (100000 iterations)**: 1,335,546 µs
- **Average Direct Computation Time per Iteration**: 13.3555 µs
- **Overhead per Evaluation**: 0.41458 µs

---

### Logarithmic and Exponential
- **Expression**: `log(a) + exp(b) - pow(c, 2)`
- **Compilation Time**: 143 µs
- **Total Evaluation Time (100000 iterations)**: 1,545,875 µs
- **Average Evaluation Time per Iteration**: 15.4588 µs
- **Direct Computation Result**: `13.681`
- **Total Direct Computation Time (100000 iterations)**: 1,313,846 µs
- **Average Direct Computation Time per Iteration**: 13.1385 µs
- **Overhead per Evaluation**: 2.32029 µs

---

### Maximum and Minimum
- **Expression**: `max(a, b) + min(b, c)`
- **Compilation Time**: 131 µs
- **Total Evaluation Time (100000 iterations)**: 1,247,631 µs
- **Average Evaluation Time per Iteration**: 12.4763 µs
- **Direct Computation Result**: `4.75963`
- **Total Direct Computation Time (100000 iterations)**: 1,313,656 µs
- **Average Direct Computation Time per Iteration**: 13.1366 µs
- **Overhead per Evaluation**: -0.66025 µs

---

### Absolute Difference Division
- **Expression**: `abs(a - b) / (c + 1)`
- **Compilation Time**: 133 µs
- **Total Evaluation Time (100000 iterations)**: 1,254,223 µs
- **Average Evaluation Time per Iteration**: 12.5422 µs
- **Direct Computation Result**: `0.16169`
- **Total Direct Computation Time (100000 iterations)**: 1,314,553 µs
- **Average Direct Computation Time per Iteration**: 13.1455 µs
- **Overhead per Evaluation**: -0.6033 µs

---

### Two-Argument Arctangent
- **Expression**: `atan2(a, b)`
- **Compilation Time**: 111 µs
- **Total Evaluation Time (100000 iterations)**: 547,163 µs
- **Average Evaluation Time per Iteration**: 5.47163 µs
- **Direct Computation Result**: `0.857512`
- **Total Direct Computation Time (100000 iterations)**: 1,320,582 µs
- **Average Direct Computation Time per Iteration**: 13.2058 µs
- **Overhead per Evaluation**: -7.73419 µs

---

### Hypotenuse Calculation
- **Expression**: `hypot(a, b)`
- **Compilation Time**: 113 µs
- **Total Evaluation Time (100000 iterations)**: 543,134 µs
- **Average Evaluation Time per Iteration**: 5.43134 µs
- **Direct Computation Result**: `4.15435`
- **Total Direct Computation Time (100000 iterations)**: 1,318,710 µs
- **Average Direct Computation Time per Iteration**: 13.1871 µs
- **Overhead per Evaluation**: -7.75576 µs

---

### Factorial and Clamping
- **Expression**: `fac(5) + clamp(a, 0, 10)`
- **Compilation Time**: 133 µs
- **Total Evaluation Time (100000 iterations)**: 1,189,621 µs
- **Average Evaluation Time per Iteration**: 11.8962 µs
- **Direct Computation Result**: `123.142`
- **Total Direct Computation Time (100000 iterations)**: 1,321,934 µs
- **Average Direct Computation Time per Iteration**: 13.2193 µs
- **Overhead per Evaluation**: -1.32313 µs

---

### String Concatenation
- **Expression**: `"Hello, " + "World!"`
- **Compilation Time**: 108 µs
- **Total Evaluation Time (100000 iterations)**: 470,097 µs
- **Average Evaluation Time per Iteration**: 4.70097 µs
- **Direct Computation Result**: `Hello, World!`
- **Total Direct Computation Time (100000 iterations)**: 1,329,497 µs
- **Average Direct Computation Time per Iteration**: 13.295 µs
- **Overhead per Evaluation**: -8.594 µs

---

### String Replacement
- **Expression**: `replace("The quick brown fox", "fox", "dog")`
- **Compilation Time**: 121 µs
- **Total Evaluation Time (100000 iterations)**: 756,333 µs
- **Average Evaluation Time per Iteration**: 7.56333 µs
- **Direct Computation Result**: `The quick brown dog`
- **Total Direct Computation Time (100000 iterations)**: 1,322,221 µs
- **Average Direct Computation Time per Iteration**: 13.2222 µs
- **Overhead per Evaluation**: -5.65888 µs

---

### String Splitting
- **Expression**: `split("apple,orange,banana", ",")`
- **Compilation Time**: 114 µs
- **Total Evaluation Time (100000 iterations)**: 2,042,629 µs
- **Average Evaluation Time per Iteration**: 20.4263 µs
- **Direct Computation Result**: `apple, orange, banana`
- **Total Direct Computation Time (100000 iterations)**: 1,325,345 µs
- **Average Direct Computation Time per Iteration**: 13.2535 µs
- **Overhead per Evaluation**: 7.17284 µs

---

### Length of JSON Array
- **Expression**: `len(json_var.cars)`
- **Compilation Time**: 111 µs
- **Total Evaluation Time (100000 iterations)**: 7,269,558 µs
- **Average Evaluation Time per Iteration**: 72.6956 µs
- **Direct Computation Result**: `2`
- **Total Direct Computation Time (100000 iterations)**: 1,383,954 µs
- **Average Direct Computation Time per Iteration**: 13.8395 µs
- **Overhead per Evaluation**: 58.856 µs

---

### JSON Access
- **Expression**: `json_var.friends[2]`
- **Compilation Time**: 116 µs
- **Total Evaluation Time (100000 iterations)**: 4,920,995 µs
- **Average Evaluation Time per Iteration**: 49.2099 µs
- **Direct Computation Result**: `Tom`
- **Total Direct Computation Time (100000 iterations)**: 1,392,843 µs
- **Average Direct Computation Time per Iteration**: 13.9284 µs
- **Overhead per Evaluation**: 35.2815 µs

---

### Nested JSON Retrieval
- **Expression**: `json_var.cars[0].models[1]`
- **Compilation Time**: 136 µs
- **Total Evaluation Time (100000 iterations)**: 12,044,564 µs
- **Average Evaluation Time per Iteration**: 120.446 µs
- **Direct Computation Result**: `Focus`
- **Total Direct Computation Time (100000 iterations)**: 1,449,834 µs
- **Average Direct Computation Time per Iteration**: 14.4983 µs
- **Overhead per Evaluation**: 105.947 µs

## | The code used for the eval:
```cpp
#include "my_expr/my_expr.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>

int main() {
    // Define a collection of test expressions for mathematical, string, and JSON scenarios
    std::vector<std::pair<std::string, std::string>> expressions = {
        {"Simple Addition", "1 + 2"},
        {"Variable Arithmetic", "a * b + c"},
        {"Square Root Calculation", "sqrt(a^2 + b^2)"},
        {"Trigonometric Operations", "sin(a) * cos(b) + tan(c)"},
        {"Logarithmic and Exponential", "log(a) + exp(b) - pow(c, 2)"},
        {"Maximum and Minimum", "max(a, b) + min(b, c)"},
        {"Absolute Difference Division", "abs(a - b) / (c + 1)"},
        {"Two-Argument Arctangent", "atan2(a, b)"},
        {"Hypotenuse Calculation", "hypot(a, b)"},
        {"Factorial and Clamping", "fac(5) + clamp(a, 0, 10)"},
        {"String Concatenation", "\"Hello, \" + \"World!\""},
        {"String Replacement", "replace(\"The quick brown fox\", \"fox\", \"dog\")"},
        {"String Splitting", "split(\"apple,orange,banana\", \",\")"},
        {"Length of JSON Array", "len(json_var.cars)"},
        {"JSON Access", "json_var.friends[2]"},
        {"Nested JSON Retrieval", "json_var.cars[0].models[1]"}
    };

    // Define some commonly used variables
    std::unordered_map<string_t, token_data_t> variables = {
        {"a", 3.1415926535},  // π
        {"b", 2.7182818284},  // e
        {"c", 1.6180339887},  // Golden ratio φ
        {"json_var", R"(
            {
                "name": "John",
                "age": 30,
                "cars": [
                    {"name": "Ford", "models": ["Fiesta", "Focus"]},
                    {"name": "BMW", "models": ["320", "X3"]}
                ],
                "friends": ["Anna", "Peter", "Tom"]
            }
        )"_json}
    };

    // Number of iterations for timing evaluation
    const int iterations = 100000;

    // For each test case
    for (const auto& [name, exp] : expressions) {
        // Compilation phase
        auto start_compile = std::chrono::high_resolution_clock::now();
        expr e(exp);
        e.set_variables(variables);
        e.compile();
        auto end_compile = std::chrono::high_resolution_clock::now();
        auto compile_time = std::chrono::duration_cast<std::chrono::microseconds>(end_compile - start_compile).count();

        // Evaluation phase: Measure over multiple iterations
        auto start_eval = std::chrono::high_resolution_clock::now();
        parser_dtype result;
        for (int i = 0; i < iterations; ++i) {
            result = e.eval();
        }
        auto end_eval = std::chrono::high_resolution_clock::now();
        auto eval_time = std::chrono::duration_cast<std::chrono::microseconds>(end_eval - start_eval).count();
        double avg_eval_time = static_cast<double>(eval_time) / iterations;

        // Output results of compilation and evaluation times
        std::cout << "Test Case: " << name << std::endl;
        std::cout << "Expression: " << exp << std::endl;
        std::cout << "Compilation Time: " << compile_time << " us" << std::endl;
        std::cout << "Total Evaluation Time (" << iterations << " iterations): " << eval_time << " us" << std::endl;
        std::cout << "Average Evaluation Time per Iteration: " << avg_eval_time << " us" << std::endl;

        // Measure direct computation time for comparison
        auto start_direct = std::chrono::high_resolution_clock::now();
        double direct_result_num = 0.0;
        std::string direct_result_str = "";
        for (int i = 0; i < iterations; ++i) {
            double a = std::get<num_t>(variables["a"]);
            double b = std::get<num_t>(variables["b"]);
            double c = std::get<num_t>(variables["c"]);
            auto json_var = std::get<json_t>(variables["json_var"]);

            if (exp == "1 + 2") {
                direct_result_num = 1 + 2;
            } else if (exp == "a * b + c") {
                direct_result_num = a * b + c;
            } else if (exp == "sqrt(a^2 + b^2)") {
                direct_result_num = std::sqrt(a * a + b * b);
            } else if (exp == "sin(a) * cos(b) + tan(c)") {
                direct_result_num = std::sin(a) * std::cos(b) + std::tan(c);
            } else if (exp == "log(a) + exp(b) - pow(c, 2)") {
                direct_result_num = std::log(a) + std::exp(b) - std::pow(c, 2);
            } else if (exp == "max(a, b) + min(b, c)") {
                direct_result_num = std::max(a, b) + std::min(b, c);
            } else if (exp == "abs(a - b) / (c + 1)") {
                direct_result_num = std::abs(a - b) / (c + 1);
            } else if (exp == "atan2(a, b)") {
                direct_result_num = std::atan2(a, b);
            } else if (exp == "hypot(a, b)") {
                direct_result_num = std::hypot(a, b);
            } else if (exp == "fac(5) + clamp(a, 0, 10)") {
                direct_result_num = std::tgamma(5 + 1) + std::clamp(a, 0.0, 10.0);
            } else if (exp == "\"Hello, \" + \"World!\"") {
                direct_result_str = "Hello, " + std::string("World!");
            } else if (exp == "replace(\"The quick brown fox\", \"fox\", \"dog\")") {
                direct_result_str = "The quick brown dog";
            } else if (exp == "split(\"apple,orange,banana\", \",\")") {
                // Here we're just assuming we're running the equivalent logic
                direct_result_str = "apple orange banana";  // Simplified version
            } else if (exp == "len(json_var.cars)") {
                direct_result_num = json_var["cars"].size();
            } else if (exp == "json_var.friends[2]") {
                direct_result_str = json_var["friends"][2];
            } else if (exp == "json_var.cars[0].models[1]") {
                direct_result_str = json_var["cars"][0]["models"][1];
            }
        }
        auto end_direct = std::chrono::high_resolution_clock::now();
        auto direct_time = std::chrono::duration_cast<std::chrono::microseconds>(end_direct - start_direct).count();
        double avg_direct_time = static_cast<double>(direct_time) / iterations;

        // Output direct computation results and overhead analysis
        std::cout << "Direct Computation Result: ";
        if (direct_result_str.empty()) {
            std::cout << direct_result_num << std::endl;
        } else {
            std::cout << direct_result_str << std::endl;
        }
        std::cout << "Total Direct Computation Time (" << iterations << " iterations): " << direct_time << " us" << std::endl;
        std::cout << "Average Direct Computation Time per Iteration: " << avg_direct_time << " us" << std::endl;

        double overhead = avg_eval_time - avg_direct_time;
        std::cout << "Overhead per Evaluation: " << overhead << " us" << std::endl;

        std::cout << "----------------------------------------" << std::endl;
    }

    return 0;
}
```