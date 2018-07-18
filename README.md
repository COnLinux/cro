A Userspace Cooperative Scheduler Demonstration
==================================================

This small library/demo shows how to implement a user space schedulable function. It is inspired by coroutine and I
personally feel very interested in writing a high performance web server using coroutine.

The demo shows a scheduler which act as a user space kernel and bunch of scheduled function. A function's runtime
information is called a task and they are executed in their own *stack* , when a function wants to be cooperatively
preempted, it can call YieldTask , like coroutine's yield function. Then the context will be swapped back to the
scheduler function to let it do other job or find a pending task and reschedule it. See unittest/main-test.c for detail.

To build it, you need to install nasm

make test will give you the test sample.
