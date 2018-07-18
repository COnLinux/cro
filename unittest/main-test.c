#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include "../src/sched.h"

static void Another(Sched* sched , void* data) {
  YieldData dd;
  int a = 10;
  int b = 20;
  int c = 30;
  int d = 40;
  double e = 5.5;
  double f = 7.6 + e;
  dd.ptr = NULL;
  (void)data;

  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
  SchedYield(sched,&dd);
  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
  SchedYield(sched,&dd);
  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
  SchedYield(sched,&dd);
  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
  SchedYield(sched,&dd);
  printf("Here:%d,%d,%d,%d,%f,%f\n",a,b,c,d,e,f);
}

static int Resume(Sched* sched) {
  uint32_t sz = sched->size;
  uint32_t target = rand() % 1771;
  return sz > target;
}

static int Scheduler(Sched* sched,void* data) {
  (void)data;

  while(1) {

    for( int i = 1 ; i < 100 ; ++i ) {
      Task* t = SchedNewTask(sched,Another,NULL,1024*10);
      SchedRunTask(sched,t);
      assert(t->status == SCHED_PENDING);
    }
    fprintf(stderr,"%d\n",sched->size);

    if(Resume(sched)) {
      for( Task* t = SchedTail(*sched)->next ; t != SchedTail(*sched); ) {
        Task* n = t->next;
        SchedRunTask(sched,t);
        if(t->status == SCHED_DEAD)
          SchedDeleteTask(sched,t);
        t = n;
      }
    }
  }
  return 0;
}

int main() {
  Sched* sched = SchedCreate(Scheduler,NULL);
  srand(0);
  assert(SchedStart(sched,1) == 0);
  return 0;
}
