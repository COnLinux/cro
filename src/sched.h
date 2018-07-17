#ifndef SCHED_H_
#define SCHED_H_
#include "cr.h"

// status of the task
enum { INIT , EXEC, PENDING , DEAD };

typedef struct _Task {
  // double linked list
  struct _Task*   next;
  struct _Task*   prev;
  Context* ctx ;        // context stored for scheduler
  int status;           // status of the current task
  ContextFunction main; // main function
} Task;

void InitTask( ContextFunction , void* );
Task* NewTask( ContextFunction , void* );
int RunTask  ( Task* );
void YieldTask();
void DeleteTask( Task* );
Task* ScheduleTask();
Context* GetScheduler();

uint64_t GetTaskSize();

#endif // SCHED_H_
