#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <list>
#include "variant.hh"
#include "lex.hh"
#include "cmd.hh"
#include "env.hh"
#include "variant.hh"

using namespace std;

namespace lispy
{
	void eval(struct method* fn);

	method parse(environment&, const token_array&);
}
int main() try
{
	using namespace lispy;
	lexer lex;
	lex.add_keyword("define");
	lex.add_keyword("set");
	lex.add_keyword("lambda");
	lex.add_keyword("begin");
	token_array test_tokens = lex.lex_file("test.scm"); //lex.lex_string("(print \"Hello from drift::lisp \" (* 2 1234) \" \" (if 0 \"and good tidings\" \"not!\"))(+ 1 2 3 4 5)(print (+ x 101)");
	//"(+ 1 2 3)(+ 2 3 4)(+ 1 2 3 (+ 2 3 4))(if 0 \"One\" \"Zero\")");
	environment env = std_env();
	env.symbols["x"] = make_shared<variant>(100LL);

	method fn = parse(env, test_tokens);

	for (auto& expr : fn.expressions)
		expr->value_function();
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
