SECTION .data
SECTION .text

global SwapContext
global RunInit
global YieldContext

extern GetScheduler

;; The only trick here is how to easily maintain the ABI requirement of RSP register
;; which means you have to put it align with 16 bytes to satisfy sick SSE requirement
;; on X64 machine.
;; What I did is *do not mess up* stack here. Yes, I found it easier to write code by
;; not touching stack at all. All the context is based on either global varaibles or
;; other stuff. I personally feel it really simple and easy to write.

%macro save_context 1
	; rdi -> input  context
	; rsi -> output context
	; save callee saved registers
	mov qword[%1]   , rbx
	mov qword[%1+8] , rbp
	mov qword[%1+16], r12
	mov qword[%1+24], r13
	mov qword[%1+32], r14
	mov qword[%1+40], r15
	; sse status register
	stmxcsr dword[%1+48]
	; save rsp for this coroutine
	mov qword[%1+56],rsp
	; save return address
	mov rax, qword [rsp]
	mov qword[%1+64], rax
%endmacro

%macro restore_context 1
	;; dump the output context into callee saved registers
	mov rbx, qword[%1]
	mov rbp, qword[%1+8]
	mov r12, qword[%1+16]
	mov r13, qword[%1+24]
	mov r14, qword[%1+32]
	mov r15, qword[%1+40]
	; restore sse status register
	ldmxcsr  dword[%1+48]
	; use new rsp
	mov rsp, qword[%1+56]
%endmacro

;; the shit is we jump back to the callee saved return address however
;; the stack has that return address as well so we will have a double
;; return which is absolutely wrong. Here when we do resume, I manually
;; adjust the stack register rsp by one qword to omit the return address
%macro resume 1
	add rsp,8
	jmp qword[%1+64]
%endmacro

YieldContext:
	; rdi --> from
	; rsi --> to
	save_context    rdi
	restore_context rsi
	resume rsi

SwapContext:
  ; save the context
	save_context    rdi
	restore_context rsi

	; do the call
	resume rsi

RunInit:
  ; save the context
	save_context    rdi
	restore_context rsi

	; do the call
	mov rdi, qword [rsi+72]
	call qword [rsi+64]

	; when we return here it means we are able to
	; finish the function call normally without a
	; context switching involved.
	call GetScheduler
	restore_context rax

	; return from the functions
	resume rax
