#include "cr.h"
#include <stdio.h>
#include <stdlib.h>

// To be ABI compatible , this value must be aligned to be 16
static const uint32_t kSizeOfStack = 10240;

// FIXME:: do not use malloc but use mmap and put a readonly page after the page
//         allocated to detect stack overflow otherwise it is extreamly hard to
//         detect any sort of stack overflow w.r.t coroutine in runtime
Context* CreateContext( ContextFunction main , void* data ) {
  Context* ctx   = malloc(sizeof(Context) + kSizeOfStack);
  // RSP is growing in reverse direction
  (*ctx)[RSP]       = (uint64_t)((char*)(ctx) + sizeof(Context) + kSizeOfStack);
  (*ctx)[RIP]       = (uint64_t)(main);
  (*ctx)[DATA]      = (uint64_t)(data);
  return ctx;
}

void FreeContext( Context* ctx ) {
  free(ctx);
}
