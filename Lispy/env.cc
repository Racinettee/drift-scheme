#include "env.hh"
#include "lib.hh"

namespace lispy
{
	environment::environment(environment&& env) :
		symbols(std::move(env.symbols)), user_fn(std::move(env.user_fn))
	{
	}
	void environment::add_function(const std::string& n, std::function<variant_ptr(const lispy::array&)> f)
	{
		user_fn[n] = f;
	}
	environment std_env()
	{
		environment env;
		env.add_function("print", lib::print);
		env.add_function("println", lib::println);
		env.symbols["pi"] = std::make_shared<variant>(3.141592653589793);
		return env;
	}
}