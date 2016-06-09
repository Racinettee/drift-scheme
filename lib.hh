#pragma once
#include "variant.hh"
namespace lispy
{
	namespace lib
	{
		variant_ptr print(const list&);
		variant_ptr println(const list&);
		variant_ptr readln(const list&);
		variant_ptr foreach(const list&);
	}
}