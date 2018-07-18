#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "../src/sched.h"

Sched* kSched;

static void Slave( Sched* sched , void* data) {
  YieldData d;
  d.ptr = NULL;

  assert(data == NULL);
  assert(kSched == sched);

  printf("Slave1\n");
  SchedYield(sched,&d);
  printf("Slave2\n");
  SchedYield(sched,&d);
  printf("SlaveDone\n");
}

static int SchedulerSequence( Sched* sched , void* data) {
  assert(data  == NULL);
  assert(sched == kSched);

  Task* t = SchedNewTask(sched,Slave,NULL,10240);

  SchedRunTask(sched,t);
  assert(t->status == SCHED_PENDING);
  printf("Scheduler1\n");

  SchedRunTask(sched,t);
  assert(t->status == SCHED_PENDING);
  printf("Scheduler2\n");
  SchedRunTask(sched,t);
  assert(t->status == SCHED_DEAD);
  printf("SchedulerDone\n");
  SchedDeleteTask(sched,t);
  return 0;
}

int main() {
  kSched = SchedCreate(SchedulerSequence,NULL);
  assert( SchedStart(kSched,1) == 0 );
  return 0;
}
