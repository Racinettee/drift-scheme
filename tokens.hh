#pragma once
#include <string>
#include <vector>
#include <utility>
#include <cstdio>
#include "variant.hh"
#ifdef DEBUG
#include <iostream>
#endif
namespace drift
{
	enum token_type
	{
		Keyword,//0
		Int,
		Float,// 2
		Num,
		Str,// 4
		Comp,
		Arith,//6
		Semicolon,
		Colon,//8
		LParen,
		RParen,//10
		LBrace,
		RBrace,//12
		LBracket,
		RBracket,//14
		Comma,
		Quote,//16
		Identifier,
		LThan,//18
		GThan,
		// Conditional
		Conditional,//20
		// Bitwise
		Bitwise,
		// Comment types
		LineComment,//22
		MultilineComment,
		Null,//24
		Not
	};

	typedef std::pair<token_type, variant_ptr> token;
	typedef std::vector< token > token_array;

	const std::string token_string(token_type);

	inline void print_token(const token& value)
	{
		switch(value.first)
		{
			case token_type::Str:
			case token_type::Identifier:
			case token_type::Keyword:
				std::puts(value.second->value_string.c_str());
				break;
			case token_type::Int:
				std::printf("%lli", value.second->value_int);
				break;
			case token_type::Float:
			case token_type::Num:
				std::printf("%f", value.second->value_double);
				break;
			default:
				std::putc(value.second->value_char, stdout);
		}
	}
}
