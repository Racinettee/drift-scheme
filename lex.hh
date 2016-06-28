#pragma once
#include <unordered_map>

#include "tokens.hh"

namespace drift
{
	class lexer
	{
	public:
		token_array lex_input(std::istream& input);
		token_array lex_file(const std::string&);
		token_array lex_string(const std::string&);
		void add_keyword(const std::string&);
		lexer();
	private:
		void lex_number(std::istream&, token_array&);
		void lex_identifier(std::istream&, token_array&);
		int lex_syntax(std::istream&, token_array&);
		void lex_string(std::istream&, token_array&, char);
		bool is_whitespace(char);
		bool syntax_look(std::istream&, std::string&, int&);
	private:
		// Text editors start with line one
		long long line_number = 1LL;
		long long column_index = 1LL;
		std::unordered_map<std::string, token_type> token_key;
	};
}
