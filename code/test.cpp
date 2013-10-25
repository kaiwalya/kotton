#include "kotton/kotton.hpp"

#include <iostream>
#include <assert.h>
#include <vector>
#include <memory>

size_t gCount = 0;

int main () {
	//Single
	{
		kotton::stack s;
		kotton::fiber f([] (kotton::fiber_execution * ctx) {
			ctx->yield();
			std::cout << "Hello from fiber" << std::endl;
			ctx->yield();
		});
		
		kotton::fiber_execution exec(f, s);
		while(exec.proceed());
	}

	//Multiple
	{
		const size_t N = 128;
		struct Single {
			kotton::stack s;
			kotton::fiber f;
			kotton::fiber_execution e;
			Single():f([] (kotton::fiber_execution * ctx) {
				ctx->yield();
				std::cout << "Hello from fiber: " << ++gCount <<std::endl;
				ctx->yield();
			}), e(f,s) {
				
			}
			
			bool proceed() {
				s.checkGuard();
				auto ret = e.proceed();
				s.checkGuard();
				return ret;
			}
		};
		
		std::vector<std::shared_ptr<Single>> singles;
		for(auto i = N; i > 0; i--) {
			std::shared_ptr<Single> p(new Single);
			singles.push_back(p);
		}
		
		for (auto & s: singles) {
			s->proceed();
		}
		
		
		for (auto & s: singles) {
			s->proceed();
		}
		
		
		for (auto & s: singles) {
			s->proceed();
		}

	}

	return 0;
}
