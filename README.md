Customized C coroutine library
=======================================

This is a customized C coroutine library. It has a special model for coroutine scheduling. Essentially the coroutine
implemented here is not sysmetric but have master and slave. A master coroutine is called scheduler , it will be used to
spawn and resume suspended coroutine ; another thing is if a coroutine yield itself it will come back to this scheduler.
A slave itself is a coroutine that has its isolated stack spaces. Each coroutine runs in its own stack spaces and can
call Yield at anytime to go back to the scheduler. This model is super useful for implementing IO related code. The
scheduler can be acceptor or function blocks in epoll/poll/select ; and the slave is handler to handle each connection.
If a slave fails to make any prograss, then it yields back to the scheduler and once the desired event happened the
scheduler can yield back to the slave function/routine. I am super interested in using the coroutine method to write a
event driven web server.

The current way to write event driven code is either do continuation passing style or wraps states in a large switch
case , essentially a state machine. None of them looks close to elegant. With coroutine style yielding user can write
blocking code but have none blocking semantic internally.


# Build

You need nasm to build the source. Once nasm is installed, just do make


# Example
Checkout the unittest folder to see samples/examples. It is not really unittest but just test sample
