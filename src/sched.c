#include "sched.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// the tail
static Task kTail;
static Task* kCurrent;
static Context* kScheduler;
static uint64_t kTaskSize;

void InitTask( ContextFunction main , void* data ) {
  kTail.next = &kTail;
  kTail.prev = &kTail;
  kTaskSize  = 0;
  kCurrent   = NULL;
  kScheduler = CreateContext(main,data);
  // block into the main function and start working
  main(data);
}

Task* NewTask( ContextFunction main , void* data ) {
  Task* t = calloc(1,sizeof(Task));

  // link the object into the list
  t->prev = &kTail;
  kTail.next->prev = t;
  t->next          = kTail.next;
  kTail.next       = t;

  // create the ctx object
  t->ctx = CreateContext(main,data);
  t->status = INIT;

  ++kTaskSize;
  return t;
}

int RunTask( Task* t ) {
  kCurrent = t;
  switch(t->status) {
    case INIT:
      t->status = EXEC;
      RunInit    (kScheduler,t->ctx);
      break;
    case PENDING:
      t->status = EXEC;
      SwapContext(kScheduler,t->ctx);
      break;
    case DEAD:
      break;
    default:
      assert(0);
      break;
  }
  if(t->status != PENDING) t->status = DEAD;

  return t->status;
}

void YieldTask() {
  kCurrent->status = PENDING;
  YieldContext(kCurrent->ctx,kScheduler);
}

void DeleteTask( Task* t ) {
  assert(t->status == DEAD);
  t->prev->next = t->next;
  t->next->prev = t->prev;
  --kTaskSize;
  FreeContext(t->ctx);
  free(t);
}

Context* GetScheduler() { return kScheduler; }
Task*  GetCurrentTask() { return kCurrent; }

Task* ScheduleTask() {
  // iterate through all the task in the task list to find out whether we
  // have any pending task can be waken up
  for( Task* o = kTail.next; o != &kTail ; o = o->next ) {
    if(o->status == PENDING) {
      return o;
    }
  }
  return NULL;
}

uint64_t GetTaskSize() { return kTaskSize; }
