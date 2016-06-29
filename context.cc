#include <string>
#include "context.hh"
using namespace std;
namespace drift
{
	namespace scheme
	{
		context::context(): env(std_env())
		{
			lex.add_keyword("define"s);
			lex.add_keyword("set"s);
			lex.add_keyword("lambda"s);
			lex.add_keyword("begin"s);
			lex.add_keyword("list"s);
			lex.add_keyword("map"s);
		}
		shared_ptr<method> parse(environment& e, const token_array& tokens);
		void context::load_file(const string& name)
		{
			token_array tokens = lex.lex_file(name);
			
			auto fn = parse(env, tokens);
			
			auto& expressions = fn->expressions;
			
			for (auto& expr : expressions)
				expr->value_function({});
		}
		selector context::operator()(const string& line)
		{
			token_array tokens = lex.lex_string(line);

			auto method = parse(env, tokens);

			try {
				return move(method->expressions.at(0));
				//auto& expr = method->expressions.at(0);

				//return expr->variant_kind == variant::kind_function ? expr->value_function({}) : expr;
			}
			catch(...)
			{
				return selector();
			}
		}
	}
}