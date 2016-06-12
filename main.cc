#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include "context.hh"

using namespace std;

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
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
