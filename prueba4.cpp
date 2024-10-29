
#include "my_expr/my_expr.h"


auto fiumba = [](const token_data_t *args) -> token_data_t
{
	return token_data_t(string_t("fiumba")) ;
}; 


int main()
{
	// try to tokenize a really complex expression with all cases

	std::string exp = R"(pow(2, cos(50)) + 8 *4 + "hola" + fiumba(1)*as.ES )";
	
	expr e(exp);
	std::unordered_map<string_t, token_data_t> vars = {{"ey", 5}};
	e.set_variables({{"ey", 5}});

	e.set_functions({
		{"fiumba", {fiumba, 1}},
	});

	e.set_unknown_var_resolver(
		[](const std::string_view &symbol) -> token_data_t
		{
			return 3;
		},
		true);

	e.compile();
	auto result = e.eval();
	std::cout << "Result: " << result.toString() << std::endl;
	return 0;
}