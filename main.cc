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
	context["x"] = 100LL;
	// Load and run the script
	context.load_file("test.scm");
	// Call a function defined in test.scm
	context["lam"]("Jericho Billy", "Andrew");

	assertions assert;
	assert.eq(100LL, context("100"));
	assert.eq(drift::string("Hello world"), context("\"Hello world\""));
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
