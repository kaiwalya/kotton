#if !defined(_KOTTON_HPP_)
#define _KOTTON_HPP_

#include <functional>
#include <setjmp.h>
#include <assert.h>
#include <unistd.h>

static const int gPageSize = getpagesize();

namespace kotton {
	struct execution;
	using Func = std::function<void(execution *)>;
	static const size_t guardSize = sizeof(int64_t) * 2;
	static const char guardData[sizeof(uint64_t)] = "kotton!";
	
	enum class stack_growth {
		increasing, decreasing
	};
	
	struct stack {
		const size_t sz;
		char * const loc;
		
		stack(size_t size = 0):sz(size ? size : gPageSize), loc(new char[sz]) {
			installGuard();
		}
		
		~stack() {
			checkGuard();
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
			assert(sz > guardSize);
			memset_pattern8(loc, guardData, guardSize);
		}
		
		bool checkGuard() const {
			assert(sz > sizeof(int64_t));
			return memcmp(loc, guardData, guardSize) == 0;
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
	struct execution {
		
		execution(const fiber & f, const stack & s)
		:m_state(fiber_state::notReady),
		m_enterReturn(false),
		m_f(f),
		m_s(s){
			enter();
		}
		
		bool proceed();
		void yield();
		
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