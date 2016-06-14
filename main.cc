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
		printf("Assertions: %i failed; %i run;\n", failed_assertions, run_assertions);
	}
	template<class T> void eq(const T& val, const drift::scheme::selector& res)
	{
		if (val != res.as<T>())
			++failed_assertions;
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
private:
	std::size_t failed_assertions = 0;
	std::size_t run_assertions = 0;
};

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
	assert.eq(100LL, context("100"s));
	assert.eq("Hello world"s, context("\"Hello world\""s));
	assert.eq(3.1415, context("3.1415"s));
	assert.eq(10LL, context("(+ 1 2 3 4)"s));
	assert.eq(100LL, context("(* 5 5 4)"s));
	assert.eq("Scheme"s, context(R"((lambda () "Scheme"))"));
	assert.not_type<drift::string>(context("100000"s));
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
