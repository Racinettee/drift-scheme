#include <numeric>
#include <stdexcept>
#include <cstdio>
#include "variant.hh"
#include "tokens.hh"
#include "env.hh"
using namespace std;
namespace lispy
{
	namespace
	{
		struct unexpected_token : public invalid_argument
		{
			unexpected_token(const string& msg) : invalid_argument("Unexpected token: " + msg) { }
		};
		// Asserts that the token expected is present and increments the pointer
		static void expect_fn(token_type tok_ty, const token_array& tok_array, size_t& ptr, const std::string& message)
		{
			try
			{
				if (tok_ty != tok_array.at(ptr++).first)
					throw unexpected_token("expected: " + token_string(tok_ty) + " but got: " + token_string(tok_array[ptr].first) + message);
			}
			catch (out_of_range& e)
			{
				throw unexpected_token("expected: " + token_string(tok_ty) + " but caught out of bounds exception" + e.what());
			}
		}
		inline static void optional(token_type tok_ty, const token_array& tok_array, size_t& ptr)
		{
			if (tok_ty == tok_array.at(ptr).first)
				++ptr;
		}
#define expect(tok, exparry, ptr, msg) expect_fn(tok, exparry, ptr, string(msg)+" "+string(__FILE__)+":"+std::to_string(__LINE__))

