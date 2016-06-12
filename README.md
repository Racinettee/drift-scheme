#drift::scheme
drift::scheme is an impelmentation of scheme designed to 
interact nicely with c++.
### Current working example as seen in main.cc:
```C++
	using namespace drift;
	// Create a context
	scheme::context context;
	// Set some data to be used in the script
	context["x"] = 100LL;
	// Load and run the script
	context.load_file("test.scm");
	// Call a function defined in test.scm
	context["lam"]("Jericho Billy", "Andrew");
```

To come:
- Tests, tests and tests