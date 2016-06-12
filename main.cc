#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include "context.hh"

using namespace std;

int main() try
{
	using namespace drift;
	scheme::context context;
	context["x"] = 10000LL;
	context.load_file("test.scm");
	puts("Gonna try and call a scheme function in c++");
	//context["lam"]("Kilimanjaro Billy Boy", "Andrew the dress shirt kid");
	//context["x"] = 
}
catch (exception& e)
{
	cerr << e.what() << endl;
}
