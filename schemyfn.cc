#include "variant.hh"
#include "env.hh"
using namespace std;
namespace drift
{
	namespace schemy
	{
		variant_ptr identifier_lookup(const list& args, shared_ptr<method> fn, const string& identifier)
		{
			auto found = fn->env->lookup(identifier);
			return found->variant_kind == variant::kind_function ? found->value_function(args) : found;
		}
		variant_ptr define(const list& values, environment* env)
		{
			// values[0] should be an identifier, values[1] will be its value
			if (values[0]->variant_kind != variant::kind_string)
				throw invalid_argument("define expects an identifier as its first arg but got: "s +
					variant::kind_str(values[0]->variant_kind));

			auto& symbols = env->symbols;
			// Assert that the symbol we're defining is not already defined
			if (symbols.find(values[0]->value_string) != symbols.end())
				throw invalid_argument(values[0]->value_string + " was already defined");

			auto iter = symbols.emplace(values[0]->value_string, values[1]);

			return iter.first->second;
		}
		variant_ptr set(const list& values, shared_ptr<method> fn)
		{
			auto env = fn->env;
			if (values[0]->variant_kind != variant::kind_string)
				throw invalid_argument("set expects an identifier as its first arg but got: "s + variant::kind_str(values[0]->variant_kind));

			auto& symbols = env->symbols;
			// Assert that the symbol we're setting has been defined
			if (symbols.find(values[0]->value_string) == symbols.end())
				throw invalid_argument("set being used on identifier that hasn't been defined: "s + values[0]->value_string);

			return symbols.at(values[0]->value_string) = (values[1]->variant_kind == variant::kind_function ?
				values[1]->value_function({}) : values[1]);
		}
		variant_ptr lambda(const list& args, list values, shared_ptr<method> fn, list arguments)
		{
			environment env(fn->env);
			environment* original = fn->env;
			const size_t argsc = args.size();
			const size_t argumentsc = arguments.size();

			if (argsc != argumentsc)
				printf("Warning: Number of arguments passed to lambda do "
					"not match what it was specified with: %lu vs %lu\n", argsc, argumentsc);

			for (size_t i = 0UL; i < argsc; i++)
				// Make sure to resolve any expressions from the upper level scope before setting the new environment to the method object
				env.symbols[arguments[i]->value_string] = (args[i]->variant_kind == variant::kind_function ? args[i]->value_function({}) : args[i]);

			fn->env = &env;

			for (size_t i = 0; i < values.size() - 1; i++)
				if (values[i]->variant_kind == variant::kind_function)
					values[i]->value_function({});
			auto& value = values.back();

			auto result = (value->variant_kind == variant::kind_function ? value->value_function({}) : value);

			fn->env = original;

			return result;
		}
		variant_ptr if_fn(const list& values)
		{
			auto& cond = *values[0];

			auto& value = ((cond.variant_kind == variant::kind::kind_function ?
				cond.value_function({})->value_bool : cond.value_bool) ?
				values[1] : values[2]);

			return value->variant_kind == variant::kind_function ? value->value_function({}) : value;
		}
	}
}
