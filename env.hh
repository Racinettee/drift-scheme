#pragma once
#include "variant.hh"
#include <unordered_map>
#include <list>
#include <deque>
#include <vector>

namespace drift
{
	namespace schemy
	{
		struct environment
		{
			environment() = default;
			
			environment(environment* parent) : parent(parent) { }
			environment(environment&&);
			std::unordered_map<string, variant_ptr> symbols;
			std::unordered_map<string, variant_ptr> user_fn;
			environment* parent = nullptr;
			variant_ptr& lookup(const string& name) {
				try {
					return symbols.at(name);
				} catch(std::out_of_range&) { }
				try {
					return user_fn.at(name);
				} catch(std::out_of_range&) { }
				if(parent != nullptr)
					return parent->lookup(name);
				throw std::out_of_range(name + " evironment could not find identifier.");
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