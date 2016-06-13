#include "variant.hh"

using namespace std;
namespace drift
{
	namespace
	{
		const char* kind_str_values[] =
		{
			"char",
			"bool",
			"int",
			"float",
			"string"
			"array",
			"table",
			"function",
			"null"
		};
	}
	string variant::kind_str(kind value)
	{
		return kind_str_values[value];
	}
	variant::~variant()
	{
		switch(variant_kind)
		{
		case kind_string:
			value_string.~string();
			break;
		case kind_list:
			value_list.~vector<variant_ptr>();
			break;
		case kind_table:
			value_table.~map<string, variant_ptr>();
			break;
		case kind_function:
			value_function.~function<variant_ptr(const list&)>();
			break;
		case kind_null:
			value_null.~null_kind();
			break;
		}
	}

	variant_ptr operator+(variant_ptr l, variant_ptr r)
	{
		switch (l->variant_kind)
		{
		case variant::kind::kind_int:
			switch (r->variant_kind)
			{
			case variant::kind::kind_int:
				return make_variant(l->value_int + r->value_int);
			case variant::kind::kind_double:
				return make_variant(l->value_int + static_cast<long long>(r->value_double));
			case variant::kind::kind_string:
				return make_variant(l->value_int + stoll(r->value_string));
			case variant::kind::kind_function:
				return l + r->value_function({});
			}
			break;
		case variant::kind::kind_string:
			switch (r->variant_kind)
			{
			case variant::kind::kind_int:
				return make_variant(l->value_string + to_string(r->value_int));
			case variant::kind::kind_double:
				return make_variant(l->value_string + to_string(r->value_double));
			case variant::kind::kind_string:
				return make_variant(l->value_string + r->value_string);
			}
		case variant::kind::kind_function:
				// Recurse to get the function evaluated and eventually get to a concrete value to add
				return l->value_function({}) + r;
		}
		return null();
	}

	variant_ptr operator*(variant_ptr l, variant_ptr r)
	{
		switch (l->variant_kind)
		{
		case variant::kind::kind_int:
			switch (r->variant_kind)
			{
			case variant::kind::kind_int:
				return make_variant(l->value_int * r->value_int);
			case variant::kind::kind_double:
				return make_variant(l->value_int * static_cast<long long>(r->value_double));
			case variant::kind::kind_string:
				try
				{
					return make_variant(l->value_int * stoll(r->value_string));
				}
				catch (invalid_argument&)
				{
					string result = "";
					for (int i = 0; i < l->value_int; i++)
						result += r->value_string;
					return make_variant(result);
				}
				break;
			case variant::kind_function:
				return l * r->value_function({});
			}
			break;
		case variant::kind::kind_string:
			switch (r->variant_kind)
			{
			case variant::kind::kind_int:
			{
				string result = "";
				for (int i = 0; i < r->value_int; i++)
					result += l->value_string;
				return make_variant(result);
			}
			case variant::kind::kind_double:
				return make_variant(variant::null_kind());
			case variant::kind::kind_string:
				return make_variant(variant::null_kind());
			case variant::kind_function:
				return l * r->value_function({});
			}
			break;
		case variant::kind_function:
			return l->value_function({}) * r;
		}
		return null();
	}

	variant_ptr operator-(variant_ptr l, variant_ptr r)
	{
		return null();
	}

	variant_ptr operator/(variant_ptr l, variant_ptr r)
	{
		return null();
	}
}
