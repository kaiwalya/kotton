#include "kotton/kotton.hpp"

#include <iostream>

int main () {
	
	kotton::stack s;
	kotton::fiber f([] (kotton::fiber_execution * ctx) {
		ctx->yield();
		std::cout << "Hello from fiber\n" << std::endl;
		ctx->yield();
	});
	
	kotton::fiber_execution exec(f, s);
	while(exec.proceed());
	
	return 0;
}
