#include "variant.hh"
#include <cstdio>
using namespace std;
namespace lispy
{
	namespace lib
	{
		variant_ptr print(const lispy::list& values)
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
				case variant::kind::kind_null:
					printf("<null>");
				}
			}
			return make_shared<variant>(variant::null_kind());
		}
		variant_ptr println(const lispy::list& values)
		{
			auto r = print(values);
			std::putc('\n', stdout);
			return r;
		}
	}
}