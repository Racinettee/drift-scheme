#include "env.hh"
#include "lib.hh"
using std::string;
using std::move;
namespace drift
{
	namespace schemy
	{
		environment::environment(environment&& env) :
			symbols(move(env.symbols)), user_fn(move(env.user_fn))
		{
		}
		method::method(method&& o): env(o.env), expressions(move(o.expressions))
		{
		}

		void environment::add_function(const string& n, function f)
		{
			user_fn[n] = make_variant(f);
		}
		environment std_env()
		{
			environment env;
			env.add_function("print", lib::print);
			env.add_function("println", lib::println);
			env.add_function("readln", lib::readln);
			env.add_function("foreach", lib::foreach);
			env.symbols["pi"] = make_variant(3.141592653589793);
			return env;
		}
	}
}