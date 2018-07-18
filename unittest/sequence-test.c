#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "../src/sched.h"

static void Slave(void* data) {
  (void)data;
  printf("Slave1\n");
  YieldTask();
  printf("Slave2\n");
  YieldTask();
  printf("SlaveDone\n");
}

static void SchedulerSequence(void* data) {
  (void)data;

  Task* t = NewTask(Slave,NULL);
  RunTask(t);
  assert(t->status == PENDING);
  printf("Scheduler1\n");
  RunTask(t);
  assert(t->status == PENDING);
  printf("Scheduler2\n");
  RunTask(t);
  assert(t->status == DEAD);
  printf("SchedulerDone\n");
  DeleteTask(t);
}

int main() {
  InitTask(SchedulerSequence,NULL);
  return 0;
}

