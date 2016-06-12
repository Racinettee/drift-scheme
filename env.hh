#pragma once
#include "variant.hh"
#include <map>
#include <list>
#include <deque>
#include <vector>

namespace drift
{
	namespace scheme
	{
		struct environment
		{
			environment() = default;
			
			environment(environment* parent) : parent(parent) { }
			environment(environment&&);
			std::map<string, variant_ptr> symbols;
			std::map<string, variant_ptr> user_fn;
			environment* parent = nullptr;
			variant_ptr& lookup(const string& name) {
				try {
					variant_ptr& sym = symbols.at(name);
					return sym;
				} catch(std::out_of_range&) { }
				try {
					return user_fn.at(name);
				} catch(std::out_of_range&) { }
				//auto sym = symbols.find(name);
				//if(sym != symbols.end())
				//	return sym->second;
				//auto fn = user_fn.find(name);
				//if(fn != user_fn.end())
				//	return fn->second; //return user_fn.at(name); //(*fn).second;
				if(parent != nullptr)
					return parent->lookup(name);
				throw(name + " evironment could not find identifier.");
			}
			void add_function(const string& n, function f);
		};
		environment std_env();
		struct method
		{
			method() = default;
			method(method&&);
			environment* env;
			std::vector<variant_ptr> expressions;
			
		};
	}
}