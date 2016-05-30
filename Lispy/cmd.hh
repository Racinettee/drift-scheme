#pragma once
#pragma once
namespace lispy
{
	enum cmd : long long
	{
		// Push element onto stack
		// 1 arg: element to put on stack
		push,
		// Pushes the next N elements in the instruction set to the stack
		push_n, //1
				// Removes element from stack
				// 0 args
		pop,
		// Pops N elements from the stack
		pop_n, //3
			   // Adds to elements from the stack together
			   // Pops 2 from the stack
		add,
		// Same as add but subtracts instead
		sub, //5
			 // Same as sub but muls instead
		mul,
		// Same as mul but divs instead
		div, //7
			 // Sums n elements from the stack together
			 // Uses top element of the stack as the number of elements to sum
		sum,
		// Pow pops 2 elements off the stack. Raises the first by the second
		// Pushes result on to the stack
		pow, //9
			 // FCall calls a void(*)(), pops 1 from the stack and converts it to an address
		c_fncall,
		// Mod, pops 2 -
		mod, //11
			 // also need conditional jumps, indexed tables, boolean stuff
			 // Pops that stack and jumps if the value is not 0
			 // A value to increment the instruction pointer by follows
		// Jump n instructions (relative)
		jump,
		jump_true,
		// Pops the stack and jumps if the value is 0
		// A value to increment the instruction pointer by follows
		jump_false,
		// Compares two values off the stack and pushes 0 or 1 back
		cmp,
		// Compares for inequality same as cmp otherwise
		cmp_ineq,
		// Set an object at a location in memory
		set,
		// An end token to help cope with a jump to then end of a body
		end,
	};
}
