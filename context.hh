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
			selector(const selector& o): obj(o.obj), ptr(o.ptr) { }
			selector(selector&& o): obj(o.obj), ptr(o.ptr) { }
			
            template<class ...Args> selector operator()(Args... args)
            {
                auto& fn = (*ptr)->value_function;
				return fn({make_variant(args)...});
            }
            template<class T> selector operator=(const T& v)
			{
				*ptr = make_variant(v);
				return *this;
			}
            template<class T> T as() const;
        private:
			friend class context;
			selector(variant_ptr& c) : ptr(&c) { }
			selector(variant_ptr&& m) : obj(m), ptr(&obj) { }
			// We interact exclusively through ptr
            variant_ptr* ptr;
			variant_ptr obj;
        };
        class context
        {
        public:
			context();
			void load_file(const string&);
			selector operator()(const string& line);
			selector operator[](const string& name)
			{
				return env.symbols[name];
			}
        private:
            environment env;
            lexer lex;
        };
		template<> inline long long selector::as<long long>() const
		{
			if (obj->variant_kind != variant::kind_int)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_int);
			return obj->value_int;
		}
		template<> inline double selector::as<double>() const
		{
			if (obj->variant_kind != variant::kind_double)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_double);
			return obj->value_double;
		}
		template<> inline string selector::as<string>() const
		{
			if (obj->variant_kind != variant::kind_string)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_string);
			return obj->value_string;
		}
		template<> inline bool selector::as<bool>() const
		{
			if (obj->variant_kind != variant::kind_bool)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_bool);
			return obj->value_bool;
		}
		template<> inline drift::function selector::as<function>() const
		{
			if (obj->variant_kind != variant::kind_function)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_function);
			return obj->value_function;
		}
		template<> inline list selector::as<list>() const
		{
			if (obj->variant_kind != variant::kind_list)
				throw variant::incorrect_treatment(obj->variant_kind, variant::kind_list);
			return obj->value_list;
		}
    }
}