		static variant_ptr parse_expr(method* fn, const token_array& tok_array, size_t& pos);
		struct expr_over : exception { };
		static variant_ptr parse_element(method* fn, const token_array& tok_array, size_t& pos, size_t& num_args, bool lookup_identifiers = true)
		{
			while (tok_array.at(pos).first == Quote) pos++;
			auto& tok = tok_array.at(pos);

			switch (tok.first)
			{
			case Int: case Str:	case Num:
				return tok.second;
			case Identifier:
			{
				string identifier = tok.second->value_string;
				environment* env = fn->env;
				return (lookup_identifiers ? make_shared<variant>([env, identifier]()->variant_ptr {
					return env->lookup(identifier);
				}) : tok.second);
			}
			case LParen:
			{
				auto res = parse_expr(fn, tok_array, pos); pos--; // We get incremented to far after parsing another expr
				return res;
			}
			case RParen:
				throw expr_over();
			default:
				return parse_element(fn, tok_array, ++pos, num_args);
			}
		}
		static void parse_condition(const token& first_tok, method* fn, const token_array& tok_array, size_t& pos)
		{
			size_t num_args = 0;

			for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
				parse_element(fn, tok_array, pos, num_args);

			if (num_args > 2)
				printf("Warning, comparison should only have 2 arguments");

			switch (first_tok.first)
			{
			case LThan:
			case GThan:
				//fn->instructions.push_back(cmp_ineq);
				break;
			}
		}
		static variant_ptr parse_arith_expr(const token& first_tok, method* fn, const token_array& tok_array, size_t& pos)
		{
			size_t num_args = 0;

			lispy::array values;

			try
			{
				for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
					values.emplace_back(parse_element(fn, tok_array, pos, num_args));
			}
			catch (out_of_range&) {	}
			catch (expr_over&) { }

			switch (first_tok.second->value_char)
			{
			case '+':
				return std::make_shared<variant>([values]() {
					return std::accumulate(values.begin(), values.end(),
						(values.front()->variant_kind == variant::kind_string ? make_shared<variant>(std::string("")) : make_shared<variant>(0LL)));
				});
			case '*':
				return std::make_shared<variant>([values]() {
					auto initial = values.at(0) * values.at(1);
					for (size_t i = 2; i < values.size(); i++)
						initial = initial * values[i];
					return initial;
				});
			}
			return make_shared<variant>(variant::null_kind());
		}
		static variant_ptr parse_kw_expr(const token& first_tok, method* fn, const token_array& tok_array, size_t& pos)
		{
			size_t num_args = 0;
			if (first_tok.second->value_string == "set")
				optional(Not, tok_array, pos);
			bool will_lookup = !(first_tok.second->value_string == "define" ||
								 first_tok.second->value_string == "set" ||
								 first_tok.second->value_string == "lambda");

			lispy::array values;
			try
			{
				for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
					values.emplace_back(parse_element(fn, tok_array, pos, num_args, will_lookup));
			}
			catch (out_of_range&) { --pos;  }
			catch (expr_over&) { }

			if (first_tok.second->value_string == "if")
			{
				if (num_args != 3)
					throw invalid_argument(string("Expected 3 arguments, but got ") + to_string(num_args));
				return make_shared<variant>([values]() -> shared_ptr<variant> {
					auto& cond = *values[0];
					switch(cond.variant_kind)
					{
					case variant::kind::kind_function:
					{
						auto& value = (cond.value_function()->value_bool ? values[1] : values[2]);
						return value->variant_kind == variant::kind_function ? value->value_function() : value;
					}
					case variant::kind::kind_int: case variant::kind::kind_bool:
						auto& value = (cond.value_bool ? values[1] : values[2]);
						return value->variant_kind == variant::kind_function ? value->value_function() : value;
					}
					// Only the explicit value of 0 or false can be used to evaluate false
					return make_shared<variant>(true);
				});
			}
			else if(first_tok.second->value_string == "define")
			{
				if (num_args != 2)
					throw invalid_argument(string("define expects 2 arguments, but got ") + to_string(num_args));
				
				auto env = fn->env;
				return make_shared<variant>([env, values]() -> variant_ptr {
					// values[0] should be an identifier, values[1] will be its value
					if (values[0]->variant_kind != variant::kind_string)
						throw invalid_argument(string("define expects an identifier as its first arg but got: ") +
							variant::kind_str(values[0]->variant_kind));

					auto& symbols = env->symbols;
					// Assert that the symbol we're defining is not already defined
					if (symbols.find(values[0]->value_string) != symbols.end())
						throw invalid_argument(values[0]->value_string + " was already defined");

					auto iter = symbols.emplace(values[0]->value_string, values[1]);

					return iter.first->second;
				});
			}
			else if (first_tok.second->value_string == "set")
			{
				if (num_args != 2)
					throw invalid_argument(string("set expects 2 arguments, but got ") + to_string(num_args));

				auto env = fn->env;
				return make_shared<variant>([env, values]() -> variant_ptr {
					if (values[0]->variant_kind != variant::kind_string)
						throw invalid_argument("set expects an identifier as its first arg but got: " + variant::kind_str(values[0]->variant_kind));

					auto& symbols = env->symbols;
					// Assert that the symbol we're setting has been defined
					if (symbols.find(values[0]->value_string) == symbols.end())
						throw invalid_argument("set being used on identifier that hasn't been defined: " + values[0]->value_string);

					return symbols[values[0]->value_string] = values[1];
				});
			}
			else if (first_tok.second->value_string == "lambda")
			{
				if (num_args < 2)
					throw invalid_argument(string("lambda expects at least 2 argument but got: ") + to_string(num_args));
				
				auto new_env = make_shared<environment>(fn->env);
				return make_shared<variant>([new_env, values]() -> variant_ptr {
					//return values[1]->value_function();
					for(size_t i = 1; i < values.size()-1; i++)
						if(values[i]->variant_kind == variant::kind_function)
							values[i]->value_function();
					auto& value = values.back();
					return (value->variant_kind == variant::kind_function ? value->value_function() : value);
				});
			}
			else if (first_tok.second->value_string == "begin")
			{
				return make_shared<variant>([values]() -> variant_ptr {
					for(size_t i = 0; i < values.size()-1; i++)
						if(values[i]->variant_kind == variant::kind_function)
							values[i]->value_function();
					auto& value = values.back();
					return (value->variant_kind == variant::kind_function ? value->value_function() : value);
				});
			}
			return make_shared<variant>(variant::null_kind());
		}
		static variant_ptr parse_fn_call_expr(const token& first_tok, method* fn, const token_array& tok_array, size_t& pos)
		{
			size_t num_args = 0;
			lispy::array values;
			try
			{
				for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
					values.emplace_back(parse_element(fn, tok_array, pos, num_args));
			}
			catch (out_of_range&) { --pos; }
			catch (expr_over&) { }

			auto env = fn->env;
			auto name = first_tok.second->value_string;
			return make_shared<variant>([env, name, values]()->variant_ptr{
				auto user_sym = env->user_fn.find(name);
				if (user_sym != env->user_fn.end())
					return user_sym->second(values);
				
				auto scm_sym = env->symbols.find(name);
				if (scm_sym != env->symbols.end())
					return scm_sym->second->value_function();
				
				throw invalid_argument(string("Tried to call non existent function ") + name);
			});
		}
		static variant_ptr parse_expr(method* fn, const token_array& tok_array, size_t& pos)
		{
			expect(token_type::LParen, tok_array, pos, " when staring to parse expression");

			variant_ptr result;

			try
			{
				const token& first_tok = tok_array.at(pos++);
				switch (first_tok.first)
				{
				case Arith:
					result = parse_arith_expr(first_tok, fn, tok_array, pos);
					break;

				case Keyword:
					result = parse_kw_expr(first_tok, fn, tok_array, pos);
					break;

				case Identifier:
					result = parse_fn_call_expr(first_tok, fn, tok_array, pos);
				case LThan:
					break;
				case RParen:
					// We got an empty set of paren's
					pos--;
					break;
				}
			}
			catch (out_of_range& e)
			{
				std::fprintf(stderr, "Caught: %s %s\n", e.what(), " when parsing expression");
			}

			expect(token_type::RParen, tok_array, pos, " when trying to finish parsing an expression");

			return result;
		}
	}
	method parse(environment& e, const token_array& tokens)
	{
		size_t pos = 0;
		method fn; fn.env = &e;
		while (pos < tokens.size())
			fn.expressions.push_back(parse_expr(&fn, tokens, pos));
		return fn;
	}
}