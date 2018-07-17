#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "../src/sched.h"


static void Another(void* data) {
  (void)data;
  int a = 10;
  int b = 20;
  int c = 30;
  int d = 40;
  double e = 5.5;
  double f = 7.6 + e;
  YieldTask();
  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
}

static void Scheduler(void* data) {
  (void)data;
  while(1) {

    for( int i = 1 ; i < 10 ; ++i ) {
      Task* t = NewTask(Another,NULL);
      RunTask(t);
      assert(t->status == PENDING);
    }

    printf("%d\n",GetTaskSize());

    if(GetTaskSize() > 100) {
      do {
        Task* nt = ScheduleTask();
        if(!nt) break;
        RunTask(nt);
        assert(nt->status == DEAD);
        DeleteTask(nt);
      } while(1);
    }

    sleep(1);
  }
}

int main() {
  InitTask(Scheduler,NULL);
  return 0;
}
