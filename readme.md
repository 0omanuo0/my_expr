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