#include <numeric>
#include <stdexcept>
#include <cstdio>
#include "variant.hh"
#include "tokens.hh"
#include "env.hh"
using namespace std;
namespace drift
{
	namespace schemy
	{
		variant_ptr identifier_lookup(const list& args, shared_ptr<method> fn, const string& identifier);
		variant_ptr define(const list& values, environment* env);
		variant_ptr set(const list& values, shared_ptr<method> fn);
		variant_ptr lambda(const list& args, list values, shared_ptr<method> fn, list arguments);
		variant_ptr if_fn(const list& values);
		namespace
		{
			// Thrown if a token was expected but something else was found
			struct unexpected_token : public invalid_argument
			{
				unexpected_token(const string& msg) : invalid_argument("Unexpected token: " + msg) { }
			};
			// Thrown in the case that a parsed expression is found to be over but we were parsing elements
			struct expr_over : exception { };
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
			// Meant only for optional syntax
			inline static void optional(token_type tok_ty, const token_array& tok_array, size_t& ptr)
			{
				if (tok_ty == tok_array.at(ptr).first)
					++ptr;
			}
			inline static token_type peek_tok(const token_array& t, size_t pos)
			{
				return t.at(pos + 1).first;
			}
	#define expect(tok, exparry, ptr, msg) expect_fn(tok, exparry, ptr, string(msg)+" "+string(__FILE__)+":"+std::to_string(__LINE__))

