SECTION .data
SECTION .text

global _CoRoutineSwap
global _CoRoutineInit

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

%macro resume 0
	ret
%endmacro

_CoRoutineSwap:
  ; save the context
	save_context    rdi
	restore_context rsi
	resume

;; when we call this function it means we are the very first time on the
;; calling functions so our RSP must be aligned properly
_CoRoutineInit:
  ; save the context
	save_context    rdi
	restore_context rsi

  ; NOTES: here the stack is carefully matained to be aligned with 16 bytes
	sub rsp, 16
	mov qword[rsp], rdi

	; do the call
	mov rax, rsi
	mov rdi, qword [rax+72]
	mov rsi, qword [rax+80]
	call qword[rax+64]

	; when we return here it means we are able to
	; finish the function call normally without a
	; context switching involved.
	mov rax, qword[rsp]
	restore_context rax

	; return from the functions
	resume
