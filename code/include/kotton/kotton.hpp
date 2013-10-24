#if !defined(_KOTTON_HPP_)
#define _KOTTON_HPP_

#include <functional>
#include <setjmp.h>
#include <assert.h>

namespace kotton {
	
	using Func = std::function<void(void)>;
	
	enum class stack_growth {
		increasing, decreasing
	};
	
	struct stack {
		const size_t sz;
		char * const loc;
		
		stack(size_t size = 0):sz(size ? size : 1024), loc(new char[sz]) {
			
		}
		
		~stack() {
			delete [] loc;
		}
		
		bool isCurrent () const {
			char t;
			char * pt = &t;
			
			if (pt >= loc && (pt + 1) <= (sz + loc)) {
				return true;
			}
			return false;
		}
		
		size_t freeSpace() const{
			assert(isCurrent());
			char t;
			char * pt = &t;
			return pt - loc;
		}
		
		void installGuard() const{
			assert(isCurrent());
			assert(sz >= sizeof(uint64_t) && freeSpace() >= sizeof(uint64_t));
			static const char magicVal[sizeof(uint64_t)] = "kotton!";
			*(uint64_t *)loc = *(uint64_t *)magicVal;
		}
		
		bool checkGuard() const {
			assert(isCurrent());
			assert(sz >= sizeof(uint64_t) && freeSpace() >= sizeof(uint64_t));
			static const char magicVal[sizeof(uint64_t)] = "kotton!";
			return *(uint64_t *)loc == *(uint64_t *)magicVal;
		}
		
	};
	
	
	struct fiber {
		
		fiber(Func & f):m_f(f){};
		fiber(Func && f): m_f(f){};
		
		const Func & getFunc() const {return m_f;}
		
	private:
		const Func m_f;
	};
	
	enum class fiber_state {
		notReady, paused, playing, finished
	};
	struct fiber_execution {
		
		fiber_execution(const fiber & f, const stack & s)
		:m_state(fiber_state::notReady),
		m_enterReturn(false),
		m_f(f),
		m_s(s){
			enter();
		}
		

		bool proceed();
		
	private:

		bool enter();
		void return_barrier();
		
		void swap();
		fiber_state m_state;
		bool m_enterReturn;
		jmp_buf m_targetbuff;
		jmp_buf m_exitBuff;
		const fiber & m_f;
		const stack & m_s;


	};
	
}

#endif