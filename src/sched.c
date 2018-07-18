#include "sched.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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
    // sche
    void*    sche;  // 72
    // data
    void*    data;  // 80
  };
*/

typedef uint64_t Context[11];

// index into the Context array
enum {
  RBX = 0,
  RBP,
  R12,
  R13,
  R14,
  R15,
  MXCSR,
  RSP,
  RIP,
  SCHED,
  DATA
};

static
void ContextInit( Sched* sche , Context* ctx , void* rsp , uint32_t ssize   ,
                                                           SlaveCallback cb ,
                                                           void* udata      ) {
  (*ctx)[RSP]  = (uint64_t)rsp;
  (*ctx)[RIP]  = (uint64_t)cb;
  (*ctx)[SCHED]= (uint64_t)sche;
  (*ctx)[DATA] = (uint64_t)udata;
}

// The following functions are provided by external assembly code
extern void _CoRoutineSwap( void* , void* );
extern void _CoRoutineInit( void* , void* );

#define ALIGN(x,m) (((x) + (m)-1) & ~((m)-1))

Sched* SchedCreate( SchedulerCallback cb , void* data ) {
  Sched* r = malloc(sizeof(*r) + sizeof(Context));

  r->tail.next = &(r->tail);
  r->tail.prev = &(r->tail);
  r->size      = 0;
  r->current   = NULL;
  r->scheduler = cb;
  r->data      = data;
  r->sctx      = ((char*)(r) + sizeof(*r));

  return r;
}

Task* SchedNewTask( Sched* sched , SlaveCallback cb , void* data , uint32_t ssize ) {
  Task* t = NULL;
  uint32_t sz = ALIGN(sizeof(*t)+sizeof(Context)+ssize,16);
  if(posix_memalign((void**)&t,16,sz)) return NULL;

  // 1. insert the task into the chain
  sched->tail.next->prev = t;
  t->next                = sched->tail.next;
  t->prev                = &(sched->tail);
  sched->tail.next       = t;
  ++sched->size;

  // 2. setup the context field
  t->ctx = (void*)((char*)(t) + sizeof(*t));

  // 3. setup the status
  t->status = SCHED_INIT;
  t->yield.ptr = NULL;

  // 4. initialize the *context*
  ContextInit( sched , t->ctx, (char*)(t) + sz , ssize , cb , data );

  return t;
}

int SchedRunTask( Sched* sched , Task* t ) {
  // assign the task t into the current field of scheduler
  sched->current = t;
  switch(t->status) {
    case SCHED_INIT:
      t->status = SCHED_EXEC;
      _CoRoutineInit(sched->sctx,t->ctx);
      break;
    case SCHED_PENDING:
      t->status = SCHED_EXEC;
      _CoRoutineSwap(sched->sctx,t->ctx);
      break;
    default:
      break;
  }

  if(t->status != SCHED_PENDING)
    t->status = SCHED_DEAD;

  return t->status;
}

void SchedYield( Sched* sched , const YieldData* data ) {
  sched->current->yield = *data;
  sched->current->status= SCHED_PENDING;
  _CoRoutineSwap(sched->current->ctx,sched->sctx);
}

void SchedDeleteTask( Sched* sched, Task* t ) {
  assert(t->status == SCHED_DEAD);
  SchedTerminateTask(sched,t);
}

void SchedTerminateTask( Sched* sched, Task* t ) {
  t->prev->next = t->next;
  t->next->prev = t->prev;
  --sched->size;
  free(t);
}

void SchedDelete( Sched* sched ) {
  // delete each task one by one
  for( Task* l = sched->tail.next; l != SchedTail(*sched); ) {
    Task* n = l->next;
    free(l);
    l = n;
  }

  // delete sched itself
  free(sched);
}

int SchedStart( Sched* sched , int flag ) {
  int ret = sched->scheduler(sched,sched->data);
  if(flag) SchedDelete(sched);
  return ret;
}
