#ifndef CR_H_
#define CR_H_

#include <stdint.h>

/*
  struct {
    // callee saved registers
    uint64_t rbx;   // 0
    uint64_t rbp;   // 8
    uint64_t r12;   // 16
    uint64_t r13;   // 24
    uint64_t r14;   // 32
    uint64_t r15;   // 40
    // sse status register
    uint64_t mxcsr; // 48
    // rsp
    uint64_t rsp;   // 56
    // rip
    uint64_t rip;   // 64
    // data
    void*    data;  // 72
  };
*/

enum { RBX, RBP, R12, R13, R14, R15, MXCSR , RSP , RIP , DATA };

typedef uint64_t Context[10];

typedef void (*ContextFunction)( void* );
Context* CreateContext( ContextFunction , void* );
void FreeContext ( Context* );

void YieldContext( Context* , Context* );
void SwapContext ( Context* , Context* );
void RunInit     ( Context* , Context* );

#endif // CR_H_
