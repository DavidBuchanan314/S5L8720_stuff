.extern main
.global _start

.text
.code 16
_start:

	@ TODO: maybe flush caches?


	@ set up stack

	mov r0, #0
	mov lr, r0
	ldr r0, =0x22024000
	mov sp, r0
	ldr r0, =0xbadf00d
	push {r0}


	@ call main

	mov r0, #0  @ argc
	mov r1, #0  @ argv
	mov r2, #0  @ envp
	bl main


	@ hang forever

	b .
