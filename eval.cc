#include <algorithm>
#include <numeric>
#include <memory>
#include "env.hh"
#include "cmd.hh"
using namespace std;
namespace lispy
{
	void eval(method* fn)
	{
		auto& inst = fn->instructions;
		auto& stack = fn->stack;
		for (size_t i = 0; i < inst.size(); i++)
		{
			switch (inst[i])
			{
			case push:
				stack.emplace_back(*reinterpret_cast<variant_ptr*>(inst[++i]));
				break;
				/*case push_n:
				{
				long long count = inst[++i];
				for(unsigned int j = 0; j < count; j++)
				_the_stack.push(inst[++i]);
				}*/
				//break;
			case pop:
				stack.pop_back();
				break;
			case pop_n:
				for (unsigned int j = 0; j < inst[++i]; j++)
					stack.pop_back();
				break;
			case sum:
			{
				long long count = stack.back()->value_int; stack.pop_back();
				auto rend = stack.end();
				auto sum = std::accumulate(rend - count, rend, *rend);
				// Idea - instead of poping every last element when we're going to push something back onto the stack
				// pop all but the last element to come off, and then just modify that value with the result
				// ---------------
				// this optimization will save us one loop iteration, one pop and one push
				// see mod for an example, as it was created more recently
				// The new stack implementation can pop a number of items at once
				for (int i = 0; i < count; i++)
					stack.pop_back();
				stack.emplace_back(sum);
			}
			break;
			case add:
			{
				variant_ptr right = stack.back(); stack.pop_back();
				variant_ptr left = stack.back(); stack.pop_back();
				stack.emplace_back(left + right);
			}
			break;
			case sub:
			{
				//variant_ptr right = stack.back(); stack.pop_back();
				//variant_ptr left = stack.back(); stack.pop_back();
				//stack.emplace_back(left - right);
			}
			break;
			case mul:
			{
				variant_ptr right = stack.back(); stack.pop_back();
				variant_ptr left = stack.back(); stack.pop_back();
				stack.emplace_back(left * right);
			}
			break;
			case div:
			{
				//variant_ptr right = stack.back(); stack.pop_back();
				//variant_ptr left = stack.back(); stack.pop_back();
				//stack.emplace_back(left / right);
			}
			break;
			case pow:
			{
				long long power = stack.back()->value_int; stack.pop_back();
				long long base = stack.back()->value_int; stack.pop_back();
				stack.emplace_back(std::make_shared<variant>(std::pow(base, power)));
			}
			break;
			case c_fncall:
				//reinterpret_cast<void(*)()>(stack.top())();
				//stack.pop();
				break;
			case mod:
			{
				// left will stay in the stack and become the result
				//long long right = stack.back().value_int; stack.pop_back();
				//long long left = stack.back().value_int; stack.pop_back();
				//stack.emplace_back(left % right);
			}
			break;
			case cmp:
			{
				variant_ptr right = stack.back(); stack.pop_back();
				variant_ptr left = stack.back(); stack.pop_back();
				stack.emplace_back(make_shared<variant>(left == right));
			}
			break;
			case cmp_ineq:
			{
				//int right = stack.top(); stack.pop();
				//int left  = stack.top(); stack.pop();
				//stack.emplace_back(right > left ? 1 : -1);
			}
			break;
			case jump_true:
			{
				long long value = stack.back()->value_int; stack.pop_back();
				int length = inst[++i];
				if (value > 0LL)
					i += length;
			}
			break;
			case jump:
				i += inst[++i];
				break;
			case jump_false:
			{
				long long value = stack.back()->value_int; stack.pop_back();
				int length = inst[++i];
				if (value == 0LL)
					i += length;
			}
			break;
			case set:
			{
				variant_ptr& location = *reinterpret_cast<variant_ptr*>(inst[++i]);
				variant_ptr value = stack.back(); stack.pop_back();
				location = value;
			}
			break;
			}
		}
	}
}