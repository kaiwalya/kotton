#include "kotton/kotton.hpp"

#include <iostream>

int main () {
	
	kotton::stack s;
	kotton::fiber f([] () {
		std::cout << "Hello from fiber\n" << std::endl;
	});
	
	kotton::fiber_execution exec(f, s);
	while(exec.proceed());
	
	return 0;
}
