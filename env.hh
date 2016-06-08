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
		environment(environment* parent) : parent(parent) { }
		environment(environment&&);
		std::map<string, variant_ptr> symbols;
		std::map<string, std::function<variant_ptr(const lispy::list&)>> user_fn;
		environment* parent = nullptr;
		variant_ptr lookup(const string& name) {
			auto sym = symbols.find(name);
			if(sym != symbols.end())
				return sym->second;
			auto fn = user_fn.find(name);
			if(fn != user_fn.end())
				return make_variant(fn->second);
			
			if(parent != nullptr)
				return parent->lookup(name);
			
			throw(name + " evironment could not find identifier.");
		}
		void add_function(const std::string& n, std::function<variant_ptr(const lispy::list&)> f);
	};
	environment std_env();
	struct method
	{
		environment* env;
		std::vector<variant_ptr> expressions;
	};
}