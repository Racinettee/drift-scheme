#include <string>
#include "context.hh"
using namespace std;
namespace drift
{
	namespace scheme
	{
		context::context(): main_method(make_unique<method>()), env(std_env())
		{
			main_method->env = &env;
			lex.add_keyword("define"s);
			lex.add_keyword("set"s);
			lex.add_keyword("lambda"s);
			lex.add_keyword("begin"s);
			lex.add_keyword("list"s);
		}
		void parse(method*, const token_array&);
		unique_ptr<method> parse(environment&, const token_array&);
		void context::load_file(const string& name)
		{
			token_array tokens = lex.lex_file(name);
			
			auto method = parse(env, tokens);
			
			auto& expressions = method->expressions;
			
			for (auto& expr : expressions)
				expr->value_function({});
		}
		selector context::operator()(const string& line)
		{
			token_array tokens = lex.lex_string(line);

			unique_ptr<method> fn = make_unique<method>();
			fn->env = &env;
			parse(fn.get(), tokens);

			auto& expr = fn->expressions;
			
			for(size_t i = 0; expr.size()-1; i++)
				if(expr[i]->variant_kind == variant::kind_function)
					expr[i]->value_function({});

			auto& last_expr = expr[expr.size()];
			return last_expr->variant_kind == variant::kind_function ? last_expr->value_function({}) : last_expr;
			//last_result = expr->variant_kind == variant::kind_function ? expr->value_function({}) : expr;
			//return last_result;
		}
	}
}