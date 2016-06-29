#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include "context.hh"

using namespace std;

class assertions
{
public:
	assertions() = default;
	~assertions()
	{
		printf("Assertions %s: %lu failed; %lu run;\n", failed_assertions == 0 ?
			"succeeded" : "failed", failed_assertions, run_assertions);
	}
	template<class T> void eq(const T& val, const drift::scheme::selector& res, const string& msg = "")
	{
		if (val != res.as<T>()) {
			++failed_assertions;
			cout << msg << " : " << val << " not equivalent to " << res.as<T>() << endl;
		}
		++run_assertions;
	}
	template<class T> void neq(const T& val, const drift::scheme::selector& res, const string& msg = "")
	{
		if (val == res.as<T>()) {
			++failed_assertions;
			cout << msg << " : " << val << " is equivalent to " << res.as<T>() << endl;
		}
		++run_assertions;
	}
	template<class T> void not_type(const drift::scheme::selector& res)
	{
		try {
			res.as<T>();
			++failed_assertions;
		}
		catch (drift::variant::incorrect_treatment&){ }
		++run_assertions;
	}
	template<class T> void is_type(const drift::scheme::selector& res)
	{
		try {
			res.as<T>();
		}
		catch(...) {
			++failed_assertions;
		}
		++run_assertions;
	}
	void no_throw(std::function<void()> fn)
	{
		try
		{
			fn();
		}
		catch(...)
		{
			++failed_assertions;
		}
		++run_assertions;
	}
private:
	std::size_t failed_assertions = 0;
	std::size_t run_assertions = 0;
};
#define equ(l, r) eq(l, r, #l " == " #r)
#define nequ(l, r) neq(l, r, #l " != " #r)

int main() try
{
	using namespace drift;
	// Create a context
	scheme::context context;
	// Set some data to be used in the script
	context["x"s] = 100LL;
	// Load and run the script
	context.load_file("test.scm");
	// Call a function defined in test.scm
	context["lam"s]("Jericho Billy"s, "Andrew"s);

	assertions assert;
	puts("Test empty expressions: '()' expect a warning.");
	assert.no_throw([&context]() { context("()"); });
	puts("Test atomic values (expressions that are just literals)");
	assert.eq(100LL, context("100"s));
	assert.eq("Hello world"s, context(R"("Hello world")"s));
	assert.eq(3.1415, context("3.1415"s));
	puts("Test addition and multiplication");
	assert.eq(10LL, context("(+ 1 2 3 4)"s)());
	assert.eq(100LL, context("(* 5 5 4)"s)());
	puts("Test that a function returning an atomic returns the correct value");
	assert.eq("Scheme"s, context(R"((lambda () "Scheme"))"s)());
	puts("Type tests");
	assert.not_type<drift::string>(context("100000"s)); // long long
	assert.not_type<long long>(context("(+ 1 2 3 4)")); // function
	assert.is_type<long long>(context("(+ 1 2 3 4)")()); // function call -> long long
	puts("Return a lambda and see it as function");
	assert.is_type<drift::function>(context(R"((lambda () "Scheme"))"s));
	puts("Lambda returned + called in C++ returns string");
	assert.is_type<drift::string>(context(R"((lambda () "Scheme"))"s)());
	puts("Feature tests");
	puts("Lambda defined in context() & prints argument provided by C++:");
	assert.no_throw([&context](){
		context("(lambda (msg) (println \"Message from C++: \" msg))")("Hello world... again");
	});
	puts("Multiple equality");
	assert.equ(true, context("(== 3 3 3 3)")());
	assert.equ(false, context("(== 3 3 4 3)")());
	assert.equ(true, context("(!= 1 2 3 4 5)")());
	assert.equ(false, context("(!= 5 5 5 5 5 5)")());
	puts("Assign a variable to a list");
	assert.no_throw([&context](){
		context("(set! ll (list 10 20 30 40))");
	});
	puts("Test the map function");
	assert.no_throw([&context](){
		context("(println (map (lambda (x) (* x x)) (list 1 2 3 4)))")();
	});
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
