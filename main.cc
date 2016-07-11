#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include "context.hh"
#include "rlutil/rlutil.h"
using namespace std;



template<class test_sig> class assertions
{
	struct unit_test {
		std::size_t assertions_run = 0ULL;
		std::size_t assertions_failed = 0ULL;
		std::string message = "";
		std::string diagnostic = "";
		std::function<test_sig> test;
		unit_test() = default;
		unit_test(std::function<test_sig> fn, const std::string& msg, const std::string& diag):
			test(fn), message(msg), diagnostic(diag) { }
	};
	std::vector<unit_test> unit_tests;
	std::unique_ptr<drift::schemy::context> context;
	std::size_t units_run = 0ULL;
	std::size_t units_failed = 0ULL;
public:
	assertions() = default;
	~assertions()
	{
		printf("Assertions %s: %lu failed; %lu run;\n", failed_assertions == 0 ?
			"succeeded" : "failed", failed_assertions, run_assertions);
	}

	void test(std::string msg, std::function<test_sig> unit_test, std::string diag = "")
	{
		tests.emplace_back(unit_test, msg, diag);
	}
	void setup()
	{
		context.reset();
	}
	void tear_down()
	{

	}
	void run_tests()
	{
		for(auto& unit_test : unit_tests)
		{
			setup();
			try {
				unit_test.test(*context);
			}
			catch(exception& e) {

			}
			catch(...) {

			}
			tear_down();
		}
	}
	template<class T> void eq(const T& val, const drift::schemy::selector& res, const string& msg = "")
	{
		if (val != res.as<T>()) {
			++failed_assertions;
			cout << msg << " : " << val << " not equivalent to " << res.as<T>() << endl;
		}
		++run_assertions;
	}
	template<class T> void neq(const T& val, const drift::schemy::selector& res, const string& msg = "")
	{
		if (val == res.as<T>()) {
			++failed_assertions;
			cout << msg << " : " << val << " is equivalent to " << res.as<T>() << endl;
		}
		++run_assertions;
	}
	template<class T> void not_type(const drift::schemy::selector& res)
	{
		try {
			res.as<T>();
			++failed_assertions;
		}
		catch (drift::variant::incorrect_treatment&){ }
		++run_assertions;
	}
	template<class T> void is_type(const drift::schemy::selector& res)
	{
		try {
			res.as<T>();
		}
		catch(...) {
			++failed_assertions;
		}
		++run_assertions;
	}
	void no_throw(std::function<void()> fn, const std::string& m)
	{
		try {
			fn();
		}
		catch(exception& e) { ++failed_assertions; printf("%s: %s\n", m.c_str(), e.what()); }
		catch (...) { ++failed_assertions; puts(m.c_str()); }
		++run_assertions;
	}
	void throws(std::function<void()> fn)
	{
		try	{
			fn();
			++failed_assertions;
		}
		catch(...) { }
		++run_assertions;
	}
private:
	std::size_t failed_assertions = 0;
	std::size_t run_assertions = 0;
};
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define equ(l, r) eq(l, r, #l " == " #r)
#define nequ(l, r) neq(l, r, #l " != " #r)
#define nothrow(l) no_throw(l, "No throw assertion at line: " TOSTRING(__LINE__) " threw")

int main() try
{
	using namespace drift;
	// Create a context
	schemy::context context;
	// Set some data to be used in the script
	context["x"s] = 100LL;
	// Load and run the script
	rlutil::setColor(rlutil::YELLOW);
	puts("TEST: Load & Run Schemy file");
	//context.load_file("test.scm");

	puts("TEST: Within C++");
	// Call a function defined in test.scm
	//context["lam"s]("Jericho Billy"s, "Andrew"s);

	assertions assert;
	assert.throws([&context](){
		context(")");
	});
	puts("Test empty expressions: '()' expect a warning.");
	assert.nothrow([&context]() { context("()"); });
	puts("Test atomic values (expressions that are just literals)");
	assert.equ(100LL, context("100"s));
	assert.equ("Hello world"s, context(R"("Hello world")"s));
	assert.equ(3.1415, context("3.1415"s));
	puts("Test addition, multiplication and subtraction");
	assert.equ(10LL, context("(+ 1 2 3 4)"s)());
	assert.equ(100LL, context("(* 5 5 4)"s)());
	assert.equ(-9LL, context("(- 1 10)"s)());
	puts("Test that a function returning an atomic returns the correct value");
	assert.equ("Scheme"s, context(R"((lambda () "Scheme"))"s)());
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
	assert.nothrow([&context](){
		context("(lambda (msg) (println \"Message from C++: \" msg))")("Hello world... again");
	});
	puts("Multiple equality");
	assert.equ(true, context("(== 3 3 3 3)")());
	assert.equ(false, context("(== 3 3 4 3)")());
	assert.equ(true, context("(!= 1 2 3 4 5)")());
	assert.equ(false, context("(!= 5 5 5 5 5 5)")());
	puts("Assign a variable to a list");
	assert.nothrow([&context]() {
		context("(define ll (list 10 20 30 40))")();
	});
	puts("Test the map function");
	assert.nothrow([&context]() {
		context("(println (map (lambda (x) (* x x)) (list 1 2 3 4)))")();
	});
	assert.nothrow([&context]() {
		context("(println (map (lambda (x) (* x x)) ll))")();
	});
	assert.nothrow([&context]() {
		context("(define cube (lambda (x) (* x x x)))")();
	});
	assert.nothrow([&context]() {
		context("(println (map cube (list 1 2 3 4)))")();
	});
	context("(define echo (lambda (v) v))")();

	assert.nothrow([&context]()
	{
		context("(define fact (lambda (n) "
					"(if (== n 0) 1 (* n (fact (- n 1))))))")();
	});

	assert.equ(3628800LL, context("(fact 10)")());

	return EXIT_SUCCESS;
}
catch (exception& e)
{
	cerr << e.what() << endl;
	return EXIT_FAILURE;
}
