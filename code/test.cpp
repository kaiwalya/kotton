#include "kotton/kotton.hpp"

#include <iostream>
#include <assert.h>
#include <vector>
#include <memory>
#include <exception>

size_t gCount = 0;

void throwIf(bool pred) {
	if (pred)
		throw new std::bad_exception();
}

int main () {
	//Single
	{
		kotton::stack s;
		kotton::fiber f([] (kotton::execution * ctx) {
			ctx->yield();
			gCount++;
			ctx->yield();
		});
		
		kotton::execution exec(f, s);
		while(exec.proceed());
		throwIf(gCount != 1);
	}

	//Multiple
	{
		gCount = 0;
		const size_t N = 128;
		struct Single {
			kotton::stack s;
			kotton::fiber f;
			kotton::execution e;
			Single():f([] (kotton::execution * ctx) {
				ctx->yield();
				gCount++;
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
		
		throwIf(gCount != N);

	}

	return 0;
}
