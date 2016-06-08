#pragma once
#include "variant.hh"
namespace lispy
{
	namespace lib
	{
		variant_ptr print(const lispy::list&);
		variant_ptr println(const lispy::list&);
	}
}