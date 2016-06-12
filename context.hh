#pragma once
#include "env.hh"
#include "lex.hh"
#include "variant.hh"
namespace drift
{
    namespace scheme
    {
        class selector
        {
        public:
			selector() = default;
			selector(variant_ptr v): obj(v) { }
			selector(const selector& o): obj(o.obj) { }
			selector(selector&& o): obj(o.obj) { }
			
            template<class ...Args> selector operator()(Args... args)
            {
                auto& fn = obj->value_function;
				return fn(list({make_variant(args)...}));
            }
            template<class T> selector operator=(const T& v)
			{
				obj = make_variant(v);
				return *this;
			}
            template<class T> T as() const;
        private:
            variant_ptr obj;
        };
        class context
        {
        public:
			context();
			void load_file(const string&);
			selector operator[](const string& name)
			{
				return env.symbols[name];
			}

        private:
			std::vector<std::unique_ptr<method>> methods;
            environment env;
            lexer lex;
        };
		template<> inline long long selector::as<long long>() const
		{
			return obj->value_int;
		}
		template<> inline double selector::as<double>() const
		{
			return obj->value_double;
		}
		template<> inline string selector::as<string>() const
		{
			return obj->value_string;
		}
		template<> inline list selector::as<list>() const
		{
			return obj->value_list;
		}
    }
}