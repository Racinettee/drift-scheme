#include "env.hh"
#include "lib.hh"
using std::string;
using std::move;
namespace lispy
{
	environment::environment(environment&& env) :
		symbols(move(env.symbols)), user_fn(move(env.user_fn))
	{
	}
	void environment::add_function(const string& n, std::function<variant_ptr(const lispy::list&)> f)
	{
		user_fn[n] = f;
	}
	environment std_env()
	{
		environment env;
		env.add_function("print", lib::print);
		env.add_function("println", lib::println);
		env.symbols["pi"] = make_variant(3.141592653589793);
		return env;
	}
}