			static variant_ptr parse_expr(shared_ptr<method>, const token_array&, size_t& pos);
			static variant_ptr parse_element(shared_ptr<method> fn, const token_array& tok_array, size_t& pos, size_t& num_args, bool lookup_identifiers = true)
			{
				while (tok_array.at(pos).first == Quote) pos++;
				auto& tok = tok_array.at(pos);

				switch (tok.first)
				{
				case Int: case Num:
					return tok.second;
				case Str:
					if (peek_tok(tok_array, pos) == Quote) //tok_array.at(pos + 1).first == Quote)
						++pos;
					return tok.second;
				case Identifier:
				{
					string identifier = tok.second->value_string;
					// Maybe arguments can be passed down to this function to solve the issue of de-referencing predicates
					using std::placeholders::_1;
					return (lookup_identifiers ? make_variant(bind(identifier_lookup, _1, fn, identifier)) : tok.second);
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
			static variant_ptr parse_condition(const token& first_tok, shared_ptr<method> fn, const token_array& tok_array, size_t& pos)
			{
				size_t num_args = 0;

				list values;

				for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
					values.emplace_back(parse_element(fn, tok_array, pos, num_args));
				
				if (num_args <= 1)
					throw invalid_argument("Error: equality or inequality expression requires one or more arguments");

				auto& oper = first_tok.second->value_string;

				if (num_args > 2 && !(oper == "==" || oper == "!="))
					printf("Warn: comparisons that are not == or != should only have 2 arguments\n");

				if(oper == "==")
					return num_args > 2 ? 
						make_variant([values](const list&)->variant_ptr {
							for(size_t i = 0; i < values.size()-1; i++)
								if(values[i] != values[i + 1])
									return make_variant(false);
							return make_variant(true);
						}) :
						make_variant([values](const list&)->variant_ptr {
							return make_variant(values[0] == values[1]);
						});
				else if(oper == "!=")
					return num_args > 2 ?
						make_variant([values](const list&)->variant_ptr {
							for(size_t i = 0; i < values.size()-1; i++)
								if(values[i] == values[i + 1])
									return make_variant(false);
							return make_variant(true);
						}) :
						make_variant([values](const list&)->variant_ptr {
							return make_variant(values[0] != values[1]);
						});
				return null();
			}
			static variant_ptr parse_arith_expr(const token& first_tok, shared_ptr<method> fn, const token_array& tok_array, size_t& pos)
			{
				size_t num_args = 0;

				list values;

				try
				{
					for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
						values.emplace_back(parse_element(fn, tok_array, pos, num_args));
				}
				catch (out_of_range&) {	}
				catch (expr_over&) { }

				if(num_args < 2)
					throw invalid_argument("Error: at least 2 arguments are expected to an arithmetic function, but got: "s + to_string(num_args));

				switch (first_tok.second->value_string[0])
				{
				case '+':
					return make_variant([values](const list&)->variant_ptr{
						return accumulate(values.begin(), values.end(),
							(values.front()->variant_kind == variant::kind_string ? make_variant(string("")) : make_variant(0LL)));
					});
				case '*':
					return make_variant([values](const list&) {
						auto initial = values.at(0) * values.at(1);
						for (size_t i = 2; i < values.size(); i++)
							initial = initial * values[i];
						return initial;
					});
				case '-':
					return make_variant([values](const list&) {
						auto initial = values[0] - values[1];
						for (size_t i = 2; i < values.size(); i++)
							initial = initial - values[i];
						return initial;
					});
				}
				return make_variant(variant::null_kind());
			}
			inline static list parse_args(const token_array& tok_array, size_t& pos)
			{
				expect(token_type::LParen, tok_array, pos, " while seeking arguments for a lambda");
				list args;
				for(; tok_array.at(pos).first != token_type::RParen; pos++)
					args.emplace_back(tok_array[pos].second);
				expect(token_type::RParen, tok_array, pos, " while seeking to end the arguments for a lambda");
				return args;
			}

			static variant_ptr parse_kw_expr(const token& first_tok, shared_ptr<method> fn, const token_array& tok_array, size_t& pos)
			{
				size_t num_args = 0;
				const string& command = first_tok.second->value_string;
				if (command == "set")
					optional(Not, tok_array, pos);
				bool will_lookup = !(command == "define" ||
									command == "set");// ||
									//command == "lambda");

				list arguments;
				if(command == "lambda")
					arguments = parse_args(tok_array, pos);

				list values;
				try
				{
					for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
						values.emplace_back(parse_element(fn, tok_array, pos, num_args, will_lookup));
				}
				catch (out_of_range&) { --pos;  }
				catch (expr_over&) { }

				if (command == "if")
				{
					if (num_args != 3)
						throw invalid_argument("Expected 3 arguments, but got "s + to_string(num_args));
					return make_variant(bind(if_fn, values));
				}
				else if(command == "define")
				{
					if (num_args != 2)
						throw invalid_argument("define expects 2 arguments, but got "s + to_string(num_args));
					
					auto env = fn->env;
					return make_variant(bind(define, values, env));
				}
				else if (command == "set")
				{
					if (num_args != 2)
						throw invalid_argument("set expects 2 arguments, but got "s + to_string(num_args));

					return make_variant(bind(set, values, fn));
				}
				else if (command == "lambda")
				{
					if (num_args < 1)
						throw invalid_argument("Lambda requires at least 1 expression");

					using std::placeholders::_1;
					return make_variant(bind(lambda, _1, values, fn, arguments));
				}
				else if (command == "begin")
				{
					return make_variant([values](const list&) -> variant_ptr {
						for(size_t i = 0; i < values.size()-1; i++)
							if(values[i]->variant_kind == variant::kind_function)
								values[i]->value_function({});
						auto& value = values.back();
						return (value->variant_kind == variant::kind_function ? value->value_function({}) : value);
					});
				}
				else if (command == "list")
				{
					return make_variant([values](const list&) -> variant_ptr {
						list result;
						for(auto& value : values)
							result.push_back(value->variant_kind == variant::kind_function ? value->value_function({}) : value);
						return make_variant(result);
					});
				}
				else if (command == "map")
				{
					if(values.size() != 2)
						throw invalid_argument("Invalid number of arguments: map function takes 2: predicate & list");
					return make_variant([values](const list&) -> variant_ptr {
						list result;
						auto& fn = values[0]->value_function;
						auto ls = values[1]->value_function({})->value_list;

						for(auto& value : ls)
							result.emplace_back(fn({value}));
						
						return make_variant(result);
					});
				}
				return make_variant(variant::null_kind());
			}
			static variant_ptr parse_fn_call_expr(const token& first_tok, shared_ptr<method> fn, const token_array& tok_array, size_t& pos)
			{
				size_t num_args = 0;
				list values;
				try
				{
					for (; tok_array.at(pos).first != token_type::RParen; pos++, ++num_args)
						values.emplace_back(parse_element(fn, tok_array, pos, num_args));
				}
				catch (out_of_range&) { --pos; }
				catch (expr_over&) { }

				//auto env = fn->env;
				auto name = first_tok.second->value_string;
				return make_variant([fn, name, values](const list& args)->variant_ptr{
					auto* env = fn->env;
					
					auto result = env->lookup(name);
					
					return result->variant_kind == variant::kind_function ? result->value_function(values) : result;
				});
			}
			static variant_ptr parse_expr(shared_ptr<method> fn, const token_array& tok_array, size_t& pos)
			{
				if (tok_array.at(pos).first != token_type::LParen)
				{
					size_t placehold = 0;
					auto result = parse_element(fn, tok_array, pos, placehold, false); ++pos;
					return result;
				}
				else
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

					case Conditional:
					case LThan:	case GThan:
						result = parse_condition(first_tok, fn, tok_array, pos);
						break;

					case Keyword:
						result = parse_kw_expr(first_tok, fn, tok_array, pos);
						break;

					case Identifier:
						result = parse_fn_call_expr(first_tok, fn, tok_array, pos);
						break;
					case RParen:
						// We got an empty set of paren's
						printf("Warn: empty expression '()' - will be omitted\n");
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
		shared_ptr<method> parse(environment& e, const token_array& tokens)
		{
			size_t pos = 0;
			shared_ptr<method> fn = make_shared<method>(); fn->env = &e;
			while (pos < tokens.size())
			{
				auto expr = parse_expr(fn, tokens, pos);
				if(expr)
					fn->expressions.push_back(expr);
			}
			return move(fn);
		}
	}
}