#include "variant.hh"
#include <cstdio>
#include <stdexcept>
#include <iostream>
using namespace std;
namespace drift
{
	namespace scheme
	{
		namespace lib
		{
			variant_ptr print(const list& values)
			{
				for (auto v : values)
				{
					switch (v->variant_kind)
					{
					case variant::kind::kind_function:
						print({ v->value_function({}) });
						break;
					case variant::kind::kind_int:
						printf("%lli", v->value_int);
						break;
					case variant::kind::kind_string:
						printf("%s", v->value_string.c_str());
						break;
					case variant::kind::kind_double:
						printf("%f", v->value_double);
						break;
					case variant::kind::kind_list:
					{
						auto& vals = v->value_list;
						putc('(', stdout);
						for(size_t i = 0; i < vals.size()-1; i++)
						{
							print({vals[i]});
							printf(", ");
						}
						print({vals[vals.size()-1]});
						putc(')', stdout);
						break;
					}
					case variant::kind::kind_null:
						printf("<null>");
					}
				}
				return make_shared<variant>(variant::null_kind());
			}
			variant_ptr println(const list& values)
			{
				auto r = print(values);
				putc('\n', stdout);
				return r;
			}
			variant_ptr readln(const list&)
			{
				string result = "";
				getline(cin, result);
				return make_variant(result);
			}
			variant_ptr foreach(const list& values)
			{
				if(values.size() != 2UL)
					throw invalid_argument(string("2 arguments are expected to foreach, but ") + to_string(values.size()) + " were provided");
				
				auto val_fn_list = values[0]->value_function({});
				if(val_fn_list->variant_kind != variant::kind_list)
					throw invalid_argument("Expected list as first argument to foreach");
				
				auto& val_list = val_fn_list->value_list;
				
				// The current function which is supposed to be the predicate will actually a lookup handler
				// So the thing to do here is to look up our predicate by simply calling the function and retrieving its result
				auto val_pred = values[1]->value_function;//({})->value_function;
				
				for(size_t i = 0; i < val_list.size()-1; i++)
				{
					val_pred({val_list[i]});
				}
				return val_pred({val_list[val_list.size()-1]});
			}
		}
	}
}