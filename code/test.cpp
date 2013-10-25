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
	
	//Schedular
	{
		kotton::schedular s;
		
		auto producer = [](kotton::schedular & s){
			static const char msg[] = "Hello world!";
			s.writeCopy(0, msg, sizeof(msg));
		};

		auto transformer = [](kotton::schedular & s){
			static const char lf = '\n';
			s.linkInternal(0, 0);
			s.writeCopy(0, &lf, 1);
		};
		
		auto copier = [](kotton::schedular & s) {
			s.linkInternal(0, 0);
			s.linkInternal(0, 1);
		};
		
		auto consumer = [](kotton::schedular & s) {
			char c;
			s.readCopy(0, &c, 1);
			std::cout << c;
		};
		
		auto p = s.spawn(producer);
		auto t = s.spawn(transformer);
		s.link(p, t);
		
		auto cp = s.spawn(copier);
		s.link(t, cp);
		
		auto c = s.spawn(consumer);
		s.linkPort(cp, 0, c, 0);
		
		s.linkInput(cp, 1, 0);
		consumer(s);
		
	}

	return 0;
}
