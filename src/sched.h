#ifndef SCHED_H_
#define SCHED_H_
#include <stdint.h>

// A simple user space coroutine style scheduler. The model for this is basded on
// following style :
//
// The user can create an object called TaskGroup and related it into a scheduler
// functions. The scheduler function are supposed to manage the coroutine creation
// and resume. When a new Task is created and Yieled , the scheduler function is
// resumed. Then inside of the scheduler function user can find out next available
// coroutine to resume its invocation or block into system call to wait for IO event.
//
// Each coroutine will have its own execution stack so it is properly sandboxed with
// each other. Currently we don't have code to prevent crashing if a coroutine failed
// at SEGV due to stack overflow. In the future, we may add it.

// The status of each schedulable task
typedef enum {
  SCHED_INIT,    // the task is just initialized, not executed
  SCHED_EXEC,    // the task is executing for now
  SCHED_PENDING, // the task is pending now, due to YieldTask call
  SCHED_DEAD     // the task is dead means the function correctly returns
} TaskStatus;

// Yield data. User can set this yield data to the task to help identify the
// reason why the function needs to be yielded. Example like, I yield because
// I want to read data from IO. So later on the scheduler can wake up this
// function if the fd can be readed.
typedef union {
  void* ptr;
  int  flag;
} YieldData;

struct _Sched;
struct _Task;

// Callback function for *slave*
typedef void (*SlaveCallback)     ( struct _Sched* , void* );
typedef int  (*SchedulerCallback) ( struct _Sched* , void* );

// The task represents the context of the coroutine.
typedef struct _Task {
  struct _Task*   next; // prev link
  struct _Task*   prev; // next link
  void*            ctx; // context stored for scheduler
  TaskStatus    status; // status of the current task
  YieldData      yield; // the yielded data
} Task;

// The scheduler struct itself
typedef struct _Sched {
  void*                  sctx;
  Task                   tail;
  uint32_t               size;
  Task*               current;
  SchedulerCallback scheduler;
  void*                  data;
} Sched;

// Create a scheduler object for later setup the scheduler model
Sched* SchedCreate ( SchedulerCallback , void* );

// Scheduler start will run the scheduler callback functions until
// it finished. if the flag sets to 1 then after the scheduler is
// done, it will destroy the sched object automatically. The return
// value is the return value of your SchedulerCallback functions
int    SchedStart  ( Sched* , int );

// Create a coroutine task from the specified scheduler
Task*  SchedNewTask( Sched* , SlaveCallback , void* , uint32_t );

// Run a coroutine task. If the task is init, then it starts to run;
// if the task is in pending status , then it resumes the coroutine.
int    SchedRunTask( Sched* , Task* );

// Yield from the current task to the scheduler
void   SchedYield  ( Sched* , const YieldData* );

// Delete the input task from the specified Sched object. If a task
// is *not* in DEAD status, we cannot delete it safely or you are sure
// we can delete it safely.
void   SchedDeleteTask( Sched* , Task* );

// This force a deletion of the task regardless of its status
void   SchedTerminateTask( Sched* , Task* );

// Delete a Sched object. The deletion will delete Sched's all lingering
// tasks and destroy its related memory. This is not safe at all, the only
// safe way to use it is that all the task related to this Sched is finished
// or in *DEAD* status
void   SchedDelete   ( Sched* );

// Some helper macros for iteration of the list
#define SchedTail(SCHE)     (&((SCHE).tail))
#define SchedTaskSize(SCHE) ((SCHE).size)

#endif // SCHED_H_
