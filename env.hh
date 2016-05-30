#pragma once
#include "variant.hh"
#include <map>
#include <list>
#include <deque>
#include <vector>

namespace lispy
{
	struct environment
	{
		environment() = default;
		environment(environment&&);
		std::map<string, variant_ptr> symbols;
		std::map<string, std::function<variant_ptr(const lispy::array&)>> user_fn;
		variant_ptr lookup(const string& name) { return symbols[name]; }
		void add_function(const std::string& n, std::function<variant_ptr(const lispy::array&)> f);
	};
	environment std_env();
	struct method
	{
		environment* env;
		std::vector<variant_ptr> expressions;
	};
}