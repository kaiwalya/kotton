#include "kotton/kotton.hpp"


using namespace kotton;

bool execution::enter() {
	if (!m_s.isCurrent()) {
		auto stackTop = m_s.loc + m_s.sz;
		
		stackTop = (char *)((intptr_t)stackTop & ~(intptr_t)15);
		
		constexpr bool is32Bit = sizeof(void *) == 4;
		constexpr bool is64Bit = sizeof(void *) == 8;
		static_assert(is32Bit || is64Bit, "Current only 32 or 64 bit");
		
		if (is64Bit) {
			asm ("pushq %%rbp;"
				 "movq %%rsp, %%rbp;"
				 "movq %[stackTopR], %%rsp;"
				 :
				 :[stackTopR]"r"(stackTop)
				 :
				 );
			
			this->return_barrier();
			
			asm ("movq %rbp, %rsp;"
				 "popq %rbp;"
				 );
		}
		else if (is32Bit) {
			asm ("pushl %%ebp;"
				 "movl %%esp, %%ebp;"
				 "movl %[stackTopR], %%esp;"
				 :
				 :[stackTopR]"r"(stackTop)
				 :
				 );
			
			this->return_barrier();
			
			asm ("movl %ebp, %esp;"
				 "popl %ebp;"
				 );
		}
		
	}
	else {
		assert(false);
	}
	if (m_enterReturn) {
		m_enterReturn = false;
		return true;
	}
	return false;
}

void execution::return_barrier() {
	swap();
}

void execution::swap() {
	if (m_state == fiber_state::notReady) {
		m_s.installGuard();
		m_enterReturn = true;
		if (setjmp(m_targetbuff) == 0) {
			m_state = fiber_state::paused;
			return;
		}
		m_f.getFunc()(this);
		m_s.checkGuard();
	
		
		m_state = fiber_state::finished;
		longjmp(m_exitBuff, 1);
		assert(false);
	}
	if (m_state == fiber_state::paused) {
		if (setjmp(m_exitBuff) == 0) {
			m_state = fiber_state::playing;
			longjmp(m_targetbuff, 1);
		}
		return;
	}
	else if (m_state == fiber_state::playing){
		if (setjmp(m_targetbuff) == 0) {
			m_state = fiber_state::paused;
			longjmp(m_exitBuff, 1);
		}
		return;
	}
	else {
		assert(false);
	}
}

bool execution::proceed() {
	assert(!m_s.isCurrent());
	swap();
	return m_state == fiber_state::paused;
}

void execution::yield() {
	assert(m_s.isCurrent());
	swap();
